#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

#include "lang_decompiler.h"
#include "lang_decompiler_types.h"
#include "lang_default_parsers.h"
#include "lang_parsers.h"
#include "lang_load.h"
#include "language-defs.h"
#include "binarysearch_u32.h"
#include "binarysearch.h"
#include "uint32_interval.h"
#include "stdio_ext.h"
#include "romutil.h"

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
  uint32_t end_offset;
};

struct decomp_internal_state {
  FILE *input;
  FILE *output;
  struct bsearch_root *remaining_blocks;
  struct bsearch_root *explored_blocks;
  struct bsearch_root *label_blocks;
  struct bsearch_root *labels;
  struct language_cache *language_cache;
  struct parser_cache *parser_cache;
  struct decompiler_informative_state info;
  bool is_verbose;
};

enum decompilation_stage {
  STAGE_FINDING_BLOCKS,
  STAGE_FINDING_LABELS,
  STAGE_DECOMPILING,
};

static uint32_t decomp_visit_address(
    struct decomp_internal_state *state,
    struct queued_decompilation *decompilation_type, uint32_t initial_address,
    enum decompilation_stage decomp_stage);
static struct queued_decompilation *duplicate_queued_decompilation(
    struct queued_decompilation *queued_decompilation);
static void merge_block_intervals(struct bsearch_root *decomp_blocks,
                                  struct bsearch_root *block_intervals);

