#ifndef MENU_H
#define MENU_H

#include <stdint.h>

/**
 * @brief The way I'd like the menu system to work is that cursor and buttons are
 * handled by the program itself, while drawing text or sprites something is
 * handled by the user. There's also DrawTextBox(...) which can just draw lines
 * of text.
 * @todo Small border patterns (think 4x4) can probably have memory saved by
 * clobbering the empty part of the Sprite8 data. Also at some point, it
 * probably becomes cheaper to compute the transpose as constexpr and store it
 * in the rodata/bss or whatever, and pull from there, rather than computing
 * transpose just in time.
 * For now, we just specify size in the struct and always use 8x8 in memory,
 * even if its 4x4, which is a little wasteful. And also, we do transpose on the
 * fly, which probably costs more in program instructions than it does to just
 * store the borders already transposed.... (for some large number of border
 * types)
 */

/**
 * @brief Specifies a border pattern (sprite set).
 * @todo This is a little memory costly for right now, but I couldn't figure out the graphics
 * code to rotate a packed bitmap yet...
 */
typedef struct border_pattern_t {
  union {
    uint8_t pattern[8][2];
    struct {
      uint8_t pattern_h[8]; // horizontal bar
      uint8_t pattern_v[8]; // vertical bar
    } split;
  } pattern; // vertical and horizontal bar sprites of the border box
  union {
    uint8_t corners[8][4];
    struct {
      uint8_t corners_tl[8]; // top left
      uint8_t corners_tr[8]; // top right
      uint8_t corners_bl[8]; // bottom left
      uint8_t corners_br[8]; // bottom right
    } split;
  } corners; // corner sprites of the border box
  uint8_t width : 4;
  uint8_t height : 4;
} border_pattern_t;

/** @brief Magic for the menu cursor jump table to not do anything */
#define MENU_NIL (uint8_t)0xFF

/**
 * @brief Menu item struct type. Menus are just a list of cursor positions and
 * callbacks. The cursor is controlled by the engine. e.g: Press the up arrow, engine
 * will handle it and move to the next (cursor_x, cursor_y) in the active menu_t
 * list. It is up to you, the programmer to draw the menu in a way that looks
 * right by using DrawMenuBorder, DrawStr, etc!
 * @todo custom menu ordering, remove (void *)param?
 * @todo
 */
typedef struct menu_item_t {
  uint8_t cursor_x;
  uint8_t cursor_y;
  void (*callback)(void *);
  void *opaque; // to be provided to the callback at handler time.
                // i really don't know how ideal this is. i dont want to make a
                // mini-ABI where everyone now has to accept func(void *) and
                // then work w that. i'm not pthread for fucks sake! lol

  /* jump indexing fields - for each button, which index to jump to? */
  union {
    uint32_t mask;
    uint8_t table[4];
    struct {
      uint32_t up : 8;
      uint32_t down : 8;
      uint32_t left : 8;
      uint32_t right : 8;
    } idx; // is this really a "Struct"?
  } jump;  // sizeof(...) = 4? why does clangd say 8 ... uh-oh...

} menu_item_t;

/**
 * @brief Menu structure. More simply, a list of cursor positions and callbacks.
 * @todo impl void(*dtor)(void)? when you exit the menu, shouldn't we properly undraw it?
 */
typedef struct menu_t {
  uint8_t length;
  menu_item_t (*items)[];
} menu_t;

inline void DrawMenuCursor(uint8_t x, uint8_t y);
void DrawSimpleMenuBorder(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void DrawMenuBorder(border_pattern_t *pattern, uint8_t x, uint8_t y, uint8_t w, uint8_t h);

#endif
