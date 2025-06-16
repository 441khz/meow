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

void DrawSimpleMenuBorder(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void DrawMenuBorder(border_pattern_t *pattern, uint8_t x, uint8_t y, uint8_t w, uint8_t h);

#endif
