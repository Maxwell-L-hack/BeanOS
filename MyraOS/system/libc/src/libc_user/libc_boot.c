#include <reent.h>

extern struct _reent _impure_data;
extern struct _reent* _impure_ptr;
extern void __sinit(struct _reent *);

void _libc_reent_init(void) {
    _impure_ptr = &_impure_data;
    _REENT_INIT_PTR(_impure_ptr);
    __sinit(_impure_ptr); 
}

void _init(void) {}

void _fini(void) {}