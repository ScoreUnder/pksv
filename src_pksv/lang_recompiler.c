#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

#include "binarysearch.h"
#include "binarysearch_u32.h"
#include "uint32_interval.h"
#include "lang_recompiler.h"
#include "lang_default_parsers.h"
#include "lang_parsers.h"
#include "lang_load.h"
#include "language-defs.h"
#include "textutil.h"
#include "codeproc.h"
#include "romutil.h"

struct compiler_internal_state {
  const struct language_def *curr_language;
  const struct language_def *base_language;
  struct language_cache *language_cache;
  struct parser_cache *parser_cache;
  codeblock *tail_block;
  struct bsearch_root free_space;
  struct bsearch_root erased_space;
  size_t line;
  uint32_t dynamic_base;
};

typedef struct {
  size_t len;
  char str[];
} len_string;

static const struct language lang_defines = {
    .name = "defines",
    .is_prefixed = false,
};

static const struct parser_list normal_parsers = {
    .length = 1,
    .parsers = &lang_defines,
};

static void compile_line(struct compiler_internal_state *state,
                         const char *line);
static char *parse_compiler_directive(struct compiler_internal_state *state,
                                      const char *directive, const char *cur);
static void set_language_from_token(struct compiler_internal_state *state,
                                    const char *token);
static void set_define_from_value(struct compiler_internal_state *state,
                                  const char *name, uint32_t value_int);
static struct parse_result parse_from_token(
    struct compiler_internal_state *state, const char *token_lang,
    const struct parser_list default_parsers, const len_string *token);
static void end_command(struct compiler_internal_state *state, const char *cur);
static struct parse_result pull_and_parse(
    struct compiler_internal_state *state,
    const struct parser_list default_parsers, const char **const cur);
static len_string *pull_token(const char *text);
static void cleanup_parse_result(struct parse_result result);
static void write_int_to_codeblock(struct compiler_internal_state *state,
                                   uint32_t data, size_t len);
static void warn_if_truncated(struct compiler_internal_state *state,
                              uint32_t value, size_t len);
static struct bsearch_root *get_defines(struct compiler_internal_state *state);

void compile_all(FILE *input_file, FILE *output_file,
                 const struct language_def *start_language,
                 struct language_cache *language_cache,
                 struct parser_cache *parser_cache) {
  struct compiler_internal_state state = {
      .curr_language = start_language,
      .base_language = start_language,
      .language_cache = language_cache,
      .parser_cache = parser_cache,
      .tail_block = NULL,
      .line = 1,
  };

  // XXX: Bit of a hack because I don't want to inspect the ROM file again
  // people should be using #dynamic anyway
  if (strcmp(start_language->name, "gs") == 0 ||
      strcmp(start_language->name, "cry") == 0) {
    state.dynamic_base = FREESPACE_START_GBC;
  } else {
    state.dynamic_base = FREESPACE_START_GBA;
  }

  uint32_interval_init_bsearch_root(&state.free_space);
  uint32_interval_init_bsearch_root(&state.erased_space);

  const size_t buffer_size = 0x400;  // Arbitrary
  char *buffer = malloc(buffer_size);

  while (fgets(buffer, buffer_size, input_file) != NULL) {
    // TODO: Check for oversize lines
    compile_line(&state, buffer);
    state.line++;
  }

  free(buffer);

  for (codeblock *c = state.tail_block; c != NULL; c = c->prev) {
    // TODO: handle dynamic offsets before this loop
    if (c->name != NULL) continue;
    uint32_interval_remove(&state.erased_space, c->org, c->org + c->size);
  }
  // TODO: erase what remains
  // TODO: write
  (void)output_file;
}

#define VALID_SPACES " \t"
#define TOKEN_SEPARATORS " \t\r\n:\'"

void compile_line(struct compiler_internal_state *state, const char *line) {
  const char *cur = line;
  cur += strspn(cur, VALID_SPACES);
  switch (cur[0]) {
    case '\'':
      // Comment
      break;
    case '\n':
      // Empty line
      break;
    case '#': {
      // Compiler directive
      cur++;                             // Skip '#'
      cur += strspn(cur, VALID_SPACES);  // Skip spaces
      len_string *token = pull_token(cur);
      cur += token->len;
      cur += strspn(cur, VALID_SPACES);  // Skip spaces
      cur = parse_compiler_directive(state, token->str, cur);
      free(token);
      end_command(state, cur);
      break;
    }
    case ':': {
      // Label
      cur++;  // Skip ':'

      // TODO
      len_string *token = pull_token(cur);
      cur += token->len;

      printf("label :%s\n", token->str);
      free(token);

      end_command(state, cur);
      break;
    }
    default: {
      // Any other command (not a special case)
      // TODO
      len_string *token = pull_token(cur);
      cur += token->len;

      len_string *namespace;
      len_string *command;

      if (*cur == ':') {
        namespace = token;

        cur++;  // Skip ':'
        command = pull_token(cur);
        cur += command->len;
      } else {
        namespace = NULL;
        command = token;
      }

      printf("command %s:%s\n", namespace == NULL ? "<null>" : namespace->str,
             command->str);
      (void)state;
      free(namespace);
      free(command);
      break;
    }
  }
}

