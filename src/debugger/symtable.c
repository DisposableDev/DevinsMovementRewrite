#include <ultra64.h>
#include <macros.h>
#include <PR/os_internal_error.h>
#include <stdarg.h>
#include <string.h>
#include "segments.h"
#include "game/memory.h"

#include "symtable.h"
#include "crash_screen.h"

#ifdef DEBUG_EXPORT_SYMBOLS

u32 SYMT_ROM = 0xFFFFFFFF;
extern u8 _mapDataSegmentRomStart[];
// The start and end of the exception vector
extern u8 __osExceptionPreamble[];
extern u8 send_mesg[];

#define is_in_exception(addr) ((addr) >= (u32)__osExceptionPreamble && (addr) < (u32)send_mesg)

extern u8 _mapDataSegmentRomEnd[];
static s32 sSymbolReadFailed;

#define SYMBOL_READ_TIMEOUT_US 20000

static s32 symt_rom_read(u32 offset, void *destination, u32 size) {
    u8 *bytes = destination;
    u32 end = (u32) _mapDataSegmentRomEnd;

    if (sSymbolReadFailed || offset < SYMT_ROM || offset > end || size > end - offset) {
        return FALSE;
    }
    while (size != 0) {
        u32 start = osGetCount();
        u32 word;
        OSIntMask mask;

        // Check and read atomically against the PI manager, without taking its lock.
        for (;;) {
            if (crash_screen_reset_pending()
                || (u32) (osGetCount() - start) >= OS_USEC_TO_CYCLES(SYMBOL_READ_TIMEOUT_US)) {
                sSymbolReadFailed = TRUE;
                return FALSE;
            }
            mask = osSetIntMask(OS_IM_NONE);
            if (!(IO_READ(PI_STATUS_REG) & (PI_STATUS_IO_BUSY | PI_STATUS_DMA_BUSY))) {
                word = IO_READ((u32) osRomBase | (offset & ~3U));
                osSetIntMask(mask);
                break;
            }
            osSetIntMask(mask);
        }
        do {
            *bytes++ = word >> (24 - (offset & 3) * 8);
            offset++;
            size--;
        } while (size != 0 && (offset & 3) != 0);
    }
    return TRUE;
}

static s32 symt_table_fits(u32 offset, u32 count, u32 entrySize) {
    u32 size = (u32) _mapDataSegmentRomEnd - SYMT_ROM;

    return offset >= sizeof(symtable_header_t) && offset <= size
           && count <= (size - offset) / entrySize;
}

/** 
 * @brief Open the SYMT symbol table in the rompak.
 * 
 * If not found, return a null header.
 */
symtable_header_t symt_open(void) {
    symtable_header_t header = { 0 };

    SYMT_ROM = (u32) _mapDataSegmentRomStart;
    if (SYMT_ROM == 0 || !symt_rom_read(SYMT_ROM, &header, sizeof(header))) {
        return (symtable_header_t){ 0 };
    }
    if (memcmp(header.head, "SYMT", 4) != 0 || header.version != 2 || header.addrtab_size == 0
        || header.addrtab_size != header.symtab_size
        || !symt_table_fits(header.addrtab_off, header.addrtab_size, sizeof(addrtable_entry_t))
        || !symt_table_fits(header.symtab_off, header.symtab_size, sizeof(symtable_entry_t))
        || !symt_table_fits(header.strtab_off, header.strtab_size, 1)) {
        return (symtable_header_t){ 0 };
    }
    return header;
}

/**
 * @brief Return an entry in the address table by index
 * 
 * @param symt      SYMT file header
 * @param idx       Index of the entry to return
 * @return addrtable_entry_t  Entry of the address table
 */
addrtable_entry_t symt_addrtab_entry(symtable_header_t *symt, int idx) {
    addrtable_entry_t entry = 0;

    if (idx >= 0 && (u32) idx < symt->addrtab_size) {
        symt_rom_read(SYMT_ROM + symt->addrtab_off + idx * sizeof(entry), &entry, sizeof(entry));
    }
    return entry;
}

