#ifndef MENU_H
#define MENU_H

#include <stdint.h>

/**
 * @brief The Menu Manager system can handle button inputs. It is up to you as
 * to decide how to handle button inputs though, but the manager system makes it simple.
 *
 * There are plenty of ways to handle menus.
 * Method 1: Automatic
 *    Create and SetupMenuManager()
 *    Use the HandleMenuSimple() macro in macros.h
 *    This will simply fire a callback when you press something, or do nothing if you ESC.
 * Method 2: Manual
 *    Create and SetupMenuManager
 *    Store the result of StartMenuManager in a menu_item_t *
 *         Cases of return value:
 *             result = NULL,             the ESC button was pressed.
 *             result->callback = NULL,   Pressed [2nd] on a menu_item with no callback.
 *    Also: you may want to case on a specific option only, but not have it fire a callback.
 *    This is entirely possible by:
 *         1. Using the opaque value to store a tag
 *                if(result->opaque == MY_MAGIC) { // do thing }
 *         2. Casing against the menu_item_t * pointer
 *                if((void *)result == (void *)my_menu_item) { // do thing }
 *         3. @todo Casing against the index
 *                if(result->idx == 2) { // do thing }
 *    Callback firing is no longer done inside the Menu Manager routines itself
 */

/**
 * @brief Specifies a border pattern (sprite set).
 * @todo interleave planes, rotation code?
 */
typedef struct border_pattern_t {
  union {
    uint8_t pattern_full[8][8];
    struct {
      struct {
        uint8_t horz_top[8];  // horizontal bar
        uint8_t vert_left[8]; // vertical bar
        uint8_t horz_bot[8];
        uint8_t vert_right[8];
      } light;
      struct {
        uint8_t horz_top[8];  // horizontal bar
        uint8_t vert_left[8]; // vertical bar
        uint8_t horz_bot[8];
        uint8_t vert_right[8];
      } dark;
    } plane;
  } pattern; // vertical and horizontal bar sprites of the border box
  union {
    uint8_t corners_full[8][8];
    struct {
      struct {
        uint8_t tl[8]; // top left
        uint8_t tr[8]; // top right
        uint8_t bl[8]; // bottom left
        uint8_t br[8]; // bottom right
      } light;
      struct {
        uint8_t tl[8]; // top left
        uint8_t tr[8]; // top right
        uint8_t bl[8]; // bottom left
        uint8_t br[8]; // bottom right
      } dark;
    } plane;
  } corners; // corner sprites of the border box
  uint8_t width : 4;
  uint8_t height : 4;
} border_pattern_t;

/** @brief Magic for the menu cursor jump table to not do anything */
#define MENU_NIL (uint8_t)0xFF

/**
 * @brief Menu item structure.
 * @param[table] - Which menu item index to jump to from this item
 */
typedef struct menu_item_t {
  uint8_t cursor_x;
  uint8_t cursor_y;
  void (*callback)(void *);
  void *opaque; // to be provided to the callback at handler time.

  /* jump indexing fields - for each button, which index to jump to? */
  union {
    uint32_t mask;
    uint8_t table[4];
    struct {
      uint32_t up : 8;
      uint32_t down : 8;
      uint32_t left : 8;
      uint32_t right : 8;
    } idx;
  } jump;

} menu_item_t;

/** @brief List of menu items with a known length. */
typedef struct menu_t {
  uint8_t length;
  menu_item_t (*items)[];
} menu_t;

inline void DrawMenuCursor4(uint8_t x, uint8_t y, uint8_t attr);
inline void DrawMenuCursor8(uint8_t x, uint8_t y, uint8_t attr);
void DrawSimpleMenuBorder(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void DrawMenuBorder(border_pattern_t *pattern, uint8_t x, uint8_t y, uint8_t w, uint8_t h);

inline void ResetMenuManager();
void SetupMenuManager(menu_t *menu, uint8_t initial_idx, uint8_t cursor_sz);
menu_item_t *StartMenuManager();

void DisplayStrTextBox(char str[]);

#endif
