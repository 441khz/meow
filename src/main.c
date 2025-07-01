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

/**
 * @todo Layout the border structs in a neat, packed way so that we can initialize
 * them with a strange cast (a la map_t) at the top-level [straight array].
 */
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
  Player.border = &BORDER_PKMN;
}

void DemoDrawVfRect(short attr) {
  WIN_RECT c = (WIN_RECT){.x0 = Player.vf.draw_pos.x,
                          .y0 = Player.vf.draw_pos.y,
                          .x1 = Player.vf.draw_pos.x + (16 * Player.vf.width),
                          .y1 = Player.vf.draw_pos.y + (16 * Player.vf.height)};
  DrawClipRect(&c, INLINE_NOCLIP, attr);
}
void DemoMovePl() { /* tbd */ }

void DemoScrlVf() {

  /* lol */
  void ScrlVfClear() {
    ClrScrGrey();
    DrawMap(&Player.vf);
    DrawMenuBorder(Player.border, 0, 76, 90, 20 - 4);
    DrawStrMono(10, 85, "VF Scroll", A_NORMAL);
  }

  ScrlVfClear();
  while (!_keytest(RR_2ND)) {

    if (_keytest(RR_LEFT)) {
      Player.vf.tile_pos.x -= 1;
    } else if (_keytest(RR_RIGHT)) {
      Player.vf.tile_pos.x += 1;
    } else if (_keytest(RR_UP)) {
      Player.vf.tile_pos.y -= 1;
    } else if (_keytest(RR_DOWN)) {
      Player.vf.tile_pos.y += 1;
    } else {
      continue;
    }

    ScrlVfClear();
    DEBOUNCE_WAIT();
  }
}

void DemoGrowVf() {
  ClrScrGrey();
  DrawMap(&Player.vf);
  DrawMenuBorder(Player.border, 0, 76, 90, 20 - 4);
  DrawStrMono(10, 85, "Set VF Dims ", A_NORMAL);

  DemoDrawVfRect(A_NORMAL);
  while (!_keytest(RR_2ND)) {

    if (_keytest(RR_LEFT)) {
      DemoDrawVfRect(A_REVERSE);
      Player.vf.width -= 1;
    } else if (_keytest(RR_RIGHT)) {
      DemoDrawVfRect(A_REVERSE);
      Player.vf.width += 1;
    } else if (_keytest(RR_UP)) {
      DemoDrawVfRect(A_REVERSE);
      Player.vf.height -= 1;
    } else if (_keytest(RR_DOWN)) {
      DemoDrawVfRect(A_REVERSE);
      Player.vf.height += 1;
    } else {
      continue;
    }

    DemoDrawVfRect(A_NORMAL);
    DEBOUNCE_WAIT();
  }
}

void DemoMoveVf() {

  ClrScrGrey();
  // DrawMap(&Player.vf);
  DrawMenuBorder(Player.border, 0, 76, 90, 20 - 4);
  DrawStrMono(10, 85, "Set VF Pos ", A_NORMAL);

  DemoDrawVfRect(A_NORMAL);
  while (!_keytest(RR_2ND)) {

    if (_keytest(RR_LEFT)) {
      DemoDrawVfRect(A_REVERSE);
      Player.vf.draw_pos.x -= 4;
    } else if (_keytest(RR_RIGHT)) {
      DemoDrawVfRect(A_REVERSE);
      Player.vf.draw_pos.x += 4;
    } else if (_keytest(RR_UP)) {
      DemoDrawVfRect(A_REVERSE);
      Player.vf.draw_pos.y -= 4;
    } else if (_keytest(RR_DOWN)) {
      DemoDrawVfRect(A_REVERSE);
      Player.vf.draw_pos.y += 4;
    } else {
      continue;
    }

    DemoDrawVfRect(A_NORMAL);
    DEBOUNCE_WAIT();
  }
}

static menu_item_t ITEMS[4] = {

    {.cursor_x = 10,
     .cursor_y = 85,
     .callback = DemoMoveVf,
     .jump.table = {MENU_NIL, MENU_NIL, MENU_NIL, 1}},
    {.cursor_x = 50, .cursor_y = 85, .callback = DemoGrowVf, .jump.table = {MENU_NIL, MENU_NIL, 0, 2}},
    {.cursor_x = 90, .cursor_y = 85, .callback = DemoScrlVf, .jump.table = {MENU_NIL, MENU_NIL, 1, 3}},
    {.cursor_x = 130,
     .cursor_y = 85,
     .callback = DemoMovePl,
     .jump.table = {MENU_NIL, MENU_NIL, 2, MENU_NIL}}

};

static menu_t const BOTTOM_BAR = {.length = 4, .items = (menu_item_t(*)[])(&ITEMS)};

void _main(void) {

  GameInitializePlayerAndBorders();
  GrayOn();

  Player.vf = (vf_t){.draw_pos = (vec2_draw_t){.x = 0, .y = 0},
                     .tile_pos = (vec2_tile_t){.x = 3, .y = 2},
                     .height = 4,
                     .width = 4};

  Player.pos = (vec2_tile_t){.x = 3, .y = 5};

  while (1) {

    /* step 0 - reset mm */
    ResetMenuManager();
    ClrScrGrey();

    DrawMap(&Player.vf);

    SetupMenuManager((menu_t *)&BOTTOM_BAR, 0, 8);
    DrawMenuBorder(Player.border, 0, 76, 160 - 4, 20 - 4);
    DrawStrMono(18, 85, "Move", A_NORMAL);
    DrawStrMono(58, 85, "Grow", A_NORMAL);
    DrawStrMono(98, 85, "Scrl", A_NORMAL);
    DrawStrMono(138, 85, "P", A_NORMAL);

    menu_item_t *p = StartMenuManager();
    if (p) {
      if (p->callback) {
        p->callback(p->opaque);
      }
    } else {
      break;
    }

    DEBOUNCE_WAIT();
  }

  GrayOff();
  ClrScr();
}
