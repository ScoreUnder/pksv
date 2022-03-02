#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "lang_decompiler.h"
#include "lang_default_parsers.h"
#include "lang_parsers.h"
#include "lang_load.h"
#include "language-defs.h"
#include "binarysearch_u32.h"
#include "binarysearch.h"
#include "stdio_ext.h"

#define GBA_OFFSET_MASK 0x1FFFFFF

struct language_def error_lang = {
    .rules_by_bytes = &(struct bsearch_root){0},
    .rules_by_command_name = &(struct bsearch_root){0},
    .special_rules =
        {
            [SPECIAL_RULE_DEFAULT] =
                &(struct rule){
                    .bytes = {0, 0},
                    .args = {0, 0},
                    .oneshot_lang = {"", 0},
                    .command_name =
                        "' Missing language definition for this block",
                    .attributes = RULE_ATTR_END,
                },
        },
    .lookup_bytes = &(struct byte_lookup_tables){0},
    .parents = (char *[]){NULL},
    .name = "",
    .meta_flags = METAFLAG_LANGTYPE_TEXT,
};

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
    struct queued_decompilation *decompilation_type, uint32_t initial_address,
    bool decompile);
static struct queued_decompilation *duplicate_queued_decompilation(
    struct queued_decompilation *queued_decompilation);

