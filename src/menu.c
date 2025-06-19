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

/** @brief Draws a 4px menu cursor on plane 1 using an 8px draw routine in XOR. */
inline void DrawMenuCursor4(uint8_t x, uint8_t y) {
  Sprite8(x, y, 4, (uint8_t[]){0xA0, 0xB0, 0x80, 0xF0, 0x00, 0x00, 0x00, 0x00}, GetPlane(DARK_PLANE),
          SPRT_XOR);
}

/** @brief Draws a big 8px menu cursor on plane 1 in XOR */
inline void DrawMenuCursor8(uint8_t x, uint8_t y) {
  Sprite8(x, y, 8, (uint8_t[]){0x80, 0xC0, 0xE0, 0xF0, 0xF0, 0xE0, 0xC0, 0x80}, GetPlane(DARK_PLANE),
          SPRT_XOR);
}

/**
 * @todo better error handling for debug
 */
#ifndef NDEBUG
#define _STOP()                                                                                         \
  {                                                                                                     \
    GrayOff();                                                                                          \
    FontSetSys(F_8x10);                                                                                 \
    MoveTo(0, 0);                                                                                       \
    printf("STOP!\n%s\nLine %d       ", __func__, __LINE__);                                            \
    while (1)                                                                                           \
      ;                                                                                                 \
  }
#define _ASSERT(cond)                                                                                   \
  {                                                                                                     \
    if (!(cond)) {                                                                                      \
      _STOP();                                                                                          \
    }                                                                                                   \
  }
#else
#define _STOP(code) ((void)0)
#define _ASSERT(cond) ((void)0)
#endif

/**
 * @brief Given a pattern, draw a menu border of specific size and shape. Draws a white background
 * behind it by OS routines.
 * @param[pattern] The pattern to draw
 * @param[x] - x coordinate to draw the top left corner
 * @param[y] - y coordinate to draw the top left corner
 * @param[w] - width of the box, NOT inclusive of the border's size.
 * e.g: Let `pattern` be some 8px border pattern. If `w = 100`, we will stop drawing at pixel 108.
 * @param[h] - height of the box, NOT inclusive of the border's size (see `w`).
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

  _ASSERT(pattern);

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

/**
 * @note I'm really not too sure if we should keep these static or if we should pass the storage of menu
 * manager information off to the programmer, and change the prototypes to be like
 * SetupMenuManager(menu_manager_t *mm, menu_t *menu); For now, I'm keeping this static. I don't see why
 * we'd have >1 menu manager at a time, and space is a concern.
 */

static menu_t *_MM_ActiveMenu;
static uint8_t _MM_InitialIdx;
static void (*_MM_CursorFunc)(uint8_t, uint8_t);

/**
 * @brief Resets the menu manager. Please call this when you're done with the menu manager.
 * Otherwise, stuff might be left over, especially if you have a lazy call to `StartMenuManager()` without a `SetupMenuManager()`.
*/
inline void ResetMenuManager() { _MM_ActiveMenu = NULL; }

/**
 * @brief Sets up the menu manager for use. Must be run before StartMenuManager().
 * @param[menu] - The menu to be handled
 * @param[initial_idx] - Which menu option would you wish to begin on
 * @param[cursor_sz] - Either 4px or 8px (for now).
 * @todo - Maybe change cursor_sz to just a `void(*crsr_fn)(u8, u8)` directly?
 * Not sure if it should really be the programmers job to keep passing crsr draw ptrs lol.
 * I don't think it'd be too costly to do though since Setup takes crsr_sz already and then
 * switches on it and sets `void(*_MM_CursorFunc)(u8,u8)`.
 * @todo - Custom cursors without function ptrs? Obviously with its easy, but... maybe a `cursor_t`..?
 * Seems like bloat.
 */
