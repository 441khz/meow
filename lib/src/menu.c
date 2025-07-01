#include <tigcclib.h>

#include <meow/macros.h>
#include <meow/menu.h>
#include <meow/player.h>

/**
 * @brief Uses OS routines to draw a simple rectangular border. Doesn't draw background like
 * DrawMenuBorder.
 */
inline void DrawSimpleMenuBorder(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  WIN_RECT rect = {x, y, x + w, y + h};
  SetPlane(0);
  DrawClipRect(&rect, INLINE_NOCLIP, A_NORMAL);
  SetPlane(1);
  DrawClipRect(&rect, INLINE_NOCLIP, A_NORMAL);
}

/** @brief Draws a 4px menu cursor using an 8px draw routine in XOR. */
inline void DrawMenuCursor4(uint8_t x, uint8_t y, uint8_t attr) {
  uint8_t cur[] = {0xA0, 0xB0, 0x80, 0xF0, 0x00, 0x00, 0x00, 0x00};
  _Sprite8Mono(x, y, 4, cur, attr);
}

/** @brief Draws a big 8px menu cursor in XOR */
inline void DrawMenuCursor8(uint8_t x, uint8_t y, uint8_t attr) {
  uint8_t cur[] = {0x80, 0xC0, 0xE0, 0xF0, 0xF0, 0xE0, 0xC0, 0x80};
  _Sprite8Mono(x, y, 8, cur, attr);
}

/**
 * @brief Given a pattern, draw a menu border of specific size and shape. Draws a white background
 * behind it by OS routines. Will draw _AS CLOSE AS POSSIBLE_ to the shape specified.
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
 * @todo implement advanced routines for drawing strange boxes (non-multiple of 8 by using Sprite8 XOR
 * under/over-draw)
 */
void DrawMenuBorder(border_pattern_t *pattern, uint8_t x, uint8_t y, uint8_t w, uint8_t h) {

  _ASSERT(pattern);
  // for now, i see no reason to have super small boxes.
  // i'd have to do some very special underdraw routines to get it to fill <16 in a nice looking way.
  _ASSERT(w >= 2 * pattern->width);
  _ASSERT(w >= 2 * pattern->width);

  // (slow) clear routine
  SetPlane(DARK_PLANE);
  ScrRectFill(&(SCR_RECT){{x, y, x + w + pattern->width, y + h + pattern->height}}, INLINE_NOCLIP,
              A_REVERSE);
  SetPlane(LIGHT_PLANE);
  ScrRectFill(&(SCR_RECT){{x, y, x + w + pattern->width, y + h + pattern->height}}, INLINE_NOCLIP,
              A_REVERSE);

  // draw top left corner first
  _Sprite8Grey(x, y, pattern->height, pattern->corners.plane.dark.tl, pattern->corners.plane.light.tl,
               SPRT_XOR);

  /* draw the top and bottom bars */
  uint8_t i; // when loop finished, i should be rightmost drawn pixel yet.
  for (i = pattern->width; i <= w - pattern->width; i += pattern->width) {
    // top horiz bar
    _Sprite8Grey(x + i, y, pattern->height, pattern->pattern.plane.dark.horz_top,
                 pattern->pattern.plane.light.horz_top, SPRT_XOR);
  }

  // draw top right
  _Sprite8Grey(x + i, y, pattern->height, pattern->corners.plane.dark.tr,
               pattern->corners.plane.light.tr, SPRT_XOR);

  /* draw the left and right bars */
  uint8_t j; // when loop finished, j should be bottom most drawn pixel yet.
  for (j = pattern->height; j <= h - pattern->height; j += pattern->height) {
    _Sprite8Grey(x, y + j, pattern->height, pattern->pattern.plane.dark.vert_left,
                 pattern->pattern.plane.light.vert_left, SPRT_XOR);
    _Sprite8Grey(x + i, y + j, pattern->height, pattern->pattern.plane.dark.vert_right,
                 pattern->pattern.plane.light.vert_right, SPRT_XOR);
  }

  for (i = pattern->width; i <= w - pattern->width; i += pattern->width) {
    // top horiz bar
    _Sprite8Grey(x + i, y + j, pattern->height, pattern->pattern.plane.dark.horz_bot,
                 pattern->pattern.plane.light.horz_bot, SPRT_XOR);
  } // we can't group this with line ~135 because +j is required. i think we can precompute j though, but
    // it'd cost a modulo/div.

  // draw bottom left
  _Sprite8Grey(x, y + j, pattern->height, pattern->corners.plane.dark.bl,
               pattern->corners.plane.light.bl, SPRT_XOR);

  // draw bottom right
  _Sprite8Grey(x + i, y + j, pattern->height, pattern->corners.plane.dark.br,
               pattern->corners.plane.light.br, SPRT_XOR);
}

static menu_t *_MM_ActiveMenu;
static uint8_t _MM_InitialIdx;
static void (*_MM_CursorFunc)(uint8_t, uint8_t, uint8_t);

/**
 * @brief Resets the menu manager. Please call this when you're done with the menu manager.
 * Otherwise, stuff might be left over, especially if you have a lazy call to `StartMenuManager()`
 * without a `SetupMenuManager()`.
 */
inline void ResetMenuManager() { _MM_ActiveMenu = NULL; }

