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
  SetupMenuManager(&menu, 0, 8);
  StartMenuManager();

  /* cleanup and say goodbye */
  ResetMenuManager();
  ClrScr();
  GrayOff();
}
