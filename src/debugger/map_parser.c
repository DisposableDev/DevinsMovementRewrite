#include <ultra64.h>
#include <macros.h>
#include <PR/os_internal_error.h>
#include <stdarg.h>
#include <string.h>
#include "game/memory.h"

#include "map_parser.h"
#include "symtable.h"

#ifdef DEBUG_EXPORT_SYMBOLS

#define UNKNOWN_SYMBOL "???"

char* __symbolize(void *vaddr, char *buf, int size, u32 andOffset) {
    if (buf == NULL || size <= 0) {
        return buf;
    }
    symtable_header_t symt = symt_open();
    if (symt.head[0] && size > 12) {
        u32 addr = (u32)vaddr;
        int idx = 0;
        addrtable_entry_t a = symt_addrtab_search(&symt, addr, &idx);
        while (idx > 0 && !ADDRENTRY_IS_FUNC(a)) {
            a = symt_addrtab_entry(&symt, --idx);
        }

        symtable_entry_t ALIGNED16 entry;
        // Read the symbol name
        if (!ADDRENTRY_IS_FUNC(a) || !symt_entry_fetch(&symt, &entry, idx)) {
            int count = MIN(sizeof(UNKNOWN_SYMBOL) - 1, (u32) size - 1);
            memcpy(buf, UNKNOWN_SYMBOL, count);
            buf[count] = '\0';
            return buf;
        }
        char *func = symt_entry_func(&symt, &entry, addr, buf, size-12);
        char lbuf[12];
        if (andOffset) {
            sprintf(lbuf, "+0x%lx", addr - ADDRENTRY_ADDR(a));
        } else {
            lbuf[0] = 0;
        }
        entry.func_off = addr - ADDRENTRY_ADDR(a);

        return strcat(func, lbuf);
    }
    int count = MIN(sizeof(UNKNOWN_SYMBOL) - 1, (u32) size - 1);
    memcpy(buf, UNKNOWN_SYMBOL, count);
    buf[count] = '\0';
    return buf;
}

char *parse_map(char *buf, u32 size, u32 addr, u32 andOffset) {
    if (buf == NULL || size == 0) {
        return buf;
    }
    __symbolize((void *) addr, buf, size, andOffset);
    return buf;
}

u32 get_start_of_func(u32 addr) {
    symtable_header_t symt = symt_open();
    if (symt.head[0]) {
        int idx = 0;
        addrtable_entry_t a = symt_addrtab_search(&symt, addr, &idx);
        while (idx > 0 && !ADDRENTRY_IS_FUNC(a)) {
            a = symt_addrtab_entry(&symt, --idx);
        }

        if (ADDRENTRY_IS_FUNC(a)) {
            return ADDRENTRY_ADDR(a);
        }
    }
    return addr;
}

symtable_info_t get_symbol_info(u32 addr) {
    static char filebuf[100];
    void *vaddr = (void *)addr;
    symtable_header_t symt = symt_open();

    if (symt.head[0]) {
        symtable_info_t info;
        u32 addr = (u32)vaddr;
        int idx = 0;
        symt_addrtab_search(&symt, addr, &idx);

        symtable_entry_t ALIGNED16 entry;

        // Read the symbol name
        filebuf[0] = 0;
        if (!symt_entry_fetch(&symt, &entry, idx)) {
            return (symtable_info_t){ .line = -1, .file = UNKNOWN_SYMBOL, .func = UNKNOWN_SYMBOL };
        }
        info.line = entry.line;
        info.func_offset = entry.func_off;
        addrtable_entry_t a = symt_addrtab_entry(&symt, idx);
        info.distance = addr - ADDRENTRY_ADDR(a);
        info.file = symt_entry_file(&symt, &entry, filebuf, sizeof(filebuf));
        parse_map(info.func, sizeof(info.func), addr, FALSE);

        return info;
    }
    return (symtable_info_t){ .line = -1, .file = UNKNOWN_SYMBOL, .func = UNKNOWN_SYMBOL };
}

#endif // DEBUG_EXPORT_SYMBOLS
