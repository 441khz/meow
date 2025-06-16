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

/*
inline void _Sprite8Rot90InPlace(uint8_t *mat, uint8_t dim)
{
  // im quite worried about codegen here. there is no native 64 bit support
  // on this microprocessor.... this will not be pretty.
  // this'll work, but i hate it, and there's probably a better soln.

  uint16_t x = *(uint16_t *)mat; // mat is a set of packed 8 packed u8s
  // sprite values are packed so this is a little wonky
  uint64_t r = 0;
  for (uint8_t i = 0; i < 16; ++i) {
    r += ((x >> i) & 1) << (((i % 4) * 4) + (3 - i / 4));
  } // https://stackoverflow.com/questions/1667591/rotating-a-bitmap-90-degrees
  *(uint64_t *)mat = r; // oof! don't look at the assembler...
} */

inline void DrawSimpleMenuBorder(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  WIN_RECT rect = {x, y, x+w, y+h};
  DrawClipRect (&rect, ScrRect, A_NORMAL);
}

/**
 * @brief Given a pattern, draw a menu border of specific size and shape.
 * @param[pattern] The pattern to draw
 * optimization potential ...
 * computing the mtx transpose just in time would save a little memory if we have many borders.
 * maybe we could draw big boxes with sprite16/32, somehow memcpying and then drawing with sprite16/32
 * not sure if it would be much faster, id have to benchmark it.
 */
void DrawMenuBorder(border_pattern_t *pattern, uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  if(!pattern) {
     assert(0);
     return;
  }

  /* draw corners first */
  Sprite8(x, y,         pattern->height, pattern->corners.split.corners_tl, LCD_MEM, SPRT_XOR); // top left
  Sprite8(x + w, y,     pattern->height, pattern->corners.split.corners_tr, LCD_MEM, SPRT_XOR); // top right
  Sprite8(x, y + h,     pattern->height, pattern->corners.split.corners_bl, LCD_MEM, SPRT_XOR); // bot left
  Sprite8(x + w, y + h, pattern->height, pattern->corners.split.corners_br, LCD_MEM, SPRT_XOR); // bot right

  /* draw the top&bottom bars */
  for(uint8_t i = pattern->width; i <= w - pattern->width; i += pattern->width) {
    /* draw on the top and bottom */
    Sprite8(x + i, y,     pattern->height, pattern->pattern.split.pattern_h, LCD_MEM, SPRT_XOR);
    Sprite8(x + i, y + h, pattern->height, pattern->pattern.split.pattern_h, LCD_MEM, SPRT_XOR);
  }

  /* draw the left and right bars */
  for(uint8_t i = pattern->height; i <= h - pattern->height; i += pattern->height) {
    Sprite8(x,   y + i, pattern->height, pattern->pattern.split.pattern_v, LCD_MEM, SPRT_XOR);
    Sprite8(x+w, y + i, pattern->height, pattern->pattern.split.pattern_v, LCD_MEM, SPRT_XOR); 
  }
}
