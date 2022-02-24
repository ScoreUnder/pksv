#ifndef PKSV_LANGUAGE_DEFS_H
#define PKSV_LANGUAGE_DEFS_H 1

#include <stdint.h>
#include <stdbool.h>

/*

Flat file format:

1b: metaflags
1b: parents.length
[ for parent * parents.length
    1b strlen(parent)
    <string> parent
]

1b rules.length
[ for rule * rules.length
    1b rule.attributes

    1b rule.bytes.length
    <rule.bytes.length blob> rule.bytes.bytes

    1b strlen(rule.command_name)
    <string> rule.command_name

    1b strlen(rule.oneshot_lang.name)
    <string> rule.oneshot_lang.name
    [ if rule.oneshot_lang.name is not empty
        1b rule.oneshot_lang.is_prefixed
    ]

    1b rule.args.length
    [ for arg * rule.args.length
        1b arg.size

        1b arg.parsers.length
        [ for parser * arg.parsers.length
            1b strlen(parser.name)
            <string> parser.name
            1b parser.is_prefixed
        ]

        1b arg.as_language.type
        [ if arg.as_language.type is LC_TYPE_LANG
            1b strlen(arg.as_language.lang.name)
            <string> arg.as_language.lang.name
        ]
        [ if arg.as_language.type is LC_TYPE_COMMAND
            1b strlen(arg.as_language.command)
            <string> arg.as_language.command
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

#define METAFLAG_NONE 0
#define METAFLAG_MASK_LANGTYPE 3
#define METAFLAG_LANGTYPE_LINE 1
#define METAFLAG_LANGTYPE_TEXT 2

struct bytes_list {
  const uint8_t *bytes;
  size_t length;
};

struct language {
  const char *name;
  bool is_prefixed;
};

struct parser_list {
  const struct language *parsers;
  size_t length;
};

struct lang_or_cmd {
  int type;
  union {
    struct language lang;
    const char *command;
  };
};

struct command_arg {
  size_t size;
  struct parser_list parsers;
  struct lang_or_cmd as_language;
};

struct command_args_list {
  const struct command_arg *args;
  size_t length;
};

struct rule {
  struct bytes_list bytes;
  struct command_args_list args;
  struct language oneshot_lang;
  const char *command_name;
  uint8_t attributes;
};

struct language_def {
  const struct bsearch_root *rules_by_bytes;
  const struct bsearch_root *rules_by_command_name;
  const char **parents;
  uint8_t meta_flags;
};

#endif
