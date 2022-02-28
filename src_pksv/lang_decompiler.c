#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "lang_decompiler.h"
#include "lang_parsers.h"
#include "lang_load.h"
#include "language-defs.h"
#include "binarysearch.h"
#include "stdio_ext.h"

#define GBA_OFFSET_MASK 0x1FFFFFF

struct queued_decompilation {
  const struct language_def *language;
  const struct rule *command;  // NULL if no forced command
  // TODO: hardcoded languages (ASM)
};

struct decomp_internal_state {
  FILE *input;
  struct bsearch_root *remaining_blocks;
  struct bsearch_root *seen_addresses;
  const struct language_def *language;
  struct language_cache *language_cache;
  struct parser_cache *parser_cache;
};

static void decomp_visit_address(
    struct decomp_internal_state *state,
    struct queued_decompilation *decompilation_type, uint32_t address);
static struct queued_decompilation *duplicate_queued_decompilation(
    struct queued_decompilation *queued_decompilation);

void decompile_all(FILE *input_file, size_t start_offset,
                   const struct language_def *start_language,
                   struct language_cache *language_cache,
                   struct parser_cache *parser_cache, FILE *output_file) {
  struct bsearch_root unvisited_blocks;
  bsearch_init_root(&unvisited_blocks, bsearch_key_int32cmp, bsearch_key_nocopy,
                    NULL, free);

  struct queued_decompilation *initial_decompilation =
      malloc(sizeof *initial_decompilation);
  *initial_decompilation = (struct queued_decompilation){
      .language = start_language,
      .command = NULL,
  };
  bsearch_upsert(&unvisited_blocks, (void *)start_offset,
                 initial_decompilation);

  // Individual addresses seen by decompiler.
  // Used to determine if an address is already decompiled.
  // Key: address seen by decompiler
  // Value: address of start of decompiled block
  struct bsearch_root decomp_seen_addresses;
  bsearch_init_root(&decomp_seen_addresses, bsearch_key_int32cmp,
                    bsearch_key_nocopy, NULL, NULL);

  // Addresses that are to be decompiled.
  struct bsearch_root decomp_blocks;
  bsearch_init_root(&decomp_blocks, bsearch_key_int32cmp, bsearch_key_nocopy,
                    NULL, free);

  struct decomp_internal_state decomp_state = {
      .input = input_file,
      .remaining_blocks = &unvisited_blocks,
      .seen_addresses = &decomp_seen_addresses,
      .language_cache = language_cache,
      .parser_cache = parser_cache,
      .language = start_language,
  };

  // Visit the initial address and any that result from decompiling it.
  // This serves two purposes:
  //   1. Gets all the blocks that need to be decompiled.
  //   2. Associates addresses with decompiled blocks.
  while (unvisited_blocks.size != 0) {
    uint32_t decompilation_addr =
        (uint32_t)(intptr_t)unvisited_blocks.pairs[0].key;

    if (bsearch_find(&decomp_blocks, (void *)(intptr_t)decompilation_addr) >=
        0) {
      // Already queued for decompilation.
      bsearch_remove(&unvisited_blocks, 0);
      continue;
    }

    struct queued_decompilation *decompilation_type =
        duplicate_queued_decompilation(unvisited_blocks.pairs[0].value);
    bsearch_upsert(&decomp_blocks, (void *)(intptr_t)decompilation_addr,
                   decompilation_type);

    bsearch_remove(&unvisited_blocks, 0);

    decomp_visit_address(&decomp_state, decompilation_type, decompilation_addr);
  }
  bsearch_deinit_root(&unvisited_blocks);

  // TODO

  bsearch_deinit_root(&decomp_seen_addresses);
  bsearch_deinit_root(&decomp_blocks);
}

static struct queued_decompilation *duplicate_queued_decompilation(
    struct queued_decompilation *queued_decompilation) {
  struct queued_decompilation *duplicate = malloc(sizeof *duplicate);
  *duplicate = *queued_decompilation;
  return duplicate;
}

struct lookahead {
  struct bytes_list bytes;
  size_t capacity;
  FILE *file;
};

