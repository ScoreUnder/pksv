%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "language-defs-builder.h"

extern int yylex(void);
extern int yylineno;
extern FILE* yyin;
void yyerror(const char*);

struct command_args_list_builder add_command_args_list(struct command_args_list_builder, struct command_arg_builder);
struct meta_list_builder add_meta_list(struct meta_list_builder, struct meta);
struct rule_list_builder add_rule_list(struct rule_list_builder, struct rule_builder);
struct parser_list_builder add_parser_list(struct parser_list_builder, struct language);
struct bytes_list_builder add_bytes_list(struct bytes_list_builder, uint8_t);

int error_count;
struct language_def_builder language_def;
%}

%union {
  int intval;
  const char * strval;
  struct meta_list_builder meta_list;
  struct rule_list_builder rule_list;
  struct meta meta;
  struct rule_builder rule;
  struct bytes_list_builder bytes_list;
  uint8_t attributes;
  struct command_args_list_builder command_args_list;
  struct language language;
  struct command_arg_builder command_arg;
  struct parser_list_builder parser_list;
  struct lang_or_cmd lang_or_cmd;
}

%token <strval> T_identifier
%token <strval> T_string
%token <intval> T_integer
%token T_error
%type <meta_list> metadatas
%type <rule_list> rule_defs
%type <meta> metadata
%type <rule> rule_def
%type <bytes_list> bytes
%type <attributes> attributes
%type <command_args_list> args
%type <language> oneshot_lang language
%type <command_arg> arg
%type <parser_list> parsers
%type <strval> textval
%type <intval> byte
%type <lang_or_cmd> as_language

%%

language_def: newlines metadatas rule_defs { language_def = (struct language_def_builder) {$2, $3}; }
            ;

metadatas: /*empty*/ { $$ = (struct meta_list_builder) {NULL, 0, 0}; }
         | metadatas metadata newlines { $$ = add_meta_list($1, $2); }
         ;

rule_defs: /*empty*/ { $$ = (struct rule_list_builder) {NULL, 0, 0}; }
         | rule_defs rule_def newlines { $$ = add_rule_list($1, $2); }
         ;

metadata: T_identifier textval '\n' {
            $$ = (struct meta) { .name = $1, .value = $2 };
          }
        | error '\n' { yyerror("Bad metadata definition"); yyerrok; }
        ;

rule_def: '[' bytes ']' '=' textval attributes args oneshot_lang '\n' {
            $$ = (struct rule_builder) {
              .bytes = $2,
              .command_name = $5,
              .attributes = $6,
              .args = $7,
              .oneshot_lang = $8,
            };
          }
        | error '\n' { yyerror("Bad rule definition"); yyerrok; }
        ;

bytes: /*empty*/ { $$ = (struct bytes_list_builder) {NULL, 0, 0}; }
     | bytes byte { $$ = add_bytes_list($1, $2); }
     ;

byte: T_integer {
        $$ = $1;
        if ($$ > 255 || $$ < 0) {
          yyerror("Byte value out of range");
          YYERROR;
        }
      }
    ;

attributes: /*empty*/ { $$ = RULE_ATTR_NONE; }
          | attributes '+' T_identifier {
              $$ = $1;
              if (strcmp($3, "end") == 0) {
                $$ |= RULE_ATTR_END;
              } else if (strcmp($3, "terminator")) {
                $$ |= RULE_ATTR_TERMINATOR;
              } else if (strcmp($3, "no_terminate")) {
                $$ |= RULE_ATTR_NO_TERMINATE;
              } else if (strcmp($3, "default")) {
                $$ |= RULE_ATTR_DEFAULT;
              } else {
                yyerror("Unknown attribute");
              }
            }
          ;

args: /*empty*/ { $$ = (struct command_args_list_builder) {NULL, 0, 0}; }
    | args arg { $$ = add_command_args_list($1, $2); }
    ;

oneshot_lang: /*empty*/ { $$.name = NULL; }
            | '!' language { $$ = $2; }
            ;

arg: T_integer parsers as_language {
        $$ = (struct command_arg_builder) {
          .size = $1,
          .parsers = $2,
          .as_language = $3,
        };
      }
   ;

parsers: /*empty*/ { $$ = (struct parser_list_builder) {NULL, 0, 0}; }
       | parsers ':' language { $$ = add_parser_list($1, $3); }
       | parsers ':' error { yyerror("Bad arg parser type"); yyerrok; }
       ;

as_language: /*empty*/ { $$.type = LC_TYPE_NONE; }
            | '@' language {
                $$.type = LC_TYPE_LANG;
                $$.lang = $2;
              }
            | '@' T_identifier '=' textval {
                if (strcmp($2, "cmd")) {
                  yyerror("Expected @cmd=, got something else");
                  YYERROR;
                }
                $$.type = LC_TYPE_COMMAND;
                $$.command = $4;
              }
            | '@' error { yyerror("Bad language type"); }
            ;

