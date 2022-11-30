#ifndef PKSV_CODEPROC_H
#define PKSV_CODEPROC_H 1

#include <stdint.h>
#include <stdbool.h>
#include "binarysearch.h"
#include "sulib.h"

void pksv_codeproc_reset(void);

/** Find a consecutive block of free space.
 *  @param rom_search The ROM file to search for free space
 *  @param len The length of the returned block of free space
 *  @param align The alignment of the returned block of free space
 *  @param offset Where to start searching for free space
 *  @param min_address The minimum address to return (even if earlier free space
 *                     is known)
 *  @param search The byte value considered to be free space
 *  @param free_intervals A collection of definitely-free-space intervals
 *  @return The ROM address of a block of free space of length `len`,
 *          or UINT32_MAX on error.
 */
uint32_t FindFreeSpace(FILE *rom_search, uint32_t len, uint32_t align,
                       uint32_t *offset, uint32_t *min_address, uint8_t search,
                       struct bsearch_root *free_intervals);

signed int PointerToOffset(unsigned int ptr);
signed int OffsetToPointer(unsigned int offset);

bool gsc_are_banks_equal(uint32_t offset1, uint32_t offset2);
uint32_t gsc_next_bank(uint32_t offset);

#endif