static void consume_and_refill_lookahead(struct lookahead *restrict lookahead,
                                         size_t consume) {
  assert(consume <= lookahead->bytes.length);

  memmove(lookahead->bytes.bytes, lookahead->bytes.bytes + consume,
          lookahead->bytes.length - consume);
  lookahead->bytes.length -= consume;

  size_t remaining = lookahead->capacity - lookahead->bytes.length;
  size_t read = fread(lookahead->bytes.bytes + lookahead->bytes.length, 1,
                      remaining, lookahead->file);
  lookahead->bytes.length += read;
}

static struct rule *get_rule_from_lookahead(
    struct lookahead *lookahead, const struct bsearch_root *rules_by_bytes) {
  struct rule *matched_rule = NULL;
  ssize_t index = bsearch_find(rules_by_bytes, &lookahead->bytes);
  if (index >= 0) {
    matched_rule = (struct rule *)rules_by_bytes->pairs[index].value;
  } else {
    index = -index - 1;
    // If we match any rule's bytes as a prefix, it must be to the left of the
    // index. i.e. we matched everything, but were > the rule because our length
    // wins out.
    if (index > 0) {
      matched_rule = (struct rule *)rules_by_bytes->pairs[index - 1].value;
      if (matched_rule->bytes.length >= lookahead->bytes.length ||
          memcmp(matched_rule->bytes.bytes, lookahead->bytes.bytes,
                 matched_rule->bytes.length) != 0) {
        matched_rule = NULL;
      }
    }
    // else, we matched nothing, so we're done.
  }

  if (matched_rule != NULL) {
    consume_and_refill_lookahead(lookahead, matched_rule->bytes.length);
  }
  return matched_rule;
}

static const struct language_def *get_prefixed_language(
    struct language_cache *cache, const char *prefix, const char *orig,
    size_t orig_len) {
  char *buffer = malloc(orig_len + strlen(prefix) + 2);
  sprintf("%s_%s", prefix, orig);

  const struct language_def *language = get_language(cache, buffer);

  free(buffer);

  return language;
}

static const struct language_def *decomp_get_next_language(
    struct decomp_internal_state *state, struct language lang) {
  if (lang.is_prefixed) {
    if (lang.name[0] == '\0') {
      // Language recursion, i.e. `@*`
      return state->language;
    }

    size_t lang_name_len = strlen(lang.name);
    const struct language_def *next = get_prefixed_language(
        state->language_cache, state->language->name, lang.name, lang_name_len);
    if (next != NULL) {
      return next;
    }

    char **parents = state->language->parents;
    while (parents != NULL) {
      next = get_prefixed_language(state->language_cache, *parents, lang.name,
                                   lang_name_len);
      if (next != NULL) {
        return next;
      }
      parents++;
    }
  }

  return get_language(state->language_cache, lang.name);
}

static void queue_decompilation_from_lookahead(
    struct bsearch_root *remaining_blocks, struct lookahead *lookahead,
    const struct language_def *next_language, const struct rule *command,
    size_t arg_size) {
  struct queued_decompilation *next_decompilation =
      malloc(sizeof *next_decompilation);
  next_decompilation->language = next_language;
  next_decompilation->command = command;

  size_t addr_size = 4;
  if (arg_size < addr_size) {
    addr_size = arg_size;
  }

  uint32_t next_address =
      arr_get_little_endian(lookahead->bytes.bytes, addr_size);

  // TODO: GSC offset handling
  bsearch_upsert(remaining_blocks,
                 (void *)(intptr_t)(next_address & GBA_OFFSET_MASK),
                 (void *)next_decompilation);
}

struct decomp_visit_state {
  struct lookahead lookahead;
  uint32_t address;
  bool still_going;
};

