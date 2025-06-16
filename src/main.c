#define SAVE_SCREEN

#define USE_TI89
#define USE_TI92PLUS
#define USE_V200

/* meow? */
#ifdef __CLANG__
#define asm(x)
#endif /* __CLANG__ */

#include <tigcclib.h>
#include "menu.h"

/*
  draw_menu_border (x, y, w, h)
  draw_menu_cursor (x, y)
  draw_menu_text   (fmt, x, y, ...)

 -------------
  fight   bag
  monst   run
 -------------

 // ... somewhere in the game loop ...

 if(menu_active) {
    global arr of 3-tuples = 
    
    [ 
      (cursorx, cursory, function callback),
      ....
    ]
 }
*/

void _main(void) {
    clrscr();
    // printf("Hello!");
    DrawMenuBorder(0, 24, 64, 72);
    ngetchx();
}