char *parse_compiler_directive(struct compiler_internal_state *state,
                               const char *directive, const char *cur) {
  printf("compiler directive #%s\n", directive);
  // TODO: use gperf to generate a hash table for this
  if (strcmp(directive, "language") == 0 || strcmp(directive, "lang") == 0) {
    len_string *token = pull_token(cur);
    cur += token->len;
    set_language_from_token(state, token->str);
    free(token);
  } else if (strcmp(directive, "org") == 0) {
    len_string *org_token = pull_token(cur);
    cur += org_token->len;

    char *org_name;
    uint32_t org_address;
    if (org_token->str[0] == '@') {
      // Label
      org_name = org_token->str + 1;
      org_address = state->dynamic_base;
    } else {
      // Number
      org_name = NULL;
      cur -= org_token->len;
      struct parse_result result = pull_and_parse(state, normal_parsers, &cur);
      if (result.type == PARSE_RESULT_VALUE) {
        org_address = result.value;
      } else {
        cleanup_parse_result(result);
        fprintf(stderr,
                "Error: expected number or @label for #org directive "
                "at line %zu\n",
                state->line);
        exit(1);
      }
    }

    state->tail_block = add_codeblock(state->tail_block, org_name, org_address);
    free(org_token);

    cur += strspn(cur, VALID_SPACES);  // Skip spaces
    len_string *lang_token = pull_token(cur);
    if (lang_token->len != 0) {
      set_language_from_token(state, lang_token->str);
      cur += lang_token->len;
    }
    free(lang_token);
  } else if (strcmp(directive, "dynamic") == 0 ||
             strcmp(directive, "dyn") == 0) {
    struct parse_result result = pull_and_parse(state, normal_parsers, &cur);
    if (result.type == PARSE_RESULT_VALUE) {
      state->dynamic_base = result.value;
    } else {
      cleanup_parse_result(result);
      fprintf(stderr, "Error: Could not parse dynamic base at line %zu\n",
              state->line);
      exit(1);
    }
  } else if (strcmp(directive, "define") == 0 || strcmp(directive, "d") == 0) {
    len_string *name_token = pull_token(cur);
    cur += name_token->len;
    cur += strspn(cur, VALID_SPACES);  // Skip spaces

    struct parse_result result = pull_and_parse(state, normal_parsers, &cur);
    if (result.type == PARSE_RESULT_VALUE) {
      set_define_from_value(state, name_token->str, result.value);
    } else {
      cleanup_parse_result(result);
      fprintf(stderr, "Error: Could not parse value for define at line %zu\n",
              state->line);
      exit(1);
    }
    free(name_token);
    // } else if (strcmp(directive, "include") == 0) {
    // TODO: include
  } else if (strcmp(directive, "raw") == 0) {
    struct parse_result result = pull_and_parse(state, normal_parsers, &cur);
    if (result.type == PARSE_RESULT_VALUE) {
      write_int_to_codeblock(state, result.value, 1);
    } else {
      cleanup_parse_result(result);
      fprintf(stderr, "Error: Could not parse value for #raw at line %zu\n",
              state->line);
      exit(1);
    }
  } else if (strcmp(directive, "erase") == 0) {
    struct parse_result result = pull_and_parse(state, normal_parsers, &cur);
    uint32_t erase_begin, erase_end;
    if (result.type == PARSE_RESULT_VALUE) {
      erase_begin = result.value;
    } else {
      cleanup_parse_result(result);
      fprintf(stderr,
              "Error: Could not parse start value for #erase at line %zu\n",
              state->line);
      exit(1);
    }

    cur += strspn(cur, VALID_SPACES);  // Skip spaces
    result = pull_and_parse(state, normal_parsers, &cur);
    if (result.type == PARSE_RESULT_VALUE) {
      erase_end = result.value;
    } else {
      cleanup_parse_result(result);
      fprintf(stderr,
              "Error: Could not parse end value for #erase at line %zu\n",
              state->line);
      exit(1);
    }

    if (erase_begin > erase_end) {
      fprintf(stderr,
              "Error: erase start (0x%08x) must come before erase end (0x%08x) "
              "at line %zu\n"
              "Did you mean: #erase 0x%08x 0x%08x\n",
              erase_begin, erase_end, state->line, erase_begin,
              erase_begin + erase_end);
      exit(1);
    }

    if (erase_end > GBA_MAX_ROM_SIZE) {
      fprintf(stderr,
              "Warning: Erase end (0x%08x) is larger than the maximum ROM size "
              "(0x%08x) at line %zu\n",
              erase_end, GBA_MAX_ROM_SIZE, state->line);
    }

    uint32_interval_add(&state->free_space, erase_begin, erase_end);
    uint32_interval_add(&state->erased_space, erase_begin, erase_end);
    // } else if (strcmp(directive, "pragma") == 0) {
    // TODO: pragma freespace START END
    //       pragma no-project-file (maybe?)
    //       pragma verbose [on|off]
    //       pragma warnings [on|off]
  } else if (directive[0] == '!') {
    // Shebang, do nothing
    cur += strlen(cur);
  } else {
    fprintf(stderr, "Error: Unknown compiler directive '#%s' on line %zu\n",
            directive, state->line);
    exit(1);
  }
  return cur;
}

