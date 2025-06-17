#ifndef MENU_H
#define MENU_H

#include <stdint.h>

/**
 * @todo Small border patterns (think 4x4) can probably have memory saved by clobbering the empty part of the Sprite8 data.
 * Also at some point, it probably becomes cheaper to compute the transpose as constexpr
 * and store it in the rodata/bss or whatever, and pull from there, rather than computing
 * transpose just in time.
 * 
 * For now, we just specify size in the struct and always use 8x8 in memory, even if its 4x4, which is a little wasteful.
 * And also, we do transpose on the fly, which probably costs more in program instructions than it does to just store
 * the borders already transposed.... (for some large number of border types)
*/

/**
 * The way I'd like the menu system to work is that cursor and buttons are handled by the program itself,
 * while drawing text or sprites something is handled by the user.
 * There's also DrawTextBox(...) which can just draw lines of text.
*/

/**
 * @brief Specifies a border pattern.
 * This is a little costly for right now, but I couldn't figure out the graphics
 * code to rotate a packed bitmap yet... it may be available in ExtGraph though.
 * Also. NO ANONYMOUS STRUCT & UNION SUPPORT!!!! Nooooooo!!! (and no GNU exts!)
*/
typedef struct border_pattern_t {
    union {
        uint8_t pattern[8][2]; 
        struct {
            uint8_t pattern_h[8];
            uint8_t pattern_v[8];
        } split;
    } pattern;
    union {
        uint8_t corners[8][4];
        struct {
            uint8_t corners_tl[8];
            uint8_t corners_tr[8];
            uint8_t corners_bl[8];
            uint8_t corners_br[8];
        } split;
    } corners;
    uint8_t width : 4; 
    uint8_t height: 4;
} border_pattern_t;

/**
 * @brief Menu item struct type. Menus are just a list of cursor positions and callbacks.
 * The cursor is controlled by the engine. Press the up arrow, engine will handle it and move
 * to the next (cursor_x, cursor_y) in the active menu_t list.
 * It is up to you, the programmer to draw the menu in a way that looks right by
 * using DrawMenuBorder, DrawStr, etc!
 * TODO: menus don't have one strict order to them, you can jump around...
*/

// this would probably be better as a linked list
typedef struct menu_item_t {
    uint8_t cursor_x;
    uint8_t cursor_y;
    void (*callback)(void*);
    void *param; // to be provided to the callback at handler time.
                 // i really don't know how ideal this is. i dont want to make a mini-ABI
                 // where everyone now has to accept func(void *) and then work w that.
                 // i'm not pthread for fucks sake! lol (TODO)
} menu_item_t;

// this really should be in rodata so you can sizeof.
typedef menu_item_t menu_t[];

inline void DrawMenuCursor(uint8_t x, uint8_t y);
void DrawSimpleMenuBorder(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void DrawMenuBorder(border_pattern_t *pattern, uint8_t x, uint8_t y, uint8_t w, uint8_t h);

#endif
