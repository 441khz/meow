#define SAVE_SCREEN

#define USE_TI89

#include "kbd.h"
#include "menu.h"
#include <stdbool.h>
#include <tigcclib.h>

void SomethingStupid(void *pr) {
  char *pr_s = (char *)pr;
  char s[64];
  sprintf(s, "Stupid: %s              ", pr_s);
  DrawStr(0, 90, s, A_REVERSE);
}

// slow debounce routine. timer restart because we don't want to hit the falling edge of a debounce
#define SHITTY_DEBOUNCE()                                                                               \
  GKeyFlush(); /* prolly not needed */                                                                  \
  OSTimerRestart(USER_TIMER);                                                                           \
  while (!OSTimerExpired(USER_TIMER))                                                                   \
    ;

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

  /* step 1 - draw menu */
  #define MENU_X 40
  #define MENU_Y 10
  #define MENU_W 80
  #define MENU_HT 80
  DrawMenuBorder(&TEST_BORDER, MENU_X, MENU_Y, MENU_W, MENU_HT);

  // positions of the menu immediately at the border's inner edge.
  int menu_padx = MENU_X + TEST_BORDER.width;
  int menu_pady = MENU_Y + TEST_BORDER.height; // these should always be known before hand. would be nice
                                               // to make this constexpr somehow.

  FontSetSys(F_4x6);

  #define TEXT_PAD_X 2 // how many pixels to offset the text from the cursor (in the X direction)
  #define CURSOR_SZ 4  // this is just known for now. i dont think ill make it dynamic.

  // positions of the option cursor relative to the inside of the border frame
  #define OP1_CURSOR_X 10
  #define OP1_CURSOR_Y 5
  #define OP2_CURSOR_X 10
  #define OP2_CURSOR_Y 30
  #define OP3_CURSOR_X 10
  #define OP3_CURSOR_Y 50

  // greyscale requires you to work with multiple grey planes.
  // so black text requires setting both planes!
  for (uint8_t i = 0; i <= 1; i++) {
    SetPlane(i);
    DrawStr(menu_padx + OP1_CURSOR_X + CURSOR_SZ + TEXT_PAD_X, menu_pady + OP1_CURSOR_Y, "Option 1",
            A_NORMAL);
    DrawStr(menu_padx + OP2_CURSOR_X + CURSOR_SZ + TEXT_PAD_X, menu_pady + OP2_CURSOR_Y, "Option 2",
            A_NORMAL);
    DrawStr(menu_padx + OP3_CURSOR_X + CURSOR_SZ + TEXT_PAD_X, menu_pady + OP3_CURSOR_Y, "Option 3",
            A_NORMAL);
  }

  /* step 2 - set up handlers and menu cursors */
  menu_item_t item1 = {.cursor_x = MENU_X + OP1_CURSOR_X,
                       .cursor_y = MENU_Y + OP1_CURSOR_Y,
                       .callback = SomethingStupid,
                       .opaque = (void *)"opt1",
                       .jump.table = {MENU_NIL, 1, MENU_NIL, 1}};
  menu_item_t item2 = {.cursor_x = MENU_X + OP2_CURSOR_X,
                       .cursor_y = MENU_Y + OP2_CURSOR_Y,
                       .callback = SomethingStupid,
                       .opaque = (void *)"opt2",
                       .jump.table = {0, 2, 0, 2}};
  menu_item_t item3 = {.cursor_x = MENU_X + OP3_CURSOR_X,
                       .cursor_y = MENU_Y + OP3_CURSOR_Y,
                       .callback = SomethingStupid,
                       .opaque = (void *)"opt3",
                       .jump.table = {1, MENU_NIL, 1, MENU_NIL}};

  /** @todo the amount of indirection here is kinda concerning .... */
  menu_item_t menu_opts[3] = {item1, item2, item3};
  /** @todo warning about incomp. ptr types from .items. why? should be ok... */
  menu_t menu = (menu_t){.length = sizeof(menu_opts) / sizeof(menu_item_t), .items = &menu_opts};

  /* step 3 - the actual menu handler */
  menu_t *active_menu = &menu;
  uint8_t active_menu_item_idx = 0;
  menu_item_t *active_item = &(*(active_menu->items))[active_menu_item_idx];
  uint8_t candidate_jump_idx;
  char l[64];

  // set up 5ms debounce routine pinned to PRG/system timer interrupt
  OSFreeTimer(USER_TIMER);
  OSRegisterTimer(USER_TIMER, 5);

  GKeyFlush(); // sanity?
  DrawMenuCursor(active_item->cursor_x, active_item->cursor_y); // initial draw

  while (active_menu_item_idx != MENU_NIL) {

    if (_keytest(RR_UP)) {
      candidate_jump_idx = active_item->jump.idx.up;
    } else if (_keytest(RR_DOWN)) {
      candidate_jump_idx = active_item->jump.idx.down;
    } else if (_keytest(RR_LEFT)) {
      candidate_jump_idx = active_item->jump.idx.left;
    } else if (_keytest(RR_RIGHT)) {
      candidate_jump_idx = active_item->jump.idx.right;
    } else if (_keytest(RR_ESC)) {
      DrawStr(25, 25, "Press any key to exit.", A_REVERSE);
      GKeyFlush();
      ngetchx();
      GrayOff();
      OSFreeTimer(USER_TIMER);
      exit(0);
    } else if (_keytest(RR_2ND)) {
      active_item->callback(active_item->opaque);
      SHITTY_DEBOUNCE();
      continue;
    } else {
      GKeyFlush();
      continue; // if no keys pressed, just continue.
    }

    if (candidate_jump_idx != MENU_NIL && candidate_jump_idx < active_menu->length) {
      // remove the current cursor (since we are XOR)
      DrawMenuCursor(active_item->cursor_x, active_item->cursor_y);

      // set the new active item
      active_menu_item_idx = candidate_jump_idx;
      active_item = &(*(active_menu->items))[active_menu_item_idx];

      // draw the next
      DrawMenuCursor(active_item->cursor_x, active_item->cursor_y);

      // debug info
      sprintf(l, "Active: %d             ", active_menu_item_idx);
      DrawStr(MENU_X, MENU_Y, l, A_REVERSE);
    } else {
      DrawStr(MENU_X, MENU_Y, "Can't jump now!", A_REPLACE);
    }

    SHITTY_DEBOUNCE();
  }

  printf("Probably will never get called...\n");
  ngetchx();
  GrayOff();
}