language: T_identifier { $$.is_prefixed = false; $$.name = $1; }
        | '*' T_identifier { $$.is_prefixed = true; $$.name = $2; }
        | '*' { $$.is_prefixed = true; $$.name = strdup(""); }
        ;

textval: T_identifier { $$ = $1; }
       | T_string { $$ = $1; }
       ;

newlines: /*empty*/
        | newlines '\n'
        ;

%%

#define MAKE_ADD(structtype, itemtype, items) \
    struct structtype##_builder add_##structtype(struct structtype##_builder list, itemtype item) { \
      list.length++; \
      if (list.length >= list.capacity) { \
        if (list.capacity == 0) { \
          list.capacity = 4; \
        } else { \
          list.capacity *= 2; \
        } \
        list.items = realloc(list.items, list.capacity * sizeof(itemtype)); \
      } \
      list.items[list.length - 1] = item; \
      return list; \
    }

MAKE_ADD(command_args_list, struct command_arg_builder, args)
MAKE_ADD(meta_list, struct meta, metas)
MAKE_ADD(rule_list, struct rule_builder, rules)
MAKE_ADD(parser_list, struct language, parsers)
MAKE_ADD(bytes_list, uint8_t, bytes)

struct string_list_builder {
  const char **items;
  size_t length;
  size_t capacity;
};

MAKE_ADD(string_list, const char*, items)

void yyerror(const char* err) {
  error_count++;
  fprintf(stderr, "Error: %s at line %d\n", err, yylineno - 1 /* errors are detected after newline */);
}

bool run_parser(FILE *file) {
  error_count = 0;
  yyin = file;
  int result = yyparse();
  return result == 0 && error_count == 0;
}

bool sanity_check(struct language_def_builder *language, struct string_list_builder *parents) {
  if (parents->length > 255) {
    fputs("Too many inherit clauses\n", stderr);
    return false;
  }

  for (size_t i = 0; i < parents->length; i++) {
    const char *parent = parents->items[i];
    if (strlen(parent) > 255) {
      fprintf(stderr, "Inherited language name too long: \"%s\"\n", parent);
      return false;
    }
  }

  for (size_t i = 0; i < language->rules.length; i++) {
    const struct rule_builder *rule = &language->rules.rules[i];
    if (strlen(rule->command_name) > 255) {
      fprintf(stderr, "Rule name too long: \"%s\"\n", rule->command_name);
      return false;
    }
    if (rule->bytes.length > 255) {
      fprintf(stderr, "Rule has too many bytes: \"%s\"\n", rule->command_name);
      return false;
    }
    if (rule->oneshot_lang.name != NULL && strlen(rule->oneshot_lang.name) > 255) {
      fprintf(stderr, "Rule oneshot language name too long: \"%s\"\n", rule->oneshot_lang.name);
      return false;
    }
    if (rule->oneshot_lang.name != NULL && strlen(rule->oneshot_lang.name) == 0) {
      fprintf(stderr, "Rule oneshot language cannot be recursive (use a different language type) on \"%s\"\n", rule->command_name);
      return false;
    }
    if (rule->args.length > 255) {
      fprintf(stderr, "Rule has too many args: \"%s\"\n", rule->command_name);
      return false;
    }
    for (size_t j = 0; j < rule->args.length; j++) {
      const struct command_arg_builder *arg = &rule->args.args[j];
      if (arg->size > 4) {
        fprintf(stderr, "WARNING: Rule arg consumes too many bytes: \"%s\"\n", rule->command_name);
        // return false;  // Not a strict limit on file format
      }
      if (arg->parsers.length > 255) {
        fprintf(stderr, "Rule arg has too many parsers: \"%s\" arg %d\n", rule->command_name, (int) j);
        return false;
      }
      for (size_t k = 0; k < arg->parsers.length; k++) {
        const struct language *parser = &arg->parsers.parsers[k];
        if (strlen(parser->name) > 255) {
          fprintf(stderr, "Rule arg parser name too long: \"%s\" arg %d parser \"%s\"\n", rule->command_name, (int) j, parser->name);
          return false;
        }
      }
      if (arg->as_language.type == LC_TYPE_LANG) {
        if (strlen(arg->as_language.lang.name) > 255) {
          fprintf(stderr, "Rule arg as-language name too long: \"%s\" arg %d as-language \"%s\"\n", rule->command_name, (int) j, arg->as_language.lang.name);
          return false;
        }
      } else if (arg->as_language.type == LC_TYPE_COMMAND) {
        if (strlen(arg->as_language.command) > 255) {
          fprintf(stderr, "Rule arg as-command name too long: \"%s\" arg %d as-command \"%s\"\n", rule->command_name, (int) j, arg->as_language.command);
          return false;
        }
      }
    }
  }
  return true;
}