void end_command(struct compiler_internal_state *state, const char *cur) {
  cur += strspn(cur, VALID_SPACES);  // Skip spaces
  if (*cur == '\'') {
    // Comment, always valid
    return;
  }

  if (*cur == '\r') cur++;
  if (*cur == '\n') cur++;

  if (*cur != '\0') {
    fprintf(stderr,
            "Error: Extra characters after command on line %zu. Content: %s\n",
            state->line, cur);
    exit(1);
  }
}

void set_language_from_token(struct compiler_internal_state *state,
                             const char *token) {
  const struct language_def *new_language =
      get_language(state->language_cache, token);
  if (new_language == NULL) {
    fprintf(stderr, "Error: Unknown language '%s' on line %zu\n", token,
            state->line);
    exit(1);
  }
  state->curr_language = new_language;
  state->base_language = new_language;
}

void set_define_from_value(struct compiler_internal_state *state,
                           const char *name, uint32_t value_int) {
  struct bsearch_root *defines_root = get_defines(state);

  ptrdiff_t index = bsearch_find(defines_root, name);
  if (index >= 0) {
    fprintf(stderr, "Warning: redefining %s on line %zu.", name, state->line);
    bsearch_setval_u32(defines_root, index, value_int);
  } else {
    bsearch_unsafe_insert(defines_root, index, defines_root->copy(name),
                          CAST_u32_pvoid(value_int));
  }
}

struct parse_result parse_from_token(struct compiler_internal_state *state,
                                     const char *token_lang,
                                     const struct parser_list default_parsers,
                                     const len_string *token) {
  struct parser_list parsers;
  if (token_lang == NULL) {
    parsers = default_parsers;
  } else {
    parsers = (struct parser_list){
        .length = 1,
        .parsers = malloc(sizeof(struct language)),
    };
    bool is_prefixed = token_lang != NULL && token_lang[0] == '*';
    if (is_prefixed) {
      token_lang++;
    }
    parsers.parsers[0] = (struct language){
        .name = (char *)token_lang,
        .is_prefixed = is_prefixed,
    };
  }

  struct parse_result result =
      parse_for_recomp(state->parser_cache, state->curr_language, parsers,
                       token->str, token->len);
  if (token_lang != NULL) {
    free(parsers.parsers);
  }
  return result;
}

struct parse_result pull_and_parse(struct compiler_internal_state *state,
                                   const struct parser_list default_parsers,
                                   const char **const cur) {
  len_string *value_token = pull_token(*cur);
  *cur += value_token->len;

  len_string *namespace_token = NULL;
  if (**cur == ':') {
    (*cur)++;  // Skip ':'
    namespace_token = value_token;
    value_token = pull_token(*cur);
    *cur += value_token->len;
  }

  char *namespace = namespace_token == NULL ? NULL : namespace_token->str;
  struct parse_result result =
      parse_from_token(state, namespace, default_parsers, value_token);
  free(value_token);
  free(namespace_token);
  return result;
}

len_string *pull_token(const char *text) {
  size_t token_len = strcspn(text, TOKEN_SEPARATORS);
  len_string *token = malloc(sizeof(len_string) + token_len + 1);
  token->len = token_len;
  memcpy(token->str, text, token_len);
  token->str[token_len] = '\0';
  strtolower(token->str);
  return token;
}

void cleanup_parse_result(struct parse_result result) {
  if (result.type == PARSE_RESULT_LABEL) {
    free(result.label);
  }
}

void write_int_to_codeblock(struct compiler_internal_state *state,
                            uint32_t data, size_t len) {
  assert(len != 0);
  assert(len <= 4);
  warn_if_truncated(state, data, len);

  codeblock *block = state->tail_block;
  if (block == NULL) {
    fprintf(stderr, "Error: Writing code before #org on line %zu\n",
            state->line);
    exit(1);
  }

#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  // On little endian systems, the bytes are already in the correct order
  void *raw_data = &data;
#else
  // On other endiannesses, emulate little endian byte order
  uint8_t raw_data[4];
  for (size_t i = 0; i < len; i++) {
    raw_data[i] = (data >> (i * 8)) & 0xFF;
  }
#endif
  add_data(block, raw_data, len);
}

void warn_if_truncated(struct compiler_internal_state *state, uint32_t value,
                       size_t len) {
  int32_t signed_value = (int32_t)value;
  signed_value >>= len * 8;
  if (signed_value != 0 && signed_value != -1) {
    fprintf(stderr,
            "Warning: Truncating value 0x%08" PRIx32
            " to %zu bytes on line %zu\n",
            value, len, state->line);
  }
}

struct bsearch_root *get_defines(struct compiler_internal_state *state) {
  struct loaded_or_builtin_parser *defines_parser =
      get_parser(state->parser_cache, "defines", false);
  assert(defines_parser != NULL);
  assert(defines_parser->which == PARSER_TYPE_LOADED);
  return &defines_parser->loaded.lookup_by_name;
}
