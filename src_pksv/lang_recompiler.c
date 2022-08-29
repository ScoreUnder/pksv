#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "lang_recompiler.h"
#include "lang_default_parsers.h"
#include "lang_parsers.h"
#include "lang_load.h"
#include "language-defs.h"
#include "textutil.h"

struct compiler_internal_state {
  const struct language_def *curr_language;
  const struct language_def *base_language;
  struct language_cache *language_cache;
  struct parser_cache *parser_cache;
  size_t line;
  uint32_t dynamic_base;
};

typedef struct {
  size_t len;
  char str[];
} len_string;

#define DYN_UNSPECIFIED ((uint32_t)-1)

void compile_line(struct compiler_internal_state *state, const char *line);
len_string *pull_token(const char *text);

void compile_all(FILE *input_file, FILE *output_file,
                 const struct language_def *start_language,
                 struct language_cache *language_cache,
                 struct parser_cache *parser_cache) {
  struct compiler_internal_state state = {
      .curr_language = start_language,
      .base_language = start_language,
      .language_cache = language_cache,
      .parser_cache = parser_cache,
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

      // TODO: parse compiler directive
      len_string *token = pull_token(cur);
      printf("compiler directive #%s\n", token->str);
      free(token);
      break;
    }
    case ':': {
      // Label
      cur++;  // Skip ':'

      // TODO
      len_string *token = pull_token(cur);
      printf("label :%s\n", token->str);
      free(token);
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

len_string *pull_token(const char *text) {
  size_t token_len = strcspn(text, TOKEN_SEPARATORS);
  len_string *token = malloc(sizeof(len_string) + token_len + 1);
  token->len = token_len;
  memcpy(token->str, text, token_len);
  token->str[token_len] = '\0';
  strtolower(token->str);
  return token;
}
