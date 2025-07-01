#ifndef MAP_H
#define MAP_H

#include <stdint.h>

typedef struct {
  uint16_t x;
  uint16_t y;
} vec2_t;

typedef vec2_t vec2_tile_t;
typedef vec2_t vec2_draw_t;

typedef struct {
  uint16_t x1;
  uint16_t y1;
  uint16_t x2;
  uint16_t y2;
} rect_t;

/** @brief Viewfinder struct lets us draw the map anywhere and of any width/height/pos */
typedef struct {
  vec2_t tile_pos;
  uint16_t width;
  uint16_t height;
  vec2_t draw_pos;
} viewfinder_t;

typedef viewfinder_t vf_t;

extern viewfinder_t ActiveViewfinder;

typedef struct {
  uint16_t width;
  uint16_t height;
  uint8_t data[];
} __attribute((packed)) map_t;

/* dark, then light. tttiler does light, then dark. */
#define MAKE_TILE_TYPES(sz)                                                                             \
  typedef uint##sz##_t tile##sz##_t[2][sz];                                                             \
  typedef tile##sz##_t tileset##sz##_t[];

MAKE_TILE_TYPES(8)
MAKE_TILE_TYPES(16)
MAKE_TILE_TYPES(32)

#undef MAKE_TILE_TYPES

/* Please be in ROM! */
// extern tileset16_t *ActiveTileset;
// extern map_t *ActiveMap;

void DrawMap(viewfinder_t *v);
void DrawPlayer(viewfinder_t *v);

#endif