/**
 * @brief Search the SYMT address table for the given address.
 * 
 * Run a binary search to find the entry in the table. If there is a single exact match,
 * the entry is returned. If there are multiple entries with the same address, the first
 * entry is returned (this is the case for inlined functions: so some entries following
 * the current one will have the same address). If there is no exact match, the entry
 * with the biggest address just before the given address is returned.
 *
 * @param symt      SYMT file header
 * @param addr      Address to search for
 * @param idx       If not null, will be set to the index of the entry found (or the index just before)
 * @return          The found entry (or the entry just before)
 */
addrtable_entry_t symt_addrtab_search(symtable_header_t *symt, u32 addr, int *idx) {
    int min = 0;
    int max = symt->addrtab_size - 1;

    if (idx != NULL) {
        *idx = -1;
    }
    if (symt->head[0] == 0 || symt->addrtab_size == 0 || sSymbolReadFailed) {
        return 0;
    }
    while (min < max) {
        int mid = (min + max) / 2;
        addrtable_entry_t entry = symt_addrtab_entry(symt, mid);
        if (addr <= ADDRENTRY_ADDR(entry)) {
            max = mid;
        } else {
            min = mid + 1;
        }
    }
    addrtable_entry_t entry = symt_addrtab_entry(symt, min);
    if (min > 0 && ADDRENTRY_ADDR(entry) > addr) {
        entry = symt_addrtab_entry(symt, --min);
    }
    if (sSymbolReadFailed || ADDRENTRY_ADDR(entry) > addr) {
        return 0;
    }
    if (idx != NULL) {
        *idx = min;
    }
    return entry;
}

/**
 * @brief Fetch a string from the string table
 * 
 * @param symt  SYMT file
 * @param sidx  Index of the first character of the string in the string table
 * @param slen  Length of the string
 * @param buf   Destination buffer
 * @param size  Size of the destination buffer
 * @return char*  Fetched string within the destination buffer (might not be at offset 0 for alignment reasons)
 */
char *symt_string(symtable_header_t *symt, int sidx, int slen, char *buf, int size) {
    if (size <= 0) {
        return buf;
    }
    buf[0] = '\0';
    if (sidx < 0 || slen < 0 || (u32) sidx > symt->strtab_size
        || (u32) slen > symt->strtab_size - sidx) {
        return buf;
    }
    int count = MIN(slen, size - 1);
    if (symt_rom_read(SYMT_ROM + symt->strtab_off + sidx, buf, count)) {
        buf[count] = '\0';
    } else {
        buf[0] = '\0';
    }
    return buf;
}

/**
 * @brief Fetch a symbol table entry from the SYMT file.
 * 
 * @param symt    SYMT file
 * @param entry   Output entry pointer
 * @param idx     Index of the entry to fetch
 */
s32 symt_entry_fetch(symtable_header_t *symt, symtable_entry_t *entry, int idx) {
    memset(entry, 0, sizeof(*entry));
    if (idx < 0 || (u32) idx >= symt->symtab_size) {
        return FALSE;
    }
    return symt_rom_read(SYMT_ROM + symt->symtab_off + idx * sizeof(*entry), entry, sizeof(*entry));
}

// Fetch the function name of an entry
char *symt_entry_func(symtable_header_t *symt, symtable_entry_t *entry, u32 addr, char *buf, int size) {
    if (is_in_exception(addr)) {
        // Special case exception handlers. This is just to show something slightly
        // more readable instead of "notcart+0x0" or similar assembly symbols
        if (size > 0) {
            int count = MIN(sizeof("<EXCEPTION HANDLER>") - 1, (u32) size - 1);
            memcpy(buf, "<EXCEPTION HANDLER>", count);
            buf[count] = '\0';
        }
        return buf;
    } else {
        return symt_string(symt, entry->func_sidx, entry->func_len, buf, size);
    }
}

// Fetch the file name of an entry
char *symt_entry_file(symtable_header_t *symt, symtable_entry_t *entry, char *buf, int size) {
    return symt_string(symt, entry->file_sidx, entry->file_len, buf, size);
}

#endif // DEBUG_EXPORT_SYMBOLS
