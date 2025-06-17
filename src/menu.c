#include <tigcclib.h>

#include "menu.h"
#include "gray.h"

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

inline void DrawSimpleMenuBorder(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  WIN_RECT rect = {x, y, x+w, y+h};
  SetPlane(0);
  DrawClipRect (&(WIN_RECT){x, y, x+w, y+h}, &(SCR_RECT){0, 0, 160, 100}, A_NORMAL);
  SetPlane(1);
  DrawClipRect (&(WIN_RECT){x, y, x+w, y+h}, &(SCR_RECT){0, 0, 160, 100}, A_NORMAL);
}

/**
  BIG TODO---
  implement double buffering.
  unfortunately, while drawing a cursor is simple, restoring the background is not.
  its likely that we'll have to draw the cursor on some higher "layer", and then
  blit the two layers together so can we easily redraw the background by restoring the original layer.
  I'm putting this under cursor because cursors can easily screw with the screen causing little 8x8
  empty cursor spots to be left where they once lay....
  Obviously once the menu is closed the game will probably ask to redraw, and then you're fine...
  but still, even the menus themselves are prone to this unless the menu functions are asked to be redrawn,
  which could be done by timer interrupt but is needlessly complicated and limits the framerate.
  
  I think we should just do double buffering instead.
  .... we could just AND the screen though. lol. probably will do that first.
*/
inline void DrawMenuCursor(uint8_t x, uint8_t y) {
  Sprite8(x, y, 4, (uint8_t []){0xA0,0xB0,0x80,0xF0, 0x00, 0x00, 0x00, 0x00}, GetPlane(DARK_PLANE), SPRT_XOR);
}

/**
 * @brief Given a pattern, draw a menu border of specific size and shape.
 * @param[pattern] The pattern to draw
 * optimization potential ...
 * computing the mtx transpose just in time would save a little memory if we have many borders.
 * maybe we could draw big boxes with sprite16/32, somehow memcpying and then drawing with sprite16/32
 * not sure if it would be much faster, id have to benchmark it.
 * 
 * NOTE ... As it is now, the first pixel of the first block is drawn at x, and the last pixel is at x+w+pattern->width.
 *      ... WLOG for y/height.
 * TODO ... rename DrawMenuBox? or maybe we separate bkgd drawing...? we'll see.
 * TODO ... specify box draw parameters, reverse bkgd transparent, etc.
 */
void DrawMenuBorder(border_pattern_t *pattern, uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  if(!pattern) {
     assert(0);
     return;
  }

  /* 
    small bug in the clear routine. x + w doesnt necessarily mean the end of the box.
    im not sure if its overrun to say x+w+pat_width or if thats perfect for all cases.
    either way this works, and we have vmem to overrun anyway if its buggy... lol.
  */

  /* (slow) clear routine */
  SetPlane (DARK_PLANE);
  ScrRectFill (&(SCR_RECT){{x,y,x+w+pattern->width,y+h}}, &(SCR_RECT){{0, 0,160, 100}}, A_REVERSE);
  SetPlane (LIGHT_PLANE);
  ScrRectFill (&(SCR_RECT){{x,y,x+w+pattern->width,y+h}}, &(SCR_RECT){{0, 0,160, 100}}, A_REVERSE);

  /* draw corners first */
  Sprite8(x, y,         pattern->height, pattern->corners.split.corners_tl, GetPlane(DARK_PLANE), SPRT_XOR); // top left
  Sprite8(x + w, y,     pattern->height, pattern->corners.split.corners_tr, GetPlane(DARK_PLANE), SPRT_XOR); // top right
  Sprite8(x, y + h,     pattern->height, pattern->corners.split.corners_bl, GetPlane(DARK_PLANE), SPRT_XOR); // bot left
  Sprite8(x + w, y + h, pattern->height, pattern->corners.split.corners_br, GetPlane(DARK_PLANE), SPRT_XOR); // bot right

  /* draw the top and bottom bars */
  for(uint8_t i = pattern->width; i <= w - pattern->width; i += pattern->width) {
    Sprite8(x + i, y,     pattern->height, pattern->pattern.split.pattern_h, GetPlane(DARK_PLANE), SPRT_XOR);
    Sprite8(x + i, y + h, pattern->height, pattern->pattern.split.pattern_h, GetPlane(DARK_PLANE), SPRT_XOR);
  }

  /* draw the left and right bars */
  for(uint8_t i = pattern->height; i <= h - pattern->height; i += pattern->height) {
    Sprite8(x,   y + i, pattern->height, pattern->pattern.split.pattern_v, GetPlane(DARK_PLANE), SPRT_XOR);
    Sprite8(x+w, y + i, pattern->height, pattern->pattern.split.pattern_v, GetPlane(DARK_PLANE), SPRT_XOR); 
  }

  /*
    also, TODO: this will only draw on one plane (for the sake of speed)
    but what if we want greyscale borders (2x the memory womp womp) or if
    we just want dark borders? (gotta run the whole thing again with GetPlane(LIGHT_PLANE))!
  */

}
