#ifndef PKSV_LANGUAGE_DEFS_H
#define PKSV_LANGUAGE_DEFS_H 1

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

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

        vi string index = arg.as_language.lang.name
        1b arg.as_language.lang.is_prefixed
        vi string index = arg.as_language.command
    ]
]

*/

#define RULE_ATTR_NONE 0
#define RULE_ATTR_END 1
#define RULE_ATTR_TERMINATOR 2
#define RULE_ATTR_NO_TERMINATE 4
#define RULE_ATTR_DEFAULT 8
#define RULE_ATTR_CMP_FLAG 16
#define RULE_ATTR_CMP_INT 32
#define RULE_ATTR_BREAK 64
#define RULE_ATTR_PREFER_BREAK 128

#define METAFLAG_NONE 0
#define METAFLAG_MASK_LANGTYPE 3
#define METAFLAG_LANGTYPE_LINE 1
#define METAFLAG_LANGTYPE_TEXT 2

#define SPECIAL_RULE_DEFAULT 0
#define SPECIAL_RULE_TERMINATOR 1
#define SPECIAL_RULE_NO_TERMINATE 2
#define NUM_SPECIAL_RULES 3

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

struct lang_cmd {
  struct language lang;
  char *command;
};

struct command_arg {
  size_t size;
  struct parser_list parsers;
  struct lang_cmd as_language;
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
  uint8_t refcnt;
};

struct byte_lookup_tables {
  // entry value of 0 = missing.
  uint8_t decomp[0x100];
  uint8_t recomp[0x100];
};

struct language_def {
  struct bsearch_root *rules_by_bytes;
  struct bsearch_root *rules_by_command_name;
  struct rule *special_rules[NUM_SPECIAL_RULES];
  struct byte_lookup_tables *lookup_bytes;  // if METAFLAG_LANGTYPE_TEXT
  char **parents;
  char *name;
  uint8_t meta_flags;
};

#endif
