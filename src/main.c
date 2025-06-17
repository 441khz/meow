#define SAVE_SCREEN

#define USE_TI89

#include <tigcclib.h>
#include <stdbool.h>
#include "menu.h"
#include "kbd.h"

void SomethingStupid(void *pr) {
  uint32_t pr_u32 = (uint32_t)pr;
  char s[32];
  sprintf(s, "Stupid: %d\n", pr_u32);
  DrawStr(20, 20, s, A_NORMAL);
}

void _main(void) {

    /* const static */ border_pattern_t TEST_BORDER = (border_pattern_t) {
      .pattern.split.pattern_h = {0xF0,0x00,0x00,0xF0, 0x00, 0x00, 0x00, 0x00},
      .pattern.split.pattern_v = {0x90,0x90,0x90,0x90, 0x00, 0x00, 0x00, 0x00},
      .corners.split.corners_bl= {0xA0,0xB0,0x80,0xF0, 0x00, 0x00, 0x00, 0x00},
      .corners.split.corners_tr= {0xF0,0x10,0xD0,0x50, 0x00, 0x00, 0x00, 0x00},
      .corners.split.corners_tl= {0xF0,0x80,0xB0,0xA0, 0x00, 0x00, 0x00, 0x00},
      .corners.split.corners_br= {0x50,0xD0,0x10,0xF0, 0x00, 0x00, 0x00, 0x00},
      .width = 4,
      .height = 4
  };

    GrayOn();
    ClrScr();
    DrawMenuBorder(&TEST_BORDER, 40, 40, 24, 40);
    FontSetSys (F_4x6);
    // greyscale requires you to work with multiple planes.
    // so black text requires setting both planes!
    for(uint8_t i = 0; i <= 1; i++) {
      SetPlane(i);
      DrawStr (4+2, 76+4+2, "Maybe I was born, so that I would meet you.", A_NORMAL);
      // x: border + padding, y: 76 + border + padding
    }

    SetPlane(DARK_PLANE);
    DrawStr (4+2, 76, " Kaworu ", A_REVERSE);

    menu_item_t item1 = {
      .cursor_x = 10,
      .cursor_y = 10,
      .callback = SomethingStupid,
      .param = (void*)0xDEADBEEF
    };
    menu_item_t item2 = {
      .cursor_x = 30,
      .cursor_y = 10,
      .callback = SomethingStupid,
      .param = (void*)0xBEEFDEAD
    };

    menu_t cool_menu_with_2_options = {item1, item2};
    int coolmenu_length = sizeof(cool_menu_with_2_options) / sizeof(menu_item_t);
    // _Static_assert(sizeof(menu_options) / sizeof(menu_item_t) == 2);
    menu_t *active_menu = &cool_menu_with_2_options;
  
    int8_t active_menu_item_idx = 0;
    while(true) {
       if(active_menu) {
        char l[32];
        sprintf(l, "mi:%d\n", active_menu_item_idx);
         DrawStr(60,60,l,A_REVERSE);
         DrawMenuCursor((*active_menu)[active_menu_item_idx].cursor_x, (*active_menu)[active_menu_item_idx].cursor_y);
         short key = ngetchx(); // stupid and slow but ok for the demo
         // we really ought to use _rowread or osdeq (prolly not) because we want to be able to press [2nd].
         // we also need to ensure disable vector interrupt 1 otherwise the OS displays a status line
         // at the bottom of the display when you press 2nd/Alpha/Diamond. (GrayOn disables it since it overrides IVT entry 1.)

         if(key == KEY_UP) { // lovely how Up/Dn/L/R are macros but ENTER isnt. Thanks TI!
          // Do we really have to carry the length in the struct!?!?!? Grrr.....
          if(active_menu_item_idx >= 0 && active_menu_item_idx + 1 < coolmenu_length) {
            // hes drawn XOR so it will undraw him for now.
            DrawMenuCursor((*active_menu)[active_menu_item_idx].cursor_x, (*active_menu)[active_menu_item_idx].cursor_y);
            active_menu_item_idx++;
          }
         } else if (key == KEY_DOWN) {
          if(active_menu_item_idx - 1 >= 0) {
            // hes drawn XOR so it will undraw him for now.
            DrawMenuCursor((*active_menu)[active_menu_item_idx].cursor_x, (*active_menu)[active_menu_item_idx].cursor_y);
            active_menu_item_idx--;
          }
         } else if (key == KEY_ENTER) {
          char p[64];
          // hes drawn XOR so it will undraw him for now.
            DrawMenuCursor((*active_menu)[active_menu_item_idx].cursor_x, (*active_menu)[active_menu_item_idx].cursor_y);
          sprintf(p, "Doing:%d\n",active_menu_item_idx);
          DrawStr(70, 70, p, A_REVERSE);
          (*active_menu)[active_menu_item_idx].callback((*active_menu)[active_menu_item_idx].param);
         } else {
          break;
         }
       }
    }

    printf("Ready to die\n");
    ngetchx();
    GrayOff();
}

