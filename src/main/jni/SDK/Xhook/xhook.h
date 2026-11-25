
#ifndef XHOOK_H
#define XHOOK_H 1

#ifdef __cplusplus
extern "C" {
#endif

#define XHOOK_EXPORT __attribute__((visibility("default")))

int xhook_register(const char  *pathname_regex_str,
                   const char  *symbol,
                   void        *new_func,
                   void       **old_func);

int xhook_ignore(const char *pathname_regex_str,
                 const char *symbol);

int xhook_refresh(int async);

void xhook_clear();

void xhook_enable_debug(int flag);

void xhook_enable_sigsegv_protection(int flag);

#ifdef __cplusplus
}
#endif

#endif