int main(int argc, char **argv) {
#if YYDEBUG
  yydebug = 1;
#endif
  if (argc != 3) {
    fprintf(
      stderr,
      "Usage: %s <infile> <outfile>\n"
      "  infile: the language specification to parse\n"
      "  outfile: where to put the flattened binary file for PKSV to use\n"
      "  (the output file will be overwritten indiscriminately)\n"
      "e.g. %s inputs/lang_frlg.txt lang_frlg.dat\n",
      argv[0], argv[0]);
    return 1;
  }
  FILE *file = fopen(argv[1], "rt");
  if (!file) {
    perror(argv[1]);
    return 1;
  }
  bool success = run_parser(file);
  fclose(file);

  if (!success) {
    return 1;
  }

  uint8_t meta_flags = METAFLAG_NONE;
  struct string_list_builder parents = {NULL, 0, 0};

  for (size_t i = 0; i < language_def.metas.length; i++) {
    const struct meta meta = language_def.metas.metas[i];
    if (strcmp(meta.name, "inherit") == 0) {
      parents = add_string_list(parents, meta.value);
    } else if (strcmp(meta.name, "type") == 0) {
      if (strcmp(meta.value, "line") == 0) {
        meta_flags |= METAFLAG_LANGTYPE_LINE;
      } else if (strcmp(meta.value, "text") == 0) {
        meta_flags |= METAFLAG_LANGTYPE_TEXT;
      } else {
        fprintf(stderr, "Unknown language type \"%s\"\n", meta.value);
        return 1;
      }
    } else {
      fprintf(stderr, "Unknown metadata type \"%s\"\n", meta.name);
      return 1;
    }
  }

  if (!sanity_check(&language_def, &parents)) {
    return 1;
  }

  FILE *out = fopen(argv[2], "wb");
  if (!out) {
    perror(argv[2]);
    return 1;
  }

  // Metas
  fwrite(&meta_flags, 1, 1, out);
  uint8_t parents_count = (uint8_t) parents.length;
  fwrite(&parents_count, 1, 1, out);
  for (size_t i = 0; i < parents.length; i++) {
    uint8_t parent_length = (uint8_t) strlen(parents.items[i]);
    fwrite(&parent_length, 1, 1, out);
    fwrite(parents.items[i], 1, parent_length, out);
  }

  // Rules
  uint8_t rule_count = (uint8_t) language_def.rules.length;
  fwrite(&rule_count, 1, 1, out);
  for (size_t i = 0; i < language_def.rules.length; i++) {
    const struct rule_builder *rule = &language_def.rules.rules[i];

    fwrite(&rule->attributes, 1, 1, out);

    uint8_t bytes_length = (uint8_t) rule->bytes.length;
    fwrite(&bytes_length, 1, 1, out);
    fwrite(rule->bytes.bytes, 1, bytes_length, out);

    uint8_t command_name_len = (uint8_t) strlen(rule->command_name);
    fwrite(&command_name_len, 1, 1, out);
    fwrite(rule->command_name, 1, command_name_len, out);

    if (rule->oneshot_lang.name != NULL) {
      uint8_t oneshot_lang_len = (uint8_t) strlen(rule->oneshot_lang.name);
      fwrite(&oneshot_lang_len, 1, 1, out);
      fwrite(rule->oneshot_lang.name, 1, oneshot_lang_len, out);
    } else {
      uint8_t oneshot_lang_len = 0;
      fwrite(&oneshot_lang_len, 1, 1, out);
    }

    uint8_t args_length = (uint8_t) rule->args.length;
    fwrite(&args_length, 1, 1, out);
    for (size_t j = 0; j < rule->args.length; j++) {
      const struct command_arg_builder *arg = &rule->args.args[j];

      uint8_t arg_size = (uint8_t) arg->size;
      fwrite(&arg_size, 1, 1, out);

      uint8_t parsers_length = (uint8_t) arg->parsers.length;
      fwrite(&parsers_length, 1, 1, out);
      for (size_t k = 0; k < arg->parsers.length; k++) {
        const struct language *parser = &arg->parsers.parsers[k];

        uint8_t parser_len = (uint8_t) strlen(parser->name);
        fwrite(&parser_len, 1, 1, out);
        fwrite(parser->name, 1, parser_len, out);

        uint8_t is_prefixed = parser->is_prefixed;
        fwrite(&is_prefixed, 1, 1, out);
      }

      uint8_t as_language_type = (uint8_t) arg->as_language.type;
      fwrite(&as_language_type, 1, 1, out);
      if (arg->as_language.type == LC_TYPE_LANG) {
        uint8_t as_language_len = (uint8_t) strlen(arg->as_language.lang.name);
        fwrite(&as_language_len, 1, 1, out);
        fwrite(arg->as_language.lang.name, 1, as_language_len, out);
      } else if (arg->as_language.type == LC_TYPE_COMMAND) {
        uint8_t as_command_len = (uint8_t) strlen(arg->as_language.command);
        fwrite(&as_command_len, 1, 1, out);
        fwrite(arg->as_language.command, 1, as_command_len, out);
      }
    }
  }

  if (ferror(out)) {
    perror(argv[2]);
    unlink(argv[2]);
    return 1;
  }

  fclose(out);

  return 0;
}
