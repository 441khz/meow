#ifndef MAP_H
#define MAP_H

#include <stdint.h>

typedef struct {
  uint16_t width;
  uint16_t height;
  uint8_t data[];
} __attribute((packed)) map_t;

// dark, then light. (it seems tttiler does light, dark)
typedef uint16_t tile16_t[2][16];
typedef tile16_t tileset16_t[];

typedef uint8_t tile8_t[2][8];
typedef tile8_t tileset8_t[];

/* Please be in ROM! */
extern tileset16_t *ActiveTileset;
extern map_t *ActiveMap;

void DrawMap();

#endif