void decompile_all(FILE *input_file, uint32_t start_offset,
                   const struct language_def *start_language,
                   struct language_cache *language_cache,
                   struct parser_cache *parser_cache, FILE *output_file) {
  struct bsearch_root unvisited_blocks;
  bsearch_init_root(&unvisited_blocks, bsearch_key_uint32cmp,
                    bsearch_key_nocopy, NULL, free);

  struct queued_decompilation *initial_decompilation =
      malloc(sizeof *initial_decompilation);
  *initial_decompilation = (struct queued_decompilation){
      .language = start_language,
      .command = NULL,
  };
  bsearch_upsert(&unvisited_blocks, CAST_u32_pvoid(start_offset),
                 initial_decompilation);

  // Individual addresses seen by decompiler.
  // Used to determine if an address is already decompiled.
  // Key: address seen by decompiler
  // Value: address of start of decompiled block
  struct bsearch_root decomp_seen_addresses;
  bsearch_init_root(&decomp_seen_addresses, bsearch_key_uint32cmp,
                    bsearch_key_nocopy, NULL, NULL);

  // Addresses that are to be decompiled.
  struct bsearch_root decomp_blocks;
  bsearch_init_root(&decomp_blocks, bsearch_key_uint32cmp, bsearch_key_nocopy,
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
    uint32_t decompilation_addr = bsearch_key_u32(&unvisited_blocks, 0);

    if (bsearch_find_u32(&decomp_blocks, decompilation_addr) >= 0) {
      // Already queued for decompilation.
      bsearch_remove(&unvisited_blocks, 0);
      continue;
    }

    struct queued_decompilation *decompilation_type =
        duplicate_queued_decompilation(unvisited_blocks.pairs[0].value);
    bsearch_upsert(&decomp_blocks, CAST_u32_pvoid(decompilation_addr),
                   decompilation_type);

    bsearch_remove(&unvisited_blocks, 0);

    decomp_visit_address(&decomp_state, decompilation_type, decompilation_addr,
                         false);
  }
  bsearch_deinit_root(&unvisited_blocks);
  decomp_state.remaining_blocks = NULL;

  struct bsearch_root labels;
  bsearch_init_root(&labels, bsearch_key_uint32cmp, bsearch_key_nocopy, NULL,
                    free);

  // Assign labels to decompiled blocks, remove any which overlap.
  size_t label_num = 0;
  for (size_t i = 0; i < decomp_blocks.size; i++) {
    uint32_t decomp_addr = bsearch_key_u32(&decomp_blocks, i);

    // Create a label for the decompiled block.
    char *label = malloc(32);
    snprintf(label, 32, "label_%zu", label_num++);
    bsearch_upsert(&labels, CAST_u32_pvoid(decomp_addr), label);

    // Find the start of the decompiled block.
    ptrdiff_t decomp_block_index =
        bsearch_find_u32(&decomp_seen_addresses, decomp_addr);
    assert(decomp_block_index >= 0);

    // Remove this block from decompilation queue if it overlaps with another
    // of the same decompilation type.
    uint32_t decomp_block_start =
        bsearch_val_u32(&decomp_seen_addresses, decomp_block_index);
    if (decomp_block_start != decomp_addr) {
      struct queued_decompilation *decompilation_type =
          decomp_blocks.pairs[i].value;
      ptrdiff_t other_decomp_info_index =
          bsearch_find_u32(&decomp_blocks, decomp_block_start);
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

  ptrdiff_t start_label_index = bsearch_find_u32(&labels, start_offset);
  assert(start_label_index >= 0);
  fprintf(output_file, "' Script starts at :%s\n",
          (char *)labels.pairs[start_label_index].value);

  decomp_state.labels = &labels;
  // Decompile the blocks.
  for (size_t i = 0; i < decomp_blocks.size; i++) {
    putc('\n', decomp_state.output);

    uint32_t decompilation_addr = bsearch_key_u32(&decomp_blocks, i);
    struct queued_decompilation *decompilation_type =
        decomp_blocks.pairs[i].value;

    decomp_visit_address(&decomp_state, decompilation_type, decompilation_addr,
                         true);
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
  ptrdiff_t index = bsearch_find(rules_by_bytes, &lookahead->bytes);
  if (index >= 0) {
    // Perfect match (somehow??)
    return rules_by_bytes->pairs[index].value;
  } else {
    index = -index - 1;
    // If we match any rule's bytes as a prefix, it must be to the left of the
    // index. i.e. we matched everything, but were > the rule because our length
    // wins out.
    while (--index >= 0) {
      matched_rule = (struct rule *)rules_by_bytes->pairs[index].value;
      if (matched_rule->bytes.length < lookahead->bytes.length &&
          memcmp(matched_rule->bytes.bytes, lookahead->bytes.bytes,
                 matched_rule->bytes.length) == 0) {
        return matched_rule;
      }
      if (matched_rule->bytes.length >= 1 &&
          matched_rule->bytes.bytes[0] < lookahead->bytes.bytes[0]) {
        // Scrolled too far back; no match
        break;
      }
    }
    // else, we matched nothing, so we're done.
    return NULL;
  }
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
                 CAST_u32_pvoid(next_address & GBA_OFFSET_MASK),
                 next_decompilation);
}

struct decomp_visit_state {
  struct lookahead lookahead;
  size_t line_length;
  const struct language_def *base_language;
  const struct rule *base_command;
  const struct language_def *curr_language;
  const struct rule *curr_command;
  uint32_t address;
  uint32_t initial_address;
  bool still_going;
  // false = just visiting to collect block info
  // true = decompiling
  bool decompile;
  // reported by decomp_visit_single: whether to continue on the same line
  bool continue_line;
  // informative for decomp_visit_single: whether this started on a new line
  bool is_new_line;
};

static bool lang_can_split_lines(const struct decomp_visit_state *state) {
  const struct language_def *language = state->curr_language;
  switch (language->meta_flags & METAFLAG_MASK_LANGTYPE) {
    case METAFLAG_LANGTYPE_TEXT:
    case METAFLAG_LANGTYPE_LINE: {
      // Can't wrap a language if it would force another command
      // and its arguments to be written when they are of non-zero length.
      const struct rule *command = state->base_command;
      if (command != NULL &&
          (command->bytes.length != 0 ||
           command->args.length != 0)) {
        return false;
      }
      // If we want to wrap a text or line-type language,
      // we need to have a way to suppress line terminators.
      return language->special_rules[SPECIAL_RULE_TERMINATOR] == NULL ||
             language->special_rules[SPECIAL_RULE_NO_TERMINATE] != NULL;
    }
    default:
      return false;
  }
}

static bool check_and_report_eof(struct decomp_internal_state *state,
                                 struct decomp_visit_state *visit_state,
                                 size_t needed_bytes) {
  if (visit_state->lookahead.bytes.length < needed_bytes) {
    if (visit_state->decompile) {
      fputs("\n' EOF\n", state->output);
    }
    visit_state->still_going = false;
    return true;
  }
  return false;
}

static void decomp_visit_single(struct decomp_internal_state *state,
                                struct decomp_visit_state *visit_state) {
  if (check_and_report_eof(state, visit_state, 1)) return;
  const struct language_def *language = visit_state->curr_language;

  uint32_t command_start_address = visit_state->address;
  if (!visit_state->decompile &&
      (visit_state->is_new_line || lang_can_split_lines(visit_state))) {
    // Record this address as a visited "line"
    ptrdiff_t index =
        bsearch_find_u32(state->seen_addresses, visit_state->address);
    if (index >= 0) {
      if (bsearch_val_u32(state->seen_addresses, index) >
          visit_state->initial_address) {
        // We have visited this address before, but this time we have a fuller
        // view of the block
        bsearch_setval_u32(state->seen_addresses, index,
                           visit_state->initial_address);
      }
    } else {
      // Newly visited address
      bsearch_unsafe_insert(state->seen_addresses, index,
                            CAST_u32_pvoid(visit_state->address),
                            CAST_u32_pvoid(visit_state->initial_address));
    }
  }

  unsigned int language_type = language->meta_flags & METAFLAG_MASK_LANGTYPE;

  const struct rule *matched_rule = visit_state->curr_command;
  if (matched_rule == NULL) {
    matched_rule = get_rule_from_lookahead(&visit_state->lookahead,
                                           language->rules_by_bytes);
  }
  uint8_t translated_byte;
  if (matched_rule == NULL && language_type == METAFLAG_LANGTYPE_TEXT &&
      visit_state->lookahead.bytes.length >= 1 &&
      (translated_byte = language->lookup_bytes
                             ->decomp[visit_state->lookahead.bytes.bytes[0]]) !=
          '\0') {
    // Special case: if we're decompiling a text language, and we have a
    // "simple" (byte table lookup) rule, we can just use the byte as-is.
    // This lets us skip a lot of logic and saves on memory too.
    if (visit_state->decompile) {
      putc(translated_byte, state->output);
      visit_state->line_length++;
    }
    consume_and_refill_lookahead(&visit_state->lookahead, 1);
  } else {
    if (matched_rule == NULL) {
      matched_rule = language->special_rules[SPECIAL_RULE_DEFAULT];
      if (matched_rule == NULL) {
        // No matched rule, no default rule, so decompilation can't continue.
        visit_state->still_going = false;
        return;
      }

      if (matched_rule->bytes.length != 0) {
        fprintf(
            stderr,
            "Warning: default rule for \"%s\" is not empty\nThis will result "
            "in decompilation/recompilation asymmetry.\n",
            language->name);
      }
    }

    size_t cmd_first_len;
    if (language_type == METAFLAG_LANGTYPE_TEXT) {
      // Text languages should not have split commands
      cmd_first_len = strlen(matched_rule->command_name);
    } else {
      cmd_first_len = strcspn(matched_rule->command_name, " ");
    }

    if (visit_state->decompile) {
      if (matched_rule->attributes & RULE_ATTR_CMP_FLAG)
        state->info.is_checkflag = true;
      else if (matched_rule->attributes & RULE_ATTR_CMP_INT)
        state->info.is_checkflag = false;

      fwrite(matched_rule->command_name, 1, cmd_first_len, state->output);
      visit_state->line_length += cmd_first_len;
    }

    if (matched_rule->attributes & RULE_ATTR_END) {
      visit_state->still_going = false;
    }

    consume_and_refill_lookahead(&visit_state->lookahead,
                                 matched_rule->bytes.length);
    visit_state->address += matched_rule->bytes.length;

    for (size_t i = 0; i < matched_rule->args.length; i++) {
      struct command_arg *arg = &matched_rule->args.args[i];
      if (check_and_report_eof(state, visit_state, arg->size)) return;

      if (visit_state->decompile) {
        uint32_t value = arr_get_little_endian(
            visit_state->lookahead.bytes.bytes, arg->size);

        if (language_type == METAFLAG_LANGTYPE_TEXT) {
          // No parsers supported for raw bytes in text languages
          // ...yet?
          fprintf(state->output, "%0*x", (int)arg->size * 2, value);
        } else {
          putc(' ', state->output);
          visit_state->line_length++;

          struct parse_result result = format_for_decomp(
              state->parser_cache, language, matched_rule->args.args[i].parsers,
              value, &state->info);

          switch (result.type) {
            default:
            case PARSE_RESULT_FAIL:
              assert(
                  false);  // should always be able to parse at least fallbacks
              break;
            case PARSE_RESULT_VALUE:
              assert(false);  // this shouldn't be returned by a formatter
              break;
            case PARSE_RESULT_TOKEN:
              fputs(result.token, state->output);
              visit_state->line_length += strlen(result.token);
              free(result.token);
              break;
            case PARSE_RESULT_LABEL: {
              // TODO: GSC offset handling
              value = result.value & GBA_OFFSET_MASK;
              ptrdiff_t label_index = bsearch_find_u32(state->labels, value);
              if (label_index >= 0) {
                const char *label = state->labels->pairs[label_index].value;
                putc(':', state->output);
                fputs(label, state->output);
                visit_state->line_length += strlen(label) + 1;
              } else {
                result = builtin_parser_hex.builtin.format(value, &state->info);
                assert(result.type ==
                       PARSE_RESULT_TOKEN);  // Default hex fallback parser
                                             // can't fail on us
                fputs(result.token, state->output);
                visit_state->line_length += strlen(result.token);
              }
              break;
            }
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
              next_language = &error_lang;
            }

            queue_decompilation_from_lookahead(state->remaining_blocks,
                                               &visit_state->lookahead,
                                               next_language, NULL, arg->size);
            break;
          }
          case LC_TYPE_COMMAND: {
            ptrdiff_t index = bsearch_find(language->rules_by_command_name,
                                           arg->as_language.command);
            if (index < 0) {
              fprintf(stderr,
                      "Warning: command \"%s\" not found in language \"%s\"\n",
                      arg->as_language.command, language->name);
              break;
            }
            const struct rule *command =
                language->rules_by_command_name->pairs[index].value;

            queue_decompilation_from_lookahead(
                state->remaining_blocks, &visit_state->lookahead,
                state->language, command, arg->size);
            break;
          }
          default:
            assert(false);
        }
      }

      consume_and_refill_lookahead(&visit_state->lookahead, arg->size);
      visit_state->address += arg->size;

      // Special case for commands with spaces in (insert second half after
      // first argument)
      if (i == 0 && visit_state->decompile &&
          matched_rule->command_name[cmd_first_len] == ' ') {
        fputs(&matched_rule->command_name[cmd_first_len], state->output);
        visit_state->line_length +=
            strlen(&matched_rule->command_name[cmd_first_len]);
      }
    }

    // Handle oneshot language
    if (matched_rule->oneshot_lang.name[0] != '\0') {
      const struct language_def *next_language =
          decomp_get_next_language(state, matched_rule->oneshot_lang);
      if (next_language == NULL) {
        if (visit_state->decompile) {
          fprintf(state->output,
                  "  ' Can't find language \"%s\" for the rest of this command",
                  matched_rule->oneshot_lang.name);
          // Newline gets output after return
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

      if (visit_state->decompile && language_type != METAFLAG_LANGTYPE_TEXT) {
        putc(' ', state->output);
        visit_state->line_length++;
      }

      visit_state->curr_language = next_language;
      visit_state->curr_command = NULL;
      visit_state->continue_line = true;
      return;
    }
  }

  if (visit_state->decompile) {
    // Only need concatenated command lines if decompiling
    if (visit_state->still_going && (language_type == METAFLAG_LANGTYPE_LINE ||
                                     language_type == METAFLAG_LANGTYPE_TEXT)) {
      // If we're in a line or text language, we need to produce more on the
      // same line
      // ... unless... we can line-wrap?
      uint8_t attributes = matched_rule == NULL ? 0 : matched_rule->attributes;
      bool want_linewrap = visit_state->line_length >= 80 ||
                           (attributes & RULE_ATTR_PREFER_BREAK) != 0;
      if (want_linewrap && lang_can_split_lines(visit_state) &&
          (language_type != METAFLAG_LANGTYPE_TEXT ||
           (attributes & RULE_ATTR_BREAK) != 0)) {
        // output no-terminate token if needed
        const struct rule *noterm =
            language->special_rules[SPECIAL_RULE_NO_TERMINATE];
        if (noterm != NULL) {
          fputs(noterm->command_name, state->output);
        }
        return;
      } else {
        // otherwise, we can't line-wrap, so we just continue
        if (language_type != METAFLAG_LANGTYPE_TEXT) {
          putc(' ', state->output);
          visit_state->line_length++;
        }
        visit_state->continue_line = true;
        return;
      }
    }
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
      .line_length = 0,
      .base_language = language,
      .base_command = decompilation_type->command,
      .curr_language = language,
      .curr_command = decompilation_type->command,
      .continue_line = false,
      .is_new_line = true,
  };

  // TODO: GSC offset handling
  fseek(state->input, initial_address, SEEK_SET);
  consume_and_refill_lookahead(&visit_state.lookahead, 0);

  if (decompile) {
    fprintf(state->output, "#org 0x%08x %s\n", initial_address, language->name);
  }

  uint32_t next_label_address = UINT32_MAX;
  ptrdiff_t next_label_index = PTRDIFF_MAX;

  if (decompile) {
    next_label_index = bsearch_find_u32(state->labels, initial_address);
    if (next_label_index < 0) {
      next_label_index = -next_label_index - 1;
    }
    if ((size_t)next_label_index < state->labels->size) {
      next_label_address = bsearch_key_u32(state->labels, next_label_index);
    }
  }

  while (visit_state.still_going) {
    if (decompile && visit_state.is_new_line &&
        visit_state.address == next_label_address &&
        (size_t)next_label_index < state->labels->size) {
      fprintf(state->output, ":%s\n",
              (char *)state->labels->pairs[next_label_index].value);
      next_label_index++;
      if ((size_t)next_label_index < state->labels->size) {
        next_label_address = bsearch_key_u32(state->labels, next_label_index);
      }
    }

    visit_state.continue_line = false;
    decomp_visit_single(state, &visit_state);

    if (visit_state.decompile && !visit_state.continue_line) {
      fputs("\n", state->output);
      visit_state.line_length = 0;
      if (!visit_state.is_new_line) {
        // If we started this loop trying a sub-language, and now we have
        // a new line, we need to reset the language to the base language
        visit_state.curr_language = visit_state.base_language;
        visit_state.curr_command = visit_state.base_command;
      }
    }
    visit_state.is_new_line = !visit_state.continue_line;
  }

  free(visit_state.lookahead.bytes.bytes);
}
