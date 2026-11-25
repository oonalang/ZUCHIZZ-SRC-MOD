
#ifndef XH_ELF_H
#define XH_ELF_H 1

#include <stdint.h>
#include <elf.h>
#include <link.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    const char *pathname;

    ElfW(Addr)  base_addr;
    ElfW(Addr)  bias_addr;

    ElfW(Ehdr) *ehdr;
    ElfW(Phdr) *phdr;

    ElfW(Dyn)  *dyn;
    ElfW(Word)  dyn_sz;

    const char *strtab;
    ElfW(Sym)  *symtab;

    ElfW(Addr)  relplt;
    ElfW(Word)  relplt_sz;

    ElfW(Addr)  reldyn;
    ElfW(Word)  reldyn_sz;

    ElfW(Addr)  relandroid;
    ElfW(Word)  relandroid_sz;

    uint32_t   *bucket;
    uint32_t    bucket_cnt;
    uint32_t   *chain;
    uint32_t    chain_cnt;

    uint32_t    symoffset;
    ElfW(Addr) *bloom;
    uint32_t    bloom_sz;
    uint32_t    bloom_shift;

    int         is_use_rela;
    int         is_use_gnu_hash;
} xh_elf_t;

int xh_elf_init(xh_elf_t *self, uintptr_t base_addr, const char *pathname);
int xh_elf_hook(xh_elf_t *self, const char *symbol, void *new_func, void **old_func);

int xh_elf_check_elfheader(uintptr_t base_addr);

#ifdef __cplusplus
}
#endif

#endif
