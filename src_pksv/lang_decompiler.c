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
  FILE *output;
  struct bsearch_root *remaining_blocks;
  struct bsearch_root *seen_addresses;
  struct bsearch_root *labels;
  const struct language_def *language;
  struct language_cache *language_cache;
  struct parser_cache *parser_cache;
  struct decompiler_informative_state info;
};

static void decomp_visit_address(
    struct decomp_internal_state *state,
    struct queued_decompilation *decompilation_type, uint32_t initial_address, bool decompile);
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
      .output = output_file,
      .remaining_blocks = &unvisited_blocks,
      .seen_addresses = &decomp_seen_addresses,
      .labels = NULL,
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

    decomp_visit_address(&decomp_state, decompilation_type, decompilation_addr, false);
  }
  bsearch_deinit_root(&unvisited_blocks);
  decomp_state.remaining_blocks = NULL;

  struct bsearch_root labels;
  bsearch_init_root(&labels, bsearch_key_int32cmp, bsearch_key_nocopy, NULL,
                    free);

  // Assign labels to decompiled blocks, remove any which overlap.
  size_t label_num = 0;
  for (size_t i = 0; i < decomp_blocks.size; i++) {
    uint32_t decomp_addr = (uint32_t)(intptr_t)decomp_blocks.pairs[i].key;

    // Create a label for the decompiled block.
    char *label = malloc(32);
    snprintf(label, 32, "label_%zu", label_num++);
    bsearch_upsert(&labels, (void *)(intptr_t)decomp_addr, label);

    // Find the start of the decompiled block.
    ssize_t decomp_block_index =
        bsearch_find(&decomp_seen_addresses, (void *)(intptr_t)decomp_addr);
    assert(decomp_block_index >= 0);

    // Remove this block from decompilation queue if it overlaps with another
    // of the same decompilation type.
    uint32_t decomp_block_start =
        (uint32_t)(intptr_t)decomp_seen_addresses.pairs[decomp_block_index]
            .value;
    if (decomp_block_start != decomp_addr) {
      struct queued_decompilation *decompilation_type =
          decomp_blocks.pairs[i].value;
      ssize_t other_decomp_info_index =
          bsearch_find(&decomp_blocks, (void *)(intptr_t)decomp_block_start);
      assert(other_decomp_info_index >= 0);
      struct queued_decompilation *other_decomp_info =
          decomp_blocks.pairs[other_decomp_info_index].value;
      if (decompilation_type->language == other_decomp_info->language) {
        bsearch_remove(&decomp_blocks, i);
        i--;
      }
    }
  }
  bsearch_deinit_root(&decomp_seen_addresses);
  decomp_state.seen_addresses = NULL;

  ssize_t start_label_index = bsearch_find(&labels, (void *)(intptr_t)start_offset);
  assert(start_label_index >= 0);
  fprintf(output_file, "' Script starts at :%s\n", labels.pairs[start_label_index].value);

  decomp_state.labels = &labels;
  // Decompile the blocks.
  for (size_t i = 0; i < decomp_blocks.size; i++) {
    putc('\n', decomp_state.output);

    uint32_t decompilation_addr = (uint32_t)(intptr_t)decomp_blocks.pairs[i].key;
    struct queued_decompilation *decompilation_type = decomp_blocks.pairs[i].value;

    decomp_visit_address(&decomp_state, decompilation_type, decompilation_addr, true);
  }

  bsearch_deinit_root(&decomp_blocks);
  bsearch_deinit_root(&labels);
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

  return matched_rule;
}

