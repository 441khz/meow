#define SAVE_SCREEN

#define USE_TI89

#include "kbd.h"
#include "menu.h"
#include <stdbool.h>
#include <tigcclib.h>

void ExampleCallback(void *pr) {
  char *pr_s = (char *)pr;
  char s[64];
  sprintf(s, "Callbk: %s              ", pr_s);
  DrawStr(0, 90, s, A_REVERSE);
}

/* crude 'debounce' routine to slow polling time */
#define DEBOUNCE_WAIT()                                                                                 \
  for (int16_t i = 0; i < INT16_MAX; i++) {                                                             \
    asm("NOP");                                                                                         \
  }

/* commented and fully-featured demo of the menu system :) */
void _main(void) {

  border_pattern_t TEST_BORDER =
      (border_pattern_t){.pattern.split.pattern_h = {0xF0, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00},
                         .pattern.split.pattern_v = {0x90, 0x90, 0x90, 0x90, 0x00, 0x00, 0x00, 0x00},
                         .corners.split.corners_bl = {0xA0, 0xB0, 0x80, 0xF0, 0x00, 0x00, 0x00, 0x00},
                         .corners.split.corners_tr = {0xF0, 0x10, 0xD0, 0x50, 0x00, 0x00, 0x00, 0x00},
                         .corners.split.corners_tl = {0xF0, 0x80, 0xB0, 0xA0, 0x00, 0x00, 0x00, 0x00},
                         .corners.split.corners_br = {0x50, 0xD0, 0x10, 0xF0, 0x00, 0x00, 0x00, 0x00},
                         .width = 4,
                         .height = 4};

  GrayOn();
  ClrScr();

  /* disable Int5, hook Int1 to greyscale ONLY. (req'd to poll keyboard.) */
  INT_HANDLER vector_handle_int5;
  INT_HANDLER vector_handle_int1;
  vector_handle_int5 = GetIntVec(AUTO_INT_5);
  vector_handle_int1 = GetGrayInt1Handler();
  SetIntVec(AUTO_INT_5, DUMMY_HANDLER);
  GraySetInt1Handler(DUMMY_HANDLER);

/* step 1 - draw menu */
#define MENU_X 0
#define MENU_Y 20
#define MENU_W 160 - 4
#define MENU_HT 60
  DrawMenuBorder(&TEST_BORDER, MENU_X, MENU_Y, MENU_W, MENU_HT);

  // positions of the menu immediately at the border's inner edge.
  int menu_padx = MENU_X + TEST_BORDER.width;
  int menu_pady = MENU_Y + TEST_BORDER.height; // these should always be known before hand. would be nice
                                               // to make this constexpr somehow.

// how many pixels to offset the text from the cursor's rightmost edge (in the X direction)
#define CRSR_PAD_X 0
#define CURSOR_WIDTH 8

// positions of the option cursor relative to the inside of the border frame
#define OP0_CURSOR_X 5
#define OP0_CURSOR_Y 10
#define OP1_CURSOR_X 70
#define OP1_CURSOR_Y 10
#define OP2_CURSOR_X 70
#define OP2_CURSOR_Y 30

  FontSetSys(F_6x8);

  // greyscale requires you to work with multiple grey planes.
  // so black text requires setting both planes!
  for (uint8_t i = 0; i <= 1; i++) {
    SetPlane(i);
    DrawStr(0, 0, "2nd: Click, Esc: Exit", A_NORMAL);
    DrawStr(menu_padx + OP0_CURSOR_X + CURSOR_WIDTH + CRSR_PAD_X, menu_pady + OP0_CURSOR_Y, "Option 0",
            A_NORMAL);
    DrawStr(menu_padx + OP1_CURSOR_X + CURSOR_WIDTH + CRSR_PAD_X, menu_pady + OP1_CURSOR_Y, "Option 1",
            A_NORMAL);
    DrawStr(menu_padx + OP2_CURSOR_X + CURSOR_WIDTH + CRSR_PAD_X, menu_pady + OP2_CURSOR_Y, "Option 2",
            A_NORMAL);
  }

  FontSetSys(F_4x6);

  /* step 2 - set up handlers and menu cursors */
  menu_item_t item0 = {.cursor_x = menu_padx + OP0_CURSOR_X,
                       .cursor_y = menu_pady + OP0_CURSOR_Y,
                       .callback = ExampleCallback,
                       .opaque = (void *)"op0",
                       .jump.table = {MENU_NIL, MENU_NIL, MENU_NIL, 1}}; // {up,dn,lf,rg}
  menu_item_t item1 = {.cursor_x = menu_padx + OP1_CURSOR_X,
                       .cursor_y = menu_pady + OP1_CURSOR_Y,
                       .callback = ExampleCallback,
                       .opaque = (void *)"op1",
                       .jump.table = {MENU_NIL, 2, 0, MENU_NIL}};
  menu_item_t item2 = {.cursor_x = menu_padx + OP2_CURSOR_X,
                       .cursor_y = menu_pady + OP2_CURSOR_Y,
                       .callback = ExampleCallback,
                       .opaque = (void *)"op2",
                       .jump.table = {1, MENU_NIL, MENU_NIL, MENU_NIL}};

  menu_item_t menu_opts[] = {item0, item1, item2};

  /**
  option 0 <--> option 1
                ^
                |
                v
               option 2
*/

  menu_t menu = (menu_t){.length = sizeof(menu_opts) / sizeof(menu_item_t),
                         .items = (menu_item_t(*)[])(&menu_opts)};

  /* step 3 - the actual menu handler */
  menu_t *active_menu = &menu;
  uint8_t active_menu_item_idx = 0;
  menu_item_t *active_item = &(*(active_menu->items))[active_menu_item_idx];
  uint8_t candidate_jump_idx;
  char dbg_txt[64];

  GKeyFlush();

  DrawMenuCursor8(active_item->cursor_x, active_item->cursor_y); // initial draw

  while (active_menu_item_idx != MENU_NIL) {

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
      break;
    } else if (_keytest_optimized(RR_2ND)) {
      active_item->callback(active_item->opaque);
      DEBOUNCE_WAIT();
      continue;
    } else {
      continue; // if no keys pressed, just continue.
    }
    END_KEYTEST

    if (candidate_jump_idx != MENU_NIL && candidate_jump_idx < active_menu->length) {
      // remove the current cursor (since we are XOR)
      DrawMenuCursor8(active_item->cursor_x, active_item->cursor_y);

      // set the new active item
      active_menu_item_idx = candidate_jump_idx;
      active_item = &(*(active_menu->items))[active_menu_item_idx];

      // draw the next
      DrawMenuCursor8(active_item->cursor_x, active_item->cursor_y);

      // debug info
      sprintf(dbg_txt, "Active: %u             ", active_menu_item_idx);
      DrawStr(MENU_X, MENU_Y, dbg_txt, A_REVERSE);
    } else {
      DrawStr(MENU_X, MENU_Y, "Can't jump now!", A_REPLACE);
    }

    DEBOUNCE_WAIT();
  }

  /* restore ints and say goodbye */
  SetIntVec(AUTO_INT_5, vector_handle_int5);
  GraySetInt1Handler(vector_handle_int1);
  GrayOff();
  ClrScr();
  FontSetSys(F_8x10);
  DrawStr(0, 0, "Press any key", A_REVERSE);
  GKeyFlush();
  ngetchx();
}
