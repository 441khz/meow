#define SAVE_SCREEN

#define USE_TI89

#include <stdbool.h>
#include <tigcclib.h>

#include <meow/macros.h>
#include <meow/map.h>
#include <meow/menu.h>
#include <meow/player.h>

player_t Player;
border_pattern_t BORDER_DEFAULT;
border_pattern_t BORDER_PKMN;

/* step 1 - draw menu */
#define MENU_X 0
#define MENU_Y 0
#define MENU_W 100
#define MENU_HT 60

// how many pixels to offset the text from the cursor's rightmost edge (in the X direction)
#define CRSR_PAD_X 0
#define CURSOR_WIDTH 8

// positions of the option cursor relative to the inside of the border frame
#define OP0_CURSOR_X 4
#define OP0_CURSOR_Y 10
#define OP1_CURSOR_X 4
#define OP1_CURSOR_Y 25

// lazy
#define MENU_PADX MENU_X + 8
#define MENU_PADY MENU_Y + 8

void GameDrawMainMenu() {

  ClrScrGrey();

  // positions of the menu immediately at the border's inner edge.
  int menu_padx = MENU_PADX; // MENU_X + Player.border->width;
  int menu_pady = MENU_PADY; // MENU_Y + Player.border->height;

  FontSetSys(F_6x8);

  DrawMenuBorder(Player.border, MENU_X, MENU_Y, MENU_W, MENU_HT);

  char u[64];
  FontSetSys(F_6x8);
  sprintf(u, "Border%d   ", Player.border_number);

  DrawStrMono(menu_padx + OP0_CURSOR_X + CURSOR_WIDTH + CRSR_PAD_X, menu_pady + OP0_CURSOR_Y, "New Game",
              A_NORMAL);
  DrawStrMono(menu_padx + OP1_CURSOR_X + CURSOR_WIDTH + CRSR_PAD_X, menu_pady + OP1_CURSOR_Y, u,
              A_NORMAL);
}

#define NUM_BORDERS 2
void GameMainMenuToggleBorder() {

  if (Player.border_number >= NUM_BORDERS - 1) {
    Player.border_number = 0;
  } else {
    Player.border_number++;
  }
  switch (Player.border_number) {
  case 0:
    Player.border = &BORDER_DEFAULT;
    break;
  case 1:
    Player.border = &BORDER_PKMN;
    break;
  default:
    _STOP();
    break;
  }

  GameDrawMainMenu();
}

