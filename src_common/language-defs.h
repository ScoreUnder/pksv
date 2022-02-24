#ifndef PKSV_LANGUAGE_DEFS_H
#define PKSV_LANGUAGE_DEFS_H 1

#include <stdint.h>
#include <stdbool.h>

/*

Flat file format:

vi: string_table.length
[ for string * string_table.length ]
  vi: string.length
  <string>
]

1b: metaflags
vi: parents.length
[ for parent * parents.length
    vi string index
]

vi rules.length
[ for rule * rules.length
    1b rule.attributes

    vi rule.bytes.length
    <rule.bytes.length blob> rule.bytes.bytes

    vi string index = rule.command_name

    vi string index = rule.oneshot_lang.name
    1b rule.oneshot_lang.is_prefixed

    vi rule.args.length
    [ for arg * rule.args.length
        1b arg.size

        vi arg.parsers.length
        [ for parser * arg.parsers.length
            vi string index = parser.name
            1b parser.is_prefixed
        ]

        1b arg.as_language.type
        [ if arg.as_language.type is LC_TYPE_LANG
            vi string index = arg.as_language.lang.name
            1b arg.as_language.lang.is_prefixed
        ]
        [ if arg.as_language.type is LC_TYPE_COMMAND
            vi string index = arg.as_language.command
        ]
    ]
]

*/

#define LC_TYPE_NONE 0
#define LC_TYPE_LANG 1
#define LC_TYPE_COMMAND 2

#define RULE_ATTR_NONE 0
#define RULE_ATTR_END 1
#define RULE_ATTR_TERMINATOR 2
#define RULE_ATTR_NO_TERMINATE 4
#define RULE_ATTR_DEFAULT 8
#define RULE_ATTR_CMP_FLAG 16
#define RULE_ATTR_CMP_INT 32

#define METAFLAG_NONE 0
#define METAFLAG_MASK_LANGTYPE 3
#define METAFLAG_LANGTYPE_LINE 1
#define METAFLAG_LANGTYPE_TEXT 2

struct bytes_list {
  uint8_t *bytes;
  size_t length;
};

struct language {
  char *name;
  bool is_prefixed;
};

struct parser_list {
  struct language *parsers;
  size_t length;
};

struct lang_or_cmd {
  int type;
  union {
    struct language lang;
    char *command;
  };
};

struct command_arg {
  size_t size;
  struct parser_list parsers;
  struct lang_or_cmd as_language;
};

struct command_args_list {
  struct command_arg *args;
  size_t length;
};

struct rule {
  struct bytes_list bytes;
  struct command_args_list args;
  struct language oneshot_lang;
  char *command_name;
  uint8_t attributes;
};

struct language_def {
  struct bsearch_root *rules_by_bytes;
  struct bsearch_root *rules_by_command_name;
  struct rule *default_rule;
  struct rule *terminator_rule;
  char **parents;
  uint8_t meta_flags;
};

#endif
