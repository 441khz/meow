#include <tigcclib.h>

#include "gray.h"
#include "menu.h"

// shitty hack for clipping against the size of the ti89 screen.
#define INLINE_NOCLIP                                                                                   \
  &(SCR_RECT) {{0, 0, 160, 100}}

/**
 * @brief Uses OS routines to draw a simple rectangular border. Doesn't draw background like
 * DrawMenuBorder.
 */
inline void DrawSimpleMenuBorder(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  WIN_RECT rect = {x, y, x + w, y + h};
  SetPlane(0);
  DrawClipRect(&(WIN_RECT){x, y, x + w, y + h}, INLINE_NOCLIP, A_NORMAL);
  SetPlane(1);
  DrawClipRect(&(WIN_RECT){x, y, x + w, y + h}, INLINE_NOCLIP, A_NORMAL);
}

/**
 * @brief Draws a menu cursor. The look is hardcoded for now and drawn as `XOR` on `DARK_PLANE` only
 * (not both!)
 */
inline void DrawMenuCursor(uint8_t x, uint8_t y) {
  Sprite8(x, y, 4, (uint8_t[]){0xA0, 0xB0, 0x80, 0xF0, 0x00, 0x00, 0x00, 0x00}, GetPlane(DARK_PLANE),
          SPRT_XOR);
}

/**
 * @brief Given a pattern, draw a menu border of specific size and shape. Draws a white background
 * behind it by OS routines.
 * @param[pattern] The pattern to draw
 * @note optimization potential ...
 * computing the mtx transpose just in time would save a little memory if we
 * have many borders. maybe we could draw big boxes with `Sprite16`/`32`, somehow
 * memcpying and then drawing with `Sprite16`/`32` not sure if it would be much
 * faster, id have to benchmark it.
 * @note As it is now, the first pixel of the first block is drawn at x, and
 * the last pixel is at `x+w+pattern->width`.
 * The argument holds without loss of generality for y/height.
 * @todo rename to `DrawMenuBox`? or maybe we separate bkgd drawing...? we'll see.
 * @todo specify box draw parameters, reverse bkgd transparent, etc.
 */
void DrawMenuBorder(border_pattern_t *pattern, uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  if (!pattern) {
    assert(0);
    return;
  }

  /* (slow) clear routine */
  SetPlane(DARK_PLANE);
  ScrRectFill(&(SCR_RECT){{x, y, x + w + pattern->width, y + h}}, INLINE_NOCLIP, A_REVERSE);
  SetPlane(LIGHT_PLANE);
  ScrRectFill(&(SCR_RECT){{x, y, x + w + pattern->width, y + h}}, INLINE_NOCLIP, A_REVERSE);

  /* draw corners first */
  Sprite8(x, y, pattern->height, pattern->corners.split.corners_tl, GetPlane(DARK_PLANE), SPRT_XOR);
  Sprite8(x + w, y, pattern->height, pattern->corners.split.corners_tr, GetPlane(DARK_PLANE), SPRT_XOR);
  Sprite8(x, y + h, pattern->height, pattern->corners.split.corners_bl, GetPlane(DARK_PLANE), SPRT_XOR);
  Sprite8(x + w, y + h, pattern->height, pattern->corners.split.corners_br, GetPlane(DARK_PLANE),
          SPRT_XOR);

  /* draw the top and bottom bars */
  for (uint8_t i = pattern->width; i <= w - pattern->width; i += pattern->width) {
    Sprite8(x + i, y, pattern->height, pattern->pattern.split.pattern_h, GetPlane(DARK_PLANE), SPRT_XOR);
    Sprite8(x + i, y + h, pattern->height, pattern->pattern.split.pattern_h, GetPlane(DARK_PLANE),
            SPRT_XOR);
  }

  /* draw the left and right bars */
  for (uint8_t i = pattern->height; i <= h - pattern->height; i += pattern->height) {
    Sprite8(x, y + i, pattern->height, pattern->pattern.split.pattern_v, GetPlane(DARK_PLANE), SPRT_XOR);
    Sprite8(x + w, y + i, pattern->height, pattern->pattern.split.pattern_v, GetPlane(DARK_PLANE),
            SPRT_XOR);
  }

  /**
    @todo also, TODO: this will only draw on one plane (for the sake of speed)
    but what if we want greyscale borders (2x the memory womp womp) or if
    we just want dark borders? (gotta run the whole thing again with
    `GetPlane(LIGHT_PLANE)`)!
  */
}