static void decomp_visit_single(struct decomp_internal_state *state,
                                struct decomp_visit_state *visit_state,
                                const struct language_def *language) {
  struct rule *matched_rule = get_rule_from_lookahead(&visit_state->lookahead,
                                                      language->rules_by_bytes);
  if (matched_rule == NULL) {
    matched_rule = language->special_rules[SPECIAL_RULE_DEFAULT];
    if (matched_rule == NULL) {
      // No matched rule, no default rule, so decompilation can't continue.
      visit_state->still_going = false;
      return;
    }

    if (matched_rule->bytes.length != 0) {
      fprintf(stderr,
              "Warning: default rule for \"%s\" is not empty\nThis will result "
              "in decompilation/recompilation asymmetry.\n",
              language->name);
    }
  }

  if (matched_rule->attributes & RULE_ATTR_END) {
    visit_state->still_going = false;
  }

  consume_and_refill_lookahead(&visit_state->lookahead,
                               matched_rule->bytes.length);
  visit_state->address += matched_rule->bytes.length;

  for (size_t i = 0; i < matched_rule->args.length; i++) {
    struct command_arg *arg = &matched_rule->args.args[i];
    if (visit_state->lookahead.bytes.length < arg->size) {
      visit_state->still_going = false;
      return;  // Reached end of file
    }

    // Handle request for a new language
    switch (arg->as_language.type) {
      case LC_TYPE_NONE:
        break;
      case LC_TYPE_LANG: {
        const struct language_def *next_language =
            decomp_get_next_language(state, arg->as_language.lang);
        if (next_language == NULL) {
          fprintf(stderr, "Warning: language \"%s\" not found\n",
                  arg->as_language.lang.name);
          break;
        }

        queue_decompilation_from_lookahead(state->remaining_blocks,
                                           &visit_state->lookahead,
                                           next_language, NULL, arg->size);
        break;
      }
      case LC_TYPE_COMMAND: {
        ssize_t index = bsearch_find(language->rules_by_command_name,
                                     &arg->as_language.command);
        if (index < 0) {
          fprintf(stderr,
                  "Warning: command \"%s\" not found in language \"%s\"\n",
                  arg->as_language.command, language->name);
          break;
        }
        const struct rule *command =
            language->rules_by_command_name->pairs[index].value;

        queue_decompilation_from_lookahead(state->remaining_blocks,
                                           &visit_state->lookahead,
                                           state->language, command, arg->size);
        break;
      }
      default:
        assert(false);
    }

    consume_and_refill_lookahead(&visit_state->lookahead, arg->size);
    visit_state->address += arg->size;
  }

  // Handle oneshot language
  if (matched_rule->oneshot_lang.name[0] != '\0') {
    const struct language_def *next_language =
        decomp_get_next_language(state, matched_rule->oneshot_lang);
    if (next_language == NULL) {
      fprintf(stderr, "Warning: language \"%s\" not found\n",
              matched_rule->oneshot_lang.name);
      return;
    }

    decomp_visit_single(state, visit_state, next_language);
  }
}

static void decomp_visit_address(
    struct decomp_internal_state *restrict state,
    struct queued_decompilation *restrict decompilation_type,
    uint32_t initial_address) {
  const struct language_def *language = decompilation_type->language;
  const size_t LOOKAHEAD_SIZE =
      16;  // TODO: record maximum bytes_list length per language
  struct decomp_visit_state visit_state = {
      .lookahead =
          {
              .bytes =
                  {
                      .bytes = malloc(LOOKAHEAD_SIZE),
                      .length = 0,
                  },
              .capacity = LOOKAHEAD_SIZE,
              .file = state->input,
          },
      .address = initial_address,
      .still_going = true,
  };

  // TODO: GSC offset handling
  fseek(state->input, initial_address, SEEK_SET);
  consume_and_refill_lookahead(&visit_state.lookahead, 0);

  while (visit_state.still_going) {
    // Note: address is recorded here rather than in the call to
    // decomp_visit_single, because the recursive calls as a result of
    // oneshot_lang do not have their own line in the decompiled output.
    ssize_t index = bsearch_find(state->seen_addresses,
                                 (void *)(intptr_t)visit_state.address);
    if (index >= 0) {
      if ((uint32_t)(intptr_t)state->seen_addresses->pairs[index].value >
          initial_address) {
        // We have visited this address before, but this time we have a fuller
        // view of the block
        state->seen_addresses->pairs[index].value =
            (void *)(intptr_t)initial_address;
      }
    } else {
      // Newly visited address
      bsearch_unsafe_insert(state->seen_addresses, index,
                            (void *)(intptr_t)visit_state.address,
                            (void *)(intptr_t)initial_address);
    }
    decomp_visit_single(state, &visit_state, language);
  }

  free(visit_state.lookahead.bytes.bytes);
}
