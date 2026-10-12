#define _GNU_SOURCE
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

typedef uint32_t u32;
typedef struct {
    u32 sp, ra;
} __OSThreadContext;
typedef struct {
    __OSThreadContext context;
} OSThread;
typedef struct {
    char func[64];
    int line;
    u32 func_offset, distance;
} symtable_info_t;
typedef struct {
    char head[4];
} symtable_header_t;
typedef u32 addrtable_entry_t;
typedef u32 Insn;
#define ADDRENTRY_ADDR(addr) ((addr) & ~3u)
#include "stack_definitions.inc"
static StackFrame stack[STACK_LINE_COUNT];
static u32 stackIdx, osMemSize = 4096;
static int indirect, symbolsMissing;
static symtable_header_t symt_open(void) {
    return (symtable_header_t){ .head = { symbolsMissing ? 0 : 'S' } };
}
static symtable_info_t get_symbol_info(u32 addr) {
    (void) addr;
    symtable_info_t info = { .line = 123 };
    memset(info.func, 'x', sizeof(info.func) - 1);
    return info;
}
static addrtable_entry_t symt_addrtab_search(symtable_header_t *symt, u32 addr, int *idx) {
    assert(symt->head[0] != 0);
    (void) addr;
    (void) idx;
    return 0x80000080;
}
static u32 get_start_of_func(u32 addr) {
    return addr;
}
static int is_top_of_stack(u32 addr) {
    (void) addr;
    return 0;
}
static int is_text_addr(u32 addr) {
    return addr == 0x80000080;
}
static int insn_is_jal(Insn *insn) {
    (void) insn;
    return 0;
}
static int insn_is_jalr(Insn *insn) {
    (void) insn;
    return indirect == 1;
}
static int insn_is_j(Insn *insn) {
    (void) insn;
    return indirect == 2;
}
#include "stack_actual.inc"

int main(void) {
    // The following page stays inaccessible so an overrun fails immediately.
    void *ram = mmap((void *) 0x80000000, 8192, PROT_NONE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    assert(ram == (void *) 0x80000000);
    assert(mprotect(ram, 4096, PROT_READ | PROT_WRITE) == 0);
    OSThread thread = { { 0x80000200, 0x80000080 } };
    assert(generate_stack(&thread) == 0);
    // A candidate beyond the traversal budget must not be reached.
    *(u32 *) 0x80000394 = 0x80000088;
    indirect = 1;
    assert(generate_stack(&thread) == 0);
    *(u32 *) 0x80000204 = 0x80000088;
    assert(generate_stack(&thread) == 1);
    assert(stack[0].func == 0x80000080 && strlen(stack[0].funcname) == 63);
    indirect = 2;
    assert(generate_stack(&thread) == 1);
    // Repeated candidates must not duplicate the current frame.
    *(u32 *) 0x80000208 = 0x80000088;
    assert(generate_stack(&thread) == 1);
    thread.context.sp = 0x80000FFC;
    assert(generate_stack(&thread) == 0);
    thread.context.sp = 0x80001000;
    assert(generate_stack(&thread) == 2);
    thread.context.sp = 0x80000201;
    assert(generate_stack(&thread) == 2);
    thread.context.sp = 0xC0000000;
    assert(generate_stack(&thread) == 2);
    // Missing symbols must never attempt an address-table read.
    symbolsMissing = 1;
    thread.context.sp = 0x80000200;
    assert(generate_stack(&thread) == 1);
    assert(strcmp(stack[0].funcname, "Symbols unavailable") == 0);
    // Exercise the entire frame-name capacity plus the largest signed line.
    memset(stack[0].funcname, 'x', sizeof(stack[0].funcname) - 1);
    stack[0].funcname[sizeof(stack[0].funcname) - 1] = '\0';
    stack[0].line = INT32_MAX;
    assert(strlen(get_stack_entry(0)) == 120);
    stack[0].line = -1;
    assert(strlen(get_stack_entry(0)) == 109);
    assert(munmap(ram, 8192) == 0);
    return 0;
}
