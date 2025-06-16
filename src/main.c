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
    border_pattern_t DEFAULT_BORDER = (border_pattern_t) {
      .pattern.split.pattern_h = {0xF0,0x00,0x00,0xF0, 0x00, 0x00, 0x00, 0x00},
      .pattern.split.pattern_v = {0x90,0x90,0x90,0x90, 0x00, 0x00, 0x00, 0x00},
      .corners.split.corners_bl= {0xA0,0xB0,0x80,0xF0, 0x00, 0x00, 0x00, 0x00},
      .corners.split.corners_tr= {0xF0,0x10,0xD0,0x50, 0x00, 0x00, 0x00, 0x00},
      .corners.split.corners_tl= {0xF0,0x80,0xB0,0xA0, 0x00, 0x00, 0x00, 0x00},
      .corners.split.corners_br= {0x50,0xD0,0x10,0xF0, 0x00, 0x00, 0x00, 0x00},
      .width = 4,
      .height = 4
    };
    DrawMenuBorder(&DEFAULT_BORDER, 0, 76, 160-4, 20);
    FontSetSys (F_4x6);
    // y: 76 + border + padding
    // x: border + padding
    DrawStr (4+2, 76+4+2, "the quick brown cat jumped over the lazy", A_NORMAL);
    DrawStr (4+2, 76+4+2+6, "fox and meowed until 12:00pm! bye-bye :)", A_NORMAL);
    // y: 76 + border + padding + font height of prev line
    ngetchx();
}
