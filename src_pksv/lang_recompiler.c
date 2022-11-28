#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

#include "binarysearch.h"
#include "binarysearch_u32.h"
#include "lang_recompiler.h"
#include "lang_default_parsers.h"
#include "lang_parsers.h"
#include "lang_load.h"
#include "language-defs.h"
#include "textutil.h"
#include "codeproc.h"

struct compiler_internal_state {
  const struct language_def *curr_language;
  const struct language_def *base_language;
  struct language_cache *language_cache;
  struct parser_cache *parser_cache;
  codeblock *tail_block;
  size_t line;
  uint32_t dynamic_base;
};

typedef struct {
  size_t len;
  char str[];
} len_string;

static const struct parser_list empty_parser_list = {0};

#define DYN_UNSPECIFIED ((uint32_t)-1)

static void compile_line(struct compiler_internal_state *state,
                         const char *line);
static char *parse_compiler_directive(struct compiler_internal_state *state,
                                      const char *directive, char *cur);
static void set_language_from_token(struct compiler_internal_state *state,
                                    const char *token);
static void set_define_from_value(struct compiler_internal_state *state,
                                  const char *name, uint32_t value_int);
static struct parse_result parse_from_token(
    struct compiler_internal_state *state, const char *token_lang,
    const struct parser_list default_parsers, const len_string *token);
static void end_command(struct compiler_internal_state *state, char *cur);
static struct parse_result pull_and_parse(
    struct compiler_internal_state *state,
    const struct parser_list default_parsers, const char **const cur);
static len_string *pull_token(const char *text);
static void cleanup_parse_result(struct parse_result result);

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
      .dynamic_base = DYN_UNSPECIFIED,
  };

  const size_t buffer_size = 0x400;  // Arbitrary
  char *buffer = malloc(buffer_size);

  while (fgets(buffer, buffer_size, input_file) != NULL) {
    // TODO: Check for oversize lines
    compile_line(&state, buffer);
    state.line++;
  }

  free(buffer);

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
                               const char *directive, char *cur) {
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
      // TODO: look up findfroms to set dynamic base if necessary
      org_address = state->dynamic_base;
    } else {
      // Number
      org_name = NULL;
      cur -= org_token->len;
      struct parse_result result =
          pull_and_parse(state, empty_parser_list, &cur);
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
    }
    free(lang_token);
  } else if (strcmp(directive, "dynamic") == 0 ||
             strcmp(directive, "dyn") == 0) {
    // TODO: disallow reassignment
    struct parse_result result = pull_and_parse(state, empty_parser_list, &cur);
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

    struct parse_result result = pull_and_parse(state, empty_parser_list, &cur);
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
    // } else if (strcmp(directive, "raw") == 0) {
    // TODO: raw
    // } else if (strcmp(directive, "erase") == 0) {
    // TODO: erase
    // } else if (strcmp(directive, "pragma") == 0) {
    // TODO: pragma freespace START END
    //       pragma no-project-file (maybe?)
    //       pragma verbose [on|off]
    //       pragma warnings [on|off]
  } else {
    fprintf(stderr, "Error: Unknown compiler directive '#%s' on line %zu\n",
            directive, state->line);
    exit(1);
  }
  return cur;
}

void end_command(struct compiler_internal_state *state, char *cur) {
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
  if (strcmp(name, "findfrom") == 0 || strcmp(name, "findfromgold") == 0) {
    fprintf(stderr,
            "Warning: redefining %s on line %zu. "
            "Use \"#dynamic 0x%08" PRIx32 "\" instead.\n",
            name, state->line, value_int);
  } else {
    struct loaded_or_builtin_parser *defines_parser =
        get_parser(state->parser_cache, "defines", false);
    assert(defines_parser != NULL);
    assert(defines_parser->which == PARSER_TYPE_LOADED);
    struct bsearch_root *defines_root = &defines_parser->loaded.lookup_by_name;

    ptrdiff_t index = bsearch_find(defines_root, name);
    if (index >= 0) {
      fprintf(stderr, "Warning: redefining %s on line %zu.", name, state->line);
      bsearch_setval_u32(defines_root, index, value_int);
    } else {
      bsearch_unsafe_insert(defines_root, index, defines_root->copy(name),
                            CAST_u32_pvoid(value_int));
    }
  }
}

struct parse_result parse_from_token(struct compiler_internal_state *state,
                                     const char *token_lang,
                                     const struct parser_list default_parsers,
                                     const len_string *token) {
  bool is_prefixed = token_lang != NULL && token_lang[0] == '*';
  if (is_prefixed) {
    token_lang++;
  }

  struct parser_list parsers;
  if (token_lang == NULL) {
    parsers = default_parsers;
  } else {
    parsers = (struct parser_list){
        .length = default_parsers.length + 1,
        .parsers =
            malloc(sizeof(struct language) * (default_parsers.length + 1)),
    };
    parsers.parsers[0] = (struct language){
        .name = token_lang,
        .is_prefixed = is_prefixed,
    };
    memcpy(parsers.parsers + 1, default_parsers.parsers,
           sizeof(struct language) * default_parsers.length);
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
