#include <tigcclib.h>

#include "menu.h"

/*
  draw_menu_border (x, y, w, h)
  draw_menu_cursor (x, y)
  draw_menu_text   (fmt, x, y, ...)

 -------------
  fight   bag
  pkmn    run
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

/* assume w, h >= 8. otherwise this breaks terribly */
void DrawMenuBorder(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  static const unsigned char sprite[] = { 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF }; // temp

  /*
     ===
     ===

     XX
     XX

     ||
     ||
     ||
  */

  #define HEIGHT 4
  // Sprite8(x, y, 8, sprite, LCD_MEM, SPRT_XOR); // 1 top
  for(uint8_t i = 0; i <= w; i += 8) {
    /* draw on the top and bottom */
    Sprite8(x + i, y, HEIGHT, sprite, LCD_MEM, SPRT_XOR); // 2-w top
    Sprite8(x + i, y + h, HEIGHT, sprite, LCD_MEM, SPRT_XOR); // 2-w bot
  }

  for(uint8_t i = 8; i <= h - HEIGHT; i += 8) {
    /* draw the left and right */
    Sprite8(x, y + i, HEIGHT, sprite, LCD_MEM, SPRT_XOR); // 2-h left
    Sprite8(x+w, y + i, HEIGHT, sprite, LCD_MEM, SPRT_XOR); // 2-h right
  }
  // Sprite8(x+w, y, 8, sprite, LCD_MEM, SPRT_XOR); // bot right

}