void decompile_all(FILE *input_file, uint32_t start_offset,
                   const struct language_def *start_language,
                   struct language_cache *language_cache,
                   struct parser_cache *parser_cache, FILE *output_file,
                   bool is_verbose, bool aggressive_block_merging) {
  struct bsearch_root unvisited_blocks;
  bsearch_init_root(&unvisited_blocks, bsearch_key_uint32cmp,
                    bsearch_key_nocopy, NULL, free);

  // Map of block start addresses to larger containing block start addresses
  struct bsearch_root label_blocks;
  bsearch_init_root(&label_blocks, bsearch_key_uint32cmp, bsearch_key_nocopy,
                    NULL, NULL);

  struct queued_decompilation *initial_decompilation =
      malloc(sizeof *initial_decompilation);
  *initial_decompilation = (struct queued_decompilation){
      .language = start_language,
      .command = NULL,
  };
  bsearch_upsert(&unvisited_blocks, CAST_u32_pvoid(start_offset),
                 initial_decompilation);

  // Addresses that are to be decompiled.
  struct bsearch_root decomp_blocks;
  bsearch_init_root(&decomp_blocks, bsearch_key_uint32cmp, bsearch_key_nocopy,
                    NULL, free);

  struct decomp_internal_state decomp_state = {
      .input = input_file,
      .output = output_file,
      .remaining_blocks = &unvisited_blocks,
      .explored_blocks = NULL,
      .label_blocks = NULL,
      .labels = NULL,
      .language_cache = language_cache,
      .parser_cache = parser_cache,
      .is_verbose = is_verbose,
  };

  // Visit the initial address and any that result from decompiling it.
  // This serves two purposes:
  //   1. Gets all the blocks that need to be decompiled.
  //   2. Associates addresses with decompiled blocks.
  while (unvisited_blocks.size != 0) {
    // Taking last is faster on removal
    size_t index = unvisited_blocks.size - 1;
    uint32_t decompilation_addr = bsearch_key_u32(&unvisited_blocks, index);

    if (bsearch_find_u32(&decomp_blocks, decompilation_addr) >= 0) {
      // Already queued for decompilation.
      bsearch_remove(&unvisited_blocks, index);
      continue;
    }

    struct queued_decompilation *decompilation_type =
        duplicate_queued_decompilation(unvisited_blocks.pairs[index].value);
    bsearch_upsert(&decomp_blocks, CAST_u32_pvoid(decompilation_addr),
                   decompilation_type);

    bsearch_remove(&unvisited_blocks, index);

    uint32_t end_address =
        decomp_visit_address(&decomp_state, decompilation_type,
                             decompilation_addr, STAGE_FINDING_BLOCKS);
    decompilation_type->end_offset = end_address;
  }
  bsearch_deinit_root(&unvisited_blocks);
  decomp_state.remaining_blocks = NULL;

  // Find blocks that labels belong to
  decomp_state.explored_blocks = &decomp_blocks;
  decomp_state.label_blocks = &label_blocks;
  for (size_t i = 0; i < decomp_blocks.size; i++) {
    uint32_t decompilation_addr = bsearch_key_u32(&decomp_blocks, i);
    struct queued_decompilation *decompilation_type =
        decomp_blocks.pairs[i].value;

    decomp_visit_address(&decomp_state, decompilation_type, decompilation_addr,
                         STAGE_FINDING_LABELS);
  }
  decomp_state.explored_blocks = NULL;
  decomp_state.label_blocks = NULL;

  struct bsearch_root labels;
  bsearch_init_root(&labels, bsearch_key_uint32cmp, bsearch_key_nocopy, NULL,
                    free);

  // Name labels and ensure overlapped labels are not queued for their own
  // decompilation
  size_t label_num = 0;
  for (size_t i = 0; i < label_blocks.size; i++) {
    uint32_t label_addr = bsearch_key_u32(&label_blocks, i);
    uint32_t block_addr = bsearch_val_u32(&label_blocks, i);

    if (label_addr != block_addr) {
      // Label is overlapped by another block, so don't decompile it.
      ptrdiff_t index = bsearch_find_u32(&decomp_blocks, label_addr);
      assert(index >= 0);
      bsearch_remove(&decomp_blocks, (size_t)index);
    }

    // Name the label
    char *label = malloc(32);
    if (decomp_state.is_verbose) {
      snprintf(label, 32, "label_%" PRIx32, label_addr);
    } else {
      snprintf(label, 32, "label_%zu", label_num++);
    }
    bsearch_upsert(&labels, CAST_u32_pvoid(label_addr), label);
  }

  bsearch_deinit_root(&label_blocks);

  // Merge block intervals for #erase
  struct bsearch_root erase_intervals;
  uint32_interval_init_bsearch_root(&erase_intervals);
  merge_block_intervals(&decomp_blocks, &erase_intervals);

  ptrdiff_t start_label_index = bsearch_find_u32(&labels, start_offset);
  assert(start_label_index >= 0);
  fprintf(output_file, "' Script starts at :%s\n",
          (char *)labels.pairs[start_label_index].value);

  // Output #erase commands
  fprintf(output_file,
          "\n' Erase previous script on recompile (to reclaim space):\n");
  for (size_t i = 0; i < erase_intervals.size; i++) {
    uint32_t start = bsearch_key_u32(&erase_intervals, i);
    uint32_t end = bsearch_val_u32(&erase_intervals, i);
    fprintf(output_file, "#erase 0x%" PRIx32 " 0x%" PRIx32 "\n", start, end);
  }

  bsearch_deinit_root(&erase_intervals);

  decomp_state.labels = &labels;
  uint32_t last_end_offset = 0;
  const struct language_def *last_language = NULL;
  // Decompile the blocks.
  for (size_t i = 0; i < decomp_blocks.size; i++) {
    putc('\n', decomp_state.output);

    uint32_t decompilation_addr = bsearch_key_u32(&decomp_blocks, i);
    struct queued_decompilation *decompilation_type =
        decomp_blocks.pairs[i].value;

    if (!aggressive_block_merging || last_end_offset != decompilation_addr ||
        last_end_offset == 0) {
      fprintf(output_file, "#org 0x%08x %s\n", decompilation_addr,
              decompilation_type->language->name);
    } else if (aggressive_block_merging &&
               last_language != decompilation_type->language) {
      fprintf(output_file, "#language %s\n",
              decompilation_type->language->name);
    }
    last_end_offset =
        decomp_visit_address(&decomp_state, decompilation_type,
                             decompilation_addr, STAGE_DECOMPILING);
    last_language = decompilation_type->language;
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

void merge_block_intervals(struct bsearch_root *decomp_blocks,
                           struct bsearch_root *block_intervals) {
  for (size_t i = 0; i < decomp_blocks->size; i++) {
    uint32_t decompilation_addr = bsearch_key_u32(decomp_blocks, i);
    struct queued_decompilation *decompilation_type =
        decomp_blocks->pairs[i].value;
    uint32_t end_address = decompilation_type->end_offset;

    uint32_interval_add(block_intervals, decompilation_addr, end_address);
  }
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

struct decomp_visit_state {
  struct lookahead lookahead;
  size_t line_length;
  const struct language_def *base_language;
  const struct rule *base_command;
  const struct language_def *curr_language;
  const struct rule *curr_command;
  uint32_t address;
  uint32_t initial_address;
  size_t next_label_index;
  uint32_t next_label_address;
  bool still_going;
  enum decompilation_stage decomp_stage;
  // reported by decomp_visit_single: whether to continue on the same line
  bool continue_line;
  // informative for decomp_visit_single: whether this started on a new line
  bool is_new_line;
};

static const struct language_def *decomp_get_next_language(
    struct decomp_internal_state *state, struct decomp_visit_state *visit_state,
    struct language lang) {
  if (lang.is_prefixed) {
    if (lang.name[0] == '\0') {
      // Language recursion, i.e. `@*`
      return visit_state->base_language;
    }

    size_t lang_name_len = strlen(lang.name);
    const struct language_def *next = get_prefixed_language(
        state->language_cache, visit_state->base_language->name, lang.name,
        lang_name_len);
    if (next != NULL) {
      return next;
    }

    char **parents = visit_state->base_language->parents;
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

struct lang_and_address {
  struct language lang;
  uint32_t address;
};

static void specialcase_adjust_lang_and_address(struct lang_and_address *l) {
  if (strcmp(l->lang.name, "gba_asm") == 0) {
    l->lang.name = l->address & 1 ? "gba_thumb_asm" : "gba_arm_asm";
    l->address &= ~1;
  }
}

static void queue_decompilation_at(struct bsearch_root *remaining_blocks,
                                   uint32_t next_address,
                                   const struct language_def *next_language,
                                   const struct rule *command) {
  struct queued_decompilation *next_decompilation =
      malloc(sizeof *next_decompilation);
  next_decompilation->language = next_language;
  next_decompilation->command = command;

  // TODO: GSC offset handling
  bsearch_upsert(remaining_blocks,
                 CAST_u32_pvoid(next_address & ~ROM_BASE_ADDRESS),
                 next_decompilation);
}

static uint32_t read_addr_from_lookahead(struct lookahead *lookahead,
                                         size_t arg_size) {
  size_t addr_size = 4;
  if (arg_size < addr_size) {
    addr_size = arg_size;
  }
  return arr_get_little_endian(lookahead->bytes.bytes, addr_size);
}

static bool lang_can_split_lines(const struct decomp_visit_state *state) {
  const struct language_def *language = state->curr_language;
  switch (language->meta_flags & METAFLAG_MASK_LANGTYPE) {
    case METAFLAG_LANGTYPE_TEXT:
    case METAFLAG_LANGTYPE_LINE: {
      // Can't wrap a language if it would force another command
      // and its arguments to be written when they are of non-zero length.
      const struct rule *command = state->base_command;
      if (command != NULL &&
          (command->bytes.length != 0 || command->args.length != 0)) {
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
    if (visit_state->decomp_stage == STAGE_DECOMPILING) {
      fputs("\n' EOF\n", state->output);
    }
    visit_state->still_going = false;
    return true;
  }
  return false;
}

static void initialise_label_search(
    struct decomp_internal_state *restrict state,
    struct decomp_visit_state *restrict visit_state) {
  switch (visit_state->decomp_stage) {
    case STAGE_FINDING_BLOCKS:
      // No labels this early on
      break;
    case STAGE_FINDING_LABELS: {
      // TODO
      ptrdiff_t next_label_index = bsearch_find_u32(
          state->explored_blocks, visit_state->initial_address);
      // We should always be looking through a block already found by
      // STAGE_FINDING_BLOCKS, so next_label_index should always be >= 0
      assert(next_label_index >= 0);
      assert((size_t)next_label_index < state->explored_blocks->size);
      visit_state->next_label_index = (size_t)next_label_index;
      visit_state->next_label_address =
          bsearch_key_u32(state->explored_blocks, (size_t)next_label_index);
      break;
    }
    case STAGE_DECOMPILING: {
      ptrdiff_t next_label_index =
          bsearch_find_u32(state->labels, visit_state->initial_address);
      if (next_label_index < 0) {
        next_label_index = -next_label_index - 1;
      }
      visit_state->next_label_index = (size_t)next_label_index;
      if ((size_t)next_label_index < state->labels->size) {
        visit_state->next_label_address =
            bsearch_key_u32(state->labels, (size_t)next_label_index);
      }
      break;
    }
  }
}

static void label_visited_address(struct decomp_internal_state *state,
                                  struct decomp_visit_state *visit_state) {
  switch (visit_state->decomp_stage) {
    case STAGE_FINDING_BLOCKS:
      // No labels this early on
      break;
    case STAGE_FINDING_LABELS:
      assert(state->explored_blocks != NULL);

      // If we want to label something, we first need to be able start a new
      // line
      if (!visit_state->is_new_line && !lang_can_split_lines(visit_state))
        break;

      // Skip over any block addresses we have already passed
      while (visit_state->address > visit_state->next_label_address &&
             visit_state->next_label_index < state->explored_blocks->size) {
        visit_state->next_label_index++;
        if (visit_state->next_label_index < state->explored_blocks->size) {
          visit_state->next_label_address = bsearch_key_u32(
              state->explored_blocks, visit_state->next_label_index);
        } else {
          visit_state->next_label_address = UINT32_MAX;
        }
      }

      // Check that we have reached the point where we may want to label
      if (visit_state->address != visit_state->next_label_address) break;

      // Ensure the address can be labelled by this block
      struct queued_decompilation *decompilation =
          state->explored_blocks->pairs[visit_state->next_label_index].value;

      if (decompilation->language != visit_state->curr_language ||
          decompilation->command != visit_state->curr_command) {
        // This address cannot be labelled by this block
        // (Because it would be represented incorrectly)
        return;
      }

      // Take ownership of the label, minimum start address wins
      ptrdiff_t index =
          bsearch_find_u32(state->label_blocks, visit_state->address);
      if (index >= 0) {
        if (bsearch_val_u32(state->label_blocks, index) >
            visit_state->initial_address) {
          // We have visited this address before, but this time we have a
          // fuller view of the block
          bsearch_setval_u32(state->label_blocks, index,
                             visit_state->initial_address);
        }
      } else {
        // Newly visited address
        bsearch_unsafe_insert(state->label_blocks, index,
                              CAST_u32_pvoid(visit_state->address),
                              CAST_u32_pvoid(visit_state->initial_address));
      }
      break;
    case STAGE_DECOMPILING:
      // Insert labels if needed

      // Labels must be their own line
      // (And we can't ask for another split, that should have happened
      // already)
      // TODO: do we actually split lines for labels? check with text or
      // something
      // TODO: handle case where label is passed without inserting
      // TODO: ensure we advance only to an applicable label (i.e. check
      // ownership)
      if (!visit_state->is_new_line) return;

      if (visit_state->address == visit_state->next_label_address &&
          visit_state->next_label_index < state->labels->size) {
        fprintf(
            state->output, ":%s\n",
            (char *)state->labels->pairs[visit_state->next_label_index].value);
        visit_state->next_label_index++;
        if (visit_state->next_label_index < state->labels->size) {
          visit_state->next_label_address =
              bsearch_key_u32(state->labels, visit_state->next_label_index);
        }
      }
      break;
    default:
      assert(false);
  }
}

static void decomp_visit_single(struct decomp_internal_state *state,
                                struct decomp_visit_state *visit_state) {
  const struct language_def *language = visit_state->curr_language;
  uint32_t command_start_address = visit_state->address;

  if (check_and_report_eof(state, visit_state, 1)) return;

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
    if (visit_state->decomp_stage == STAGE_DECOMPILING) {
      putc(translated_byte, state->output);
      visit_state->line_length++;
    }
    visit_state->address++;
    consume_and_refill_lookahead(&visit_state->lookahead, 1);
  } else {
    if (matched_rule == NULL) {
      matched_rule = language->special_rules[SPECIAL_RULE_DEFAULT];
      if (matched_rule == NULL) {
        // No matched rule, no default rule, so decompilation can't continue.
        visit_state->still_going = false;
        return;
      }
    }

    size_t cmd_first_len;
    if (language_type == METAFLAG_LANGTYPE_TEXT) {
      // Text languages should not have split commands
      cmd_first_len = strlen(matched_rule->command_name);
    } else {
      cmd_first_len = strcspn(matched_rule->command_name, " ");
    }

    if (visit_state->decomp_stage == STAGE_DECOMPILING) {
      if (matched_rule->attributes & RULE_ATTR_CMP_FLAG)
        state->info.is_checkflag = true;
      else if (matched_rule->attributes & RULE_ATTR_CMP_INT)
        state->info.is_checkflag = false;

      // If not a text language, we can prefix the command name with
      // the current language name if in verbose mode.
      if (state->is_verbose && language_type != METAFLAG_LANGTYPE_TEXT) {
        assert(language->name[0] != '\0');
        fputs(language->name, state->output);
        fputc(':', state->output);
      }

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

      if (visit_state->decomp_stage == STAGE_DECOMPILING) {
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
              state->parser_cache, visit_state->base_language,
              matched_rule->args.args[i].parsers, value, &state->info);

          switch (result.type) {
            default:
            case PARSE_RESULT_FAIL:
              // should always be able to parse at least fallbacks
              assert(false);
              break;
            case PARSE_RESULT_VALUE:
              // this shouldn't be returned by a formatter
              assert(false);
              break;
            case PARSE_RESULT_TOKEN:
              if (state->is_verbose) {
                const char *name;
                if (result.parser->which == PARSER_TYPE_BUILTIN) {
                  name = result.parser->builtin.name;
                } else {
                  // if result.parser->which == PARSER_TYPE_LOADED
                  name = result.parser->loaded.name;
                }
                fputs(name, state->output);
                fputc(':', state->output);
              }
              fputs(result.token, state->output);
              visit_state->line_length += strlen(result.token);
              free(result.token);
              break;
            case PARSE_RESULT_LABEL: {
              // TODO: GSC offset handling
              uint32_t offset = result.value & ~ROM_BASE_ADDRESS;
              ptrdiff_t label_index = bsearch_find_u32(state->labels, offset);
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
                if (state->is_verbose) {
                  fputs(builtin_parser_hex.builtin.name, state->output);
                  fputc(':', state->output);
                }
                fputs(result.token, state->output);
                visit_state->line_length += strlen(result.token);
                free(result.token);
              }
              break;
            }
          }
        }
      } else if (visit_state->decomp_stage == STAGE_FINDING_BLOCKS) {
        // If collecting block info, we need to follow addresses
        // Handle request for a new language
        bool has_lang = arg->as_language.lang.name[0] != '\0' ||
                        arg->as_language.lang.is_prefixed;
        if (has_lang || arg->as_language.command[0] != '\0') {
          const struct language_def *next_language = language;
          const struct rule *command = NULL;

          struct lang_and_address next_lang_and_addr = {
              .lang = arg->as_language.lang,
              .address =
                  read_addr_from_lookahead(&visit_state->lookahead, arg->size),
          };

          if (has_lang) {
            specialcase_adjust_lang_and_address(&next_lang_and_addr);

            next_language = decomp_get_next_language(state, visit_state,
                                                     next_lang_and_addr.lang);
            if (next_language == NULL) {
              fprintf(stderr, "Warning: language \"%s\" not found\n",
                      next_lang_and_addr.lang.name);
              next_language = &error_lang;
            }
          }
          if (arg->as_language.command[0] != '\0') {
            ptrdiff_t index = bsearch_find(next_language->rules_by_command_name,
                                           arg->as_language.command);
            if (index < 0) {
              fprintf(stderr,
                      "Warning: command \"%s\" not found in language \"%s\"\n",
                      arg->as_language.command, next_language->name);
              next_language = &error_lang;
            } else {
              command =
                  next_language->rules_by_command_name->pairs[index].value;
            }
          }
          queue_decompilation_at(state->remaining_blocks,
                                 next_lang_and_addr.address, next_language,
                                 command);
        }
      }

      consume_and_refill_lookahead(&visit_state->lookahead, arg->size);
      visit_state->address += arg->size;

      // Special case for commands with spaces in (insert second half after
      // first argument)
      if (i == 0 && visit_state->decomp_stage == STAGE_DECOMPILING &&
          matched_rule->command_name[cmd_first_len] == ' ') {
        fputs(&matched_rule->command_name[cmd_first_len], state->output);
        visit_state->line_length +=
            strlen(&matched_rule->command_name[cmd_first_len]);
      }
    }

    // Handle oneshot language
    if (matched_rule->oneshot_lang.name[0] != '\0') {
      const struct language_def *next_language = decomp_get_next_language(
          state, visit_state, matched_rule->oneshot_lang);
      if (next_language == NULL) {
        if (visit_state->decomp_stage == STAGE_DECOMPILING) {
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

      if (visit_state->decomp_stage == STAGE_DECOMPILING &&
          language_type != METAFLAG_LANGTYPE_TEXT) {
        putc(' ', state->output);
        visit_state->line_length++;
      }

      visit_state->curr_language = next_language;
      visit_state->curr_command = NULL;
      visit_state->continue_line = true;
      return;
    }
  }

  if (visit_state->decomp_stage == STAGE_DECOMPILING) {
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

static uint32_t decomp_visit_address(
    struct decomp_internal_state *restrict state,
    struct queued_decompilation *restrict decompilation_type,
    uint32_t initial_address, enum decompilation_stage decomp_stage) {
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
      .decomp_stage = decomp_stage,
      .line_length = 0,
      .base_language = language,
      .base_command = decompilation_type->command,
      .curr_language = language,
      .curr_command = decompilation_type->command,
      .continue_line = false,
      .is_new_line = true,
      .next_label_address = UINT32_MAX,
      .next_label_index = SIZE_MAX,
  };

  // TODO: GSC offset handling
  fseek(state->input, initial_address, SEEK_SET);
  consume_and_refill_lookahead(&visit_state.lookahead, 0);

  initialise_label_search(state, &visit_state);

  while (visit_state.still_going) {
    // Insert or discover labels if needed
    label_visited_address(state, &visit_state);

    visit_state.continue_line = false;
    decomp_visit_single(state, &visit_state);

    if (!visit_state.continue_line) {
      if (visit_state.decomp_stage == STAGE_DECOMPILING) {
        fputs("\n", state->output);
        visit_state.line_length = 0;
      }
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
  return visit_state.address;
}