void GameInitializePlayerAndBorders() {
  BORDER_DEFAULT = (border_pattern_t){
      .pattern.plane.light.horz_top = {0xF0, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00},
      .pattern.plane.light.vert_left = {0x90, 0x90, 0x90, 0x90, 0x00, 0x00, 0x00, 0x00},
      .pattern.plane.light.horz_bot = {0xF0, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00},
      .pattern.plane.light.vert_right = {0x90, 0x90, 0x90, 0x90, 0x00, 0x00, 0x00, 0x00},
      .corners.plane.light.bl = {0xA0, 0xB0, 0x80, 0xF0, 0x00, 0x00, 0x00, 0x00},
      .corners.plane.light.tr = {0xF0, 0x10, 0xD0, 0x50, 0x00, 0x00, 0x00, 0x00},
      .corners.plane.light.tl = {0xF0, 0x80, 0xB0, 0xA0, 0x00, 0x00, 0x00, 0x00},
      .corners.plane.light.br = {0x50, 0xD0, 0x10, 0xF0, 0x00, 0x00, 0x00, 0x00},
      .pattern.plane.dark.horz_top = {0xF0, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00},
      .pattern.plane.dark.vert_left = {0x90, 0x90, 0x90, 0x90, 0x00, 0x00, 0x00, 0x00},
      .pattern.plane.dark.horz_bot = {0xF0, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00},
      .pattern.plane.dark.vert_right = {0x90, 0x90, 0x90, 0x90, 0x00, 0x00, 0x00, 0x00},
      .corners.plane.dark.bl = {0xA0, 0xB0, 0x80, 0xF0, 0x00, 0x00, 0x00, 0x00},
      .corners.plane.dark.tr = {0xF0, 0x10, 0xD0, 0x50, 0x00, 0x00, 0x00, 0x00},
      .corners.plane.dark.tl = {0xF0, 0x80, 0xB0, 0xA0, 0x00, 0x00, 0x00, 0x00},
      .corners.plane.dark.br = {0x50, 0xD0, 0x10, 0xF0, 0x00, 0x00, 0x00, 0x00},
      .width = 4,
      .height = 4};
  BORDER_PKMN = (border_pattern_t){
      .corners.plane.dark.bl = {0x14, 0x04, 0x1A, 0x2D, 0x7E, 0x42, 0x25, 0x18},
      .corners.plane.light.bl = {0x14, 0x04, 0x1A, 0x35, 0x42, 0x42, 0x25, 0x18},
      .corners.plane.dark.br = {0x28, 0x20, 0x58, 0xAC, 0x7E, 0x42, 0xA4, 0x18},
      .corners.plane.light.br = {0x28, 0x20, 0x58, 0xB4, 0x42, 0x42, 0xA4, 0x18},
      .pattern.plane.dark.horz_top = {0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00},
      .pattern.plane.light.horz_top = {0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00},
      .corners.plane.dark.tl = {0x18, 0x2D, 0x7E, 0x42, 0x25, 0x1A, 0x04, 0x14},
      .corners.plane.light.tl = {0x18, 0x35, 0x42, 0x42, 0x25, 0x1A, 0x04, 0x14},
      .corners.plane.dark.tr = {0x18, 0xAC, 0x7E, 0x42, 0xA4, 0x58, 0x20, 0x28},
      .corners.plane.light.tr = {0x18, 0xB4, 0x42, 0x42, 0xA4, 0x58, 0x20, 0x28},
      .pattern.plane.dark.vert_left = {0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14},
      .pattern.plane.light.vert_left = {0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14},
      .pattern.plane.dark.vert_right = {0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28},
      .pattern.plane.light.vert_right = {0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28},
      .pattern.plane.dark.horz_bot = {0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00},
      .pattern.plane.light.horz_bot = {0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00},
      .width = 8,
      .height = 8};
  memset(&Player, 0, sizeof(player_t));
  Player.border = &BORDER_DEFAULT;
}

void _main(void) {

  GameInitializePlayerAndBorders();
  ClrScr();
  GrayOn();

  /* step 0 - reset mm */
  ResetMenuManager();
  ClrScrGrey();

  GameDrawMainMenu();

  // positions of the menu immediately at the border's inner edge.
  // int menu_padx = MENU_X + Player.border->width;
  // int menu_pady = MENU_Y + Player.border->height;

  /* step 1 - set up handlers and menu cursors */
  menu_item_t item0 = {.cursor_x = MENU_PADX + OP0_CURSOR_X,
                       .cursor_y = MENU_PADY + OP0_CURSOR_Y,
                       .callback = (void (*)(void *))DrawMap,
                       .jump.table = {MENU_NIL, 1, MENU_NIL, MENU_NIL}};
  menu_item_t item1 = {.cursor_x = MENU_PADX + OP1_CURSOR_X,
                       .cursor_y = MENU_PADY + OP1_CURSOR_Y,
                       .callback = GameMainMenuToggleBorder,
                       .jump.table = {0, MENU_NIL, MENU_NIL, MENU_NIL}};

  menu_item_t menu_opts[] = {item0, item1};

  menu_t menu = (menu_t){.length = sizeof(menu_opts) / sizeof(menu_item_t),
                         .items = (menu_item_t (*)[])(&menu_opts)};

  SetupMenuManager(&menu, 0, CURSOR_WIDTH);

  menu_item_t *opt = StartMenuManager();
  if (opt && opt->callback) {
    opt->callback(opt->opaque);
  }
  // DEBOUNCE_WAIT();
  ngetchx();
  GrayOff();
  ClrScr();
}