static const struct language_def *get_prefixed_language(
    struct language_cache *cache, const char *prefix, const char *orig,
    size_t orig_len) {
  char *buffer = malloc(orig_len + strlen(prefix) + 2);
  sprintf(buffer, "%s_%s", prefix, orig);

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
    while (*parents != NULL) {
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
  uint32_t initial_address;
  bool still_going;
  bool decompile;  // false = just visiting to collect block info
};

static bool lang_can_split_lines(const struct language_def *language) {
  switch (language->meta_flags & METAFLAG_MASK_LANGTYPE) {
    case METAFLAG_LANGTYPE_TEXT:
    case METAFLAG_LANGTYPE_LINE:
      return language->special_rules[SPECIAL_RULE_NO_TERMINATE] != NULL;
    default:
      return false;
  }
}

static void decomp_visit_single(struct decomp_internal_state *state,
                                struct decomp_visit_state *visit_state,
                                const struct language_def *language,
                                const struct rule *force_command, bool first) {
  uint32_t command_start_address = visit_state->address;
  if (!visit_state->decompile && (first || lang_can_split_lines(language))) {
    // Record this address as a visited "line"
    ssize_t index = bsearch_find(state->seen_addresses,
                                 (void *)(intptr_t)visit_state->address);
    if (index >= 0) {
      if ((uint32_t)(intptr_t)state->seen_addresses->pairs[index].value >
          visit_state->initial_address) {
        // We have visited this address before, but this time we have a fuller
        // view of the block
        state->seen_addresses->pairs[index].value =
            (void *)(intptr_t)visit_state->initial_address;
      }
    } else {
      // Newly visited address
      bsearch_unsafe_insert(state->seen_addresses, index,
                            (void *)(intptr_t)visit_state->address,
                            (void *)(intptr_t)visit_state->initial_address);
    }
  }

  const struct rule *matched_rule = force_command;
  if (matched_rule == NULL) {
    matched_rule = get_rule_from_lookahead(&visit_state->lookahead,
                                           language->rules_by_bytes);
  }
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

  if (visit_state->decompile) {
    fputs(matched_rule->command_name, state->output);
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
      if (visit_state->decompile) {
        fputs("  ' EOF\n", state->output);
      }
      return;  // Reached end of file
    }

    if (visit_state->decompile) {
      putc(' ', state->output);
      uint32_t value = arr_get_little_endian(visit_state->lookahead.bytes.bytes,
                                             arg->size);
      struct parse_result result = format_for_decomp(
        state->parser_cache, language, matched_rule->args.args[i].parsers, value, &state->info);

      switch (result.type) {
        default:
        case PARSE_RESULT_FAIL:
          assert(false); // should always be able to parse at least fallbacks
          break;
        case PARSE_RESULT_VALUE:
          assert(false); // this shouldn't be returned by a formatter
          break;
        case PARSE_RESULT_TOKEN:
          fputs(result.token, state->output);
          free(result.token);
          break;
        case PARSE_RESULT_LABEL: {
          value = result.value;
          ssize_t label_index = bsearch_find(state->labels,
                                             (void *)(intptr_t)value);
          assert(label_index >= 0);  // all labels should be found
          putc(':', state->output);
          fputs(state->labels->pairs[label_index].value, state->output);
          break;
        }
      }
    } else {
      // If collecting block info, we need to follow addresses
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
                                      arg->as_language.command);
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
    }

    consume_and_refill_lookahead(&visit_state->lookahead, arg->size);
    visit_state->address += arg->size;
  }

  // Handle oneshot language
  if (matched_rule->oneshot_lang.name[0] != '\0') {
    const struct language_def *next_language =
        decomp_get_next_language(state, matched_rule->oneshot_lang);
    if (next_language == NULL) {
      if (visit_state->decompile) {
        fprintf(state->output, "  ' Can't find language \"%s\" for the rest of this command\n", matched_rule->oneshot_lang.name);
      } else {
        fprintf(stderr, "Warning: language \"%s\" not found\n",
                matched_rule->oneshot_lang.name);
      }
      if (visit_state->address == command_start_address) {
        // Stop, or end up in an infinite loop.
        visit_state->still_going = false;
      }
      return;
    }

    if (visit_state->decompile) {
      putc(' ', state->output);
    }

    decomp_visit_single(state, visit_state, next_language, NULL, false);
  }

  if (visit_state->decompile) {
    fputs("\n", state->output);
  }
}

static void decomp_visit_address(
    struct decomp_internal_state *restrict state,
    struct queued_decompilation *restrict decompilation_type,
    uint32_t initial_address, bool decompile) {
  const struct language_def *language = decompilation_type->language;
  const size_t LOOKAHEAD_SIZE = 16;  // TODO: record maximum bytes_list length
                                     // per language, max with arg sizes
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
      .initial_address = initial_address,
      .still_going = true,
      .decompile = decompile,
  };

  // TODO: GSC offset handling
  fseek(state->input, initial_address, SEEK_SET);
  consume_and_refill_lookahead(&visit_state.lookahead, 0);

  if (decompile) {
    fprintf(state->output, "#org 0x%08x\n", initial_address);
  }

  uint32_t next_label_address = UINT32_MAX;
  ssize_t next_label_index;

  if (decompile) {
    next_label_index = bsearch_find(state->labels, (void *)(intptr_t)initial_address);
    if (next_label_index < 0) {
      next_label_index = -next_label_index - 1;
    }
    if ((size_t)next_label_index < state->labels->size) {
      next_label_address = (uint32_t)(intptr_t) state->labels->pairs[next_label_index].key;
    }
  }

  while (visit_state.still_going) {
    if (decompile) {
      if (visit_state.address == next_label_address && (size_t)next_label_index < state->labels->size) {
        fprintf(state->output, ":%s\n", state->labels->pairs[next_label_index].value);
        next_label_index++;
        if ((size_t)next_label_index < state->labels->size) {
          next_label_address = (uint32_t)(intptr_t) state->labels->pairs[next_label_index].key;
        }
      }
    }
    decomp_visit_single(state, &visit_state, language,
                        decompilation_type->command, true);
  }

  free(visit_state.lookahead.bytes.bytes);
}