void SetupMenuManager(menu_t *menu, uint8_t initial_idx, uint8_t cursor_sz) {

  _ASSERT(menu);
  _ASSERT(initial_idx < menu->length);

  _MM_ActiveMenu = menu;
  _MM_InitialIdx = initial_idx;
  switch (cursor_sz) {
  case 4:
    _MM_CursorFunc = DrawMenuCursor4;
    break;
  case 8:
    _MM_CursorFunc = DrawMenuCursor8;
    break;
  default:
    _STOP();
    break;
  }
}

/** @brief crude 'debounce' routine to slow polling time */
#define DEBOUNCE_WAIT()                                                                                 \
  for (int16_t i = 0; i < INT16_MAX; i++) {                                                             \
    asm("NOP");                                                                                         \
  }

/** 
 * @brief Display driver for the menu manager. !!This is a blocking function!!
 * Will display cursor and handle keyboard actions to move the cursor according to menu information
 * set in SetupMenuManager. Ends when ResetMenuManager() is called or ESC is pressed.
*/
void StartMenuManager() {

  _ASSERT(_MM_ActiveMenu);
  _ASSERT(_MM_CursorFunc);
  _ASSERT(GrayOn()); // otherwise the interrupt setup below will break badly

  /* disable Int5, hook Int1 to greyscale ONLY. (req'd to poll keyboard.) */
  INT_HANDLER vector_handle_int5 = GetIntVec(AUTO_INT_5);
  INT_HANDLER vector_handle_int1 = GetGrayInt1Handler();
  SetIntVec(AUTO_INT_5, DUMMY_HANDLER);
  GraySetInt1Handler(DUMMY_HANDLER);

  uint8_t active_menu_item_idx = _MM_InitialIdx;
  menu_item_t *active_item = &(*(_MM_ActiveMenu->items))[active_menu_item_idx];
  uint8_t candidate_jump_idx;

#ifndef NDEBUG
  char dbg_txt[64];
#endif

  (_MM_CursorFunc)(active_item->cursor_x, active_item->cursor_y); // initial draw

  while (_MM_ActiveMenu) {

    BEGIN_KEYTEST
    if (_keytest_optimized(RR_UP)) {
      candidate_jump_idx = active_item->jump.idx.up;
    } else if (_keytest_optimized(RR_DOWN)) {
      candidate_jump_idx = active_item->jump.idx.down;
    } else if (_keytest_optimized(RR_LEFT)) {
      candidate_jump_idx = active_item->jump.idx.left;
    } else if (_keytest_optimized(RR_RIGHT)) {
      candidate_jump_idx = active_item->jump.idx.right;
    } else if (_keytest_optimized(RR_ESC)) {
      /** @todo nested menu support? */
      // _MM_ActiveMenu = prev_nested_menu;
      // might go with the idea of dtors i had earlier.
      /** @todo also, should we unset _MM_ActiveMenu here? */
      break;
    } else if (_keytest_optimized(RR_2ND)) {
      active_item->callback(active_item->opaque);
      DEBOUNCE_WAIT();
      continue; // should be safe if callback calls ResetMM() since loop guard dies.
    } else {
      continue; // if no keys pressed, just continue.
    }
    END_KEYTEST

    if (candidate_jump_idx != MENU_NIL && candidate_jump_idx < _MM_ActiveMenu->length) {
      // remove the current cursor (since we are XOR)
      (_MM_CursorFunc)(active_item->cursor_x, active_item->cursor_y);

      // set the new active item
      active_menu_item_idx = candidate_jump_idx;
      active_item = &(*(_MM_ActiveMenu->items))[active_menu_item_idx];

      // draw the next
      (_MM_CursorFunc)(active_item->cursor_x, active_item->cursor_y);

#ifndef NDEBUG
      // debug info
      sprintf(dbg_txt, "Active: %u             ", active_menu_item_idx);
      DrawStr(0, 15, dbg_txt, A_REVERSE);
#endif
    }

    DEBOUNCE_WAIT();
  }

  /* restore ints. may not be necessary depending on how this engine turns out */
  SetIntVec(AUTO_INT_5, vector_handle_int5);
  GraySetInt1Handler(vector_handle_int1);
  /** @todo should we reset menu manager here or should the programmer ? */
}
