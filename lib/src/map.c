#include <tigcclib.h>

#include <meow/macros.h>
#include <meow/map.h>
#include <meow/test_tileset.h>

struct __attribute__((packed)) {
  uint16_t width;
  uint16_t height;
  uint8_t data[6][10];
} const MAP_MEOW = {.width = 10, .height = 6, .data = {
  {0, 0, 0, 0, 0, 0, 7, 8, 9,  7},
  {0, 0, 0, 0, 0, 0, 7, 7, 10, 7},
  {0, 4, 5, 0, 0, 0, 0, 11, 0, 0},
  {0, 8, 9, 0, 0, 0, 0, 0, 0,  0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0,  0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0,  0}
}

};

tileset16_t *ActiveTileset = (tileset16_t *)&test_tileset;
map_t *ActiveMap = (map_t *)&MAP_MEOW;

#define BLOCK_SZ 16

/**
 * @brief Assume each tile is an 8px block.
 */
void DrawMap() {

  _ASSERT(GrayOn());
  _ASSERT(ActiveTileset);
  _ASSERT(ActiveMap);

  ClrScrGrey();

  unsigned char(*buf)[ActiveMap->height][ActiveMap->width] = (unsigned char(*)[0][0])(&ActiveMap->data);

  uint16_t x_off = 0;
  uint16_t y_off = 0;

  for (uint16_t h = 0; h < ActiveMap->height; h++) {
    x_off = 0;
    for (uint16_t w = 0; w < ActiveMap->width; w++) {
      Sprite16(x_off, y_off, BLOCK_SZ, (*ActiveTileset)[(*buf)[h][w]][0], GetPlane(DARK_PLANE), SPRT_OR);
      Sprite16(x_off, y_off, BLOCK_SZ, (*ActiveTileset)[(*buf)[h][w]][1], GetPlane(LIGHT_PLANE), SPRT_OR);
      /** @todo: this macro is a little fucked for now. */
      // _Sprite8GreyAL(x_off, y_off, 8, (*ActiveTileset)[(*buffer)[y][x]], SPRT_OR);
      x_off += BLOCK_SZ;
    }
    y_off += BLOCK_SZ;
  }

  // printf_xy(0, 0, "Done.%d,%d", x_off, y_off);
}
