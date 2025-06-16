#define SAVE_SCREEN

#define USE_TI89
#define USE_TI92PLUS
#define USE_V200

/* meow? */
#ifdef __CLANG__
#define asm(x)
#endif /* __CLANG__ */

#include <stdio.h>
#include <kbd.h>

void _main(void) {
    clrscr();
    printf("Hello!");
    ngetchx();
}