/**
 * @brief Sets up the menu manager for use. Must be run before StartMenuManager().
 * @param[menu] - The menu to be handled
 * @param[initial_idx] - Which menu option would you wish to begin on
 * @param[cursor_sz] - Either 4px or 8px (for now).
 * @todo - Maybe change cursor_sz to just a `void(*crsr_fn)(u8, u8,u8)` directly?
 * Not sure if it should really be the programmers job to keep passing crsr draw ptrs lol.
 * I don't think it'd be too costly to do though since Setup takes crsr_sz already and then
 * switches on it and sets `void(*_MM_CursorFunc)(u8,u8,u8)`.
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

/**
 * @brief Display and keyboard driver for the menu handling !!This is a blocking function!!
 * Will display cursor and handle keyboard actions to move the cursor according to menu information
 * set in SetupMenuManager. Ends when ResetMenuManager() is called or ESC is pressed.
 * Menus do NOT trivially nest.
 * @todo Actually draw backgrounds where the menu cursors will be drawn. We assume that's done for us.
 */
menu_item_t *StartMenuManager() {

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

  (_MM_CursorFunc)(active_item->cursor_x, active_item->cursor_y, SPRT_OR); // initial draw

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
      active_item = NULL;
      break;
    } else if (_keytest_optimized(RR_2ND)) {
      break;
    } else {
      continue;
    }
    END_KEYTEST

    if (candidate_jump_idx != MENU_NIL) {
      _ASSERT(candidate_jump_idx < _MM_ActiveMenu->length); // fault in the menu jump table. check length?

      /** @todo Stop assuming cursor size is 8, probably have to pass the size and not just func. */
      (_MM_CursorFunc)(active_item->cursor_x, active_item->cursor_y, SPRT_XOR);

      // set the new active item
      active_menu_item_idx = candidate_jump_idx;
      active_item = &(*(_MM_ActiveMenu->items))[active_menu_item_idx];

      // draw the next
      (_MM_CursorFunc)(active_item->cursor_x, active_item->cursor_y, SPRT_OR);
    }

    DEBOUNCE_WAIT();
  }

  DEBOUNCE_WAIT();

  /* restore ints. may not be necessary depending on how this engine turns out */
  SetIntVec(AUTO_INT_5, vector_handle_int5);
  GraySetInt1Handler(vector_handle_int1);
  /** @todo should we reset menu manager here or should the programmer ? */

  return active_item;
}

// this can cause interrupt 5/auto-int 5/int5 bugs, we do NOT tailcall prev vector!
DEFINE_INT_HANDLER(_ToggleCursorBlink) { DrawMenuCursor8(160 - 8 - 4, 100 - 16, SPRT_XOR); }

/**
 * @brief Basic routine to display text messages. Everything here's predetermined, and it uses the menu
 * manager system. Good example if you want to learn how to use MenuManager.
 * @todo Buggy routine. Int5 really shouldn't be used during keytest. Do we really have to busy loop
 * here? Good enough for now.
 * @todo unfinished... 6/19/2025
 */

void DisplayStrTextBox(char str[]) {
#define BOX_X 0
/** @todo: the graphics routines are a little buggy. 68 seems wrong (68+(35-4) = 99, not 100). */
#define BOX_Y 68
#define BOX_W 160 - (Player.border->width)
#define BOX_HT 35 - (Player.border->height)

  /**
   * we will use the OS timer routines. rr/kt will be slightly upset.
   * @todo unfortunately, I don't care enough to fix this right now...
   * will break sound engine and potentially RR unless we redirect to DH?
   */
  OSFreeTimer(1);
  OSRegisterTimer(1, 1);
  INT_HANDLER vector_handle_int5 = GetIntVec(AUTO_INT_5);
  uint16_t prg_prev = PRG_getRate();
  // SetIntVec(AUTO_INT_5, _ToggleCursorBlink);

  // DrawMenuBorder(Player.border, BOX_X, BOX_Y, BOX_W, BOX_HT);
/* Text writing animation, based on the timer */
#define FONT_WIDTH 6
#define FONT_FAMILY F_6x8

/** @todo there's gotta be a global for this somewhere lol */
#define SCREEN_WIDTH 160 - (Player.border->width)
#define PAD_X (Player.border->width)
#define PAD_Y (Player.border->height) + 2 // because i said so
  uint8_t letters_can_fit = (SCREEN_WIDTH - PAD_X) / FONT_WIDTH;
  char *p = &str[0];

  while (*p != '\0') {

    SetIntVec(AUTO_INT_5, vector_handle_int5);

    DrawMenuBorder(Player.border, BOX_X, BOX_Y, BOX_W, BOX_HT);
    PRG_setRate(prg_prev);

    // printf("lcf: %hu", letters_can_fit);
    for (uint8_t i = 0; i < letters_can_fit; i++) {
      if (*p == '\0')
        break;
      DrawCharMono(BOX_X + PAD_X + (i * FONT_WIDTH), BOX_Y + PAD_Y, *p, SPRT_OR);
      p++; // not inling (*p++) since the macro expands!
      while (!OSTimerExpired(1))
        ; // delay :)
    }

    PRG_setRate(2);
    SetIntVec(AUTO_INT_5, _ToggleCursorBlink); // display blink by changing #5 interrupt vector (lol).

    /* Wait for key press */
    while (!_keytest(RR_2ND))
      ;
  }
  SetIntVec(AUTO_INT_5, vector_handle_int5);
  PRG_setRate(prg_prev);
  DEBOUNCE_WAIT();
}
