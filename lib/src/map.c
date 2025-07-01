#include <stdbool.h>
#include <tigcclib.h>

#include <meow/macros.h>
#include <meow/map.h>
#include <meow/player.h>
#include <meow/test_tileset.h>

#define SCR_MAX_X 160
#define SCR_MAX_Y 100

struct __attribute__((packed)) {
  uint16_t width;
  uint16_t height;
  uint8_t data[12][8];
} const MAP_MEOW = {.width = 8,
                    .height = 12,
                    .data =
                        {

                            {0, 1, 2, 3, 0, 1, 2, 3},
                            {4, 5, 6, 7, 4, 5, 6, 7},
                            {8, 9, 10, 11, 8, 9, 10, 11},
                            {0, 1, 2, 3, 0, 1, 2, 3},
                            {4, 5, 6, 7, 4, 5, 6, 7},
                            {8, 9, 10, 11, 8, 9, 10, 11},
                            {0, 1, 2, 3, 0, 1, 2, 3},
                            {4, 5, 6, 7, 4, 5, 6, 7},
                            {8, 9, 10, 11, 8, 9, 10, 11},

                        }

};

static tileset16_t const *ActiveTileset = &test_tileset;
static map_t const *ActiveMap = (map_t const *)&MAP_MEOW;

#define BLOCK_SZ 16

void LoadMapAndTileset(map_t *m, tileset16_t const *t) {
  // at some point, we'll use TIGCCLIB LoadDll(...) here.
  _ASSERT(m);
  _ASSERT(t);

  ActiveMap = m;
  ActiveTileset = t;
}

void DrawMap(viewfinder_t *v) {

  _ASSERT(GrayOn());
  _ASSERT(ActiveTileset);
  _ASSERT(ActiveMap);

  unsigned char(*buf)[ActiveMap->height][ActiveMap->width] = (unsigned char(*)[0][0])(&ActiveMap->data);

  // uint16_t x_off = v->draw_pos.x;
 

  /**
   * A brief explanation of the draw logic.
   * We draw row by row, starting from a certain map tile (v->map_x, v->map_y).
   * We do not draw over the bounds of the map, or over the bounds of the viewfinder's rectangle,
   * specified by v->width, v->height.
   */

  uint16_t y_off = v->draw_pos.y;
  for (uint16_t h = v->tile_pos.y, x_off = v->draw_pos.x; (h < ActiveMap->height) && (h < v->tile_pos.y + v->height) && (y_off <= SCR_MAX_Y); h++, y_off += BLOCK_SZ, x_off = v->draw_pos.x) {
    for (uint16_t w = v->tile_pos.x; (w < ActiveMap->width) && (w < v->tile_pos.x + v->width) && (x_off <= SCR_MAX_X); w++, x_off += BLOCK_SZ) {
      Sprite16(x_off, y_off, BLOCK_SZ, (*ActiveTileset)[(*buf)[h][w]][0], GetPlane(DARK_PLANE), SPRT_OR);
      Sprite16(x_off, y_off, BLOCK_SZ, (*ActiveTileset)[(*buf)[h][w]][1], GetPlane(LIGHT_PLANE),
               SPRT_OR);
      //x_off += BLOCK_SZ;
    }
    //y_off += BLOCK_SZ;
  }
}

__attribute__((always_inline)) bool WithinBoundary(rect_t rect, vec2_t point) {
  return (rect.x1 < point.x && point.x < rect.x2) && (rect.y1 < point.y && point.y < rect.y2);
}

__attribute__((always_inline)) bool WithinBoundaryIncl(rect_t rect, vec2_t point) {
  return (rect.x1 <= point.x && point.x <= rect.x2) && (rect.y1 <= point.y && point.y <= rect.y2);
}
/*

__attribute__((always_inline))
bool WithinBoundaryHof(rect_t rect, vec2_t point, bool (*cmp)(uint16_t, uint16_t))
{
  return  (cmp(rect.x1,point.x) && cmp(point.x, rect.x2)) &&
          (cmp(rect.y1,point.y) && cmp(point.y, rect.y2));

}

__attribute__((always_inline))
bool WithinBoundary2(rect_t rect, vec2_t point)
{
  __attribute__((always_inline)) bool cmp(uint16_t l, uint16_t r)
  {
    return l < r;
  }

  return WithinBoundaryHof(rect, point, cmp);
}

__attribute__((always_inline))
bool WithinBoundaryIncl2(rect_t rect, vec2_t point)
{
  __attribute__((always_inline)) bool cmp(uint16_t l, uint16_t r)
  {
    return l <= r;
  }

  return WithinBoundaryHof(rect, point, cmp);
}

*/

/** @brief Internal */
__attribute__((always_inline)) vec2_draw_t _DrawPosOfTileVF16(vf_t *v, vec2_tile_t tile_pos) {
  /**
   * The intermediary calculations run into a small edge case:
   * If there comes a point where the numbers are very large (near s16 bounds),
   * it's possible this will over/underflow. However, the sign is desirable
   * for our calculations here, so we use an intermediary s32.
   * In the end, it should be an invariant that h and w are within u16,
   * as long as tile_pos's are valid.
   */

  int32_t h = ((int32_t)tile_pos.y - v->tile_pos.y);
  int32_t w = ((int32_t)tile_pos.x - v->tile_pos.x);
  _ASSERT(h <= UINT16_MAX);
  _ASSERT(w <= UINT16_MAX);

  // note: it is very possible for 16u * {w, h} to wrap
  // the screen is only like 160 pixels wide, so u8s are used everywhere...

  return (vec2_draw_t){.x = (v->draw_pos.x) + (16 * (uint16_t)w),
                       .y = (v->draw_pos.y) + (16 * (uint16_t)h)};
}

/** @todo __attribute__((always_inline)) vec2_draw_t _DrawPosOfTileVF16Fast(vec2_tile_t tile_pos) */

/**
 * @brief Will draw the player character sprite within the viewfinder.
 * If the player is outside the VF boundary, we refuse to draw.
 */
static const unsigned short TEST_SPRITE[16] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF , 0xFFFF , 0xFFFF , 0xFFFF};
void DrawPlayer(vf_t *v) {
  _ASSERT(v);
  _ASSERT(&Player);
  rect_t tmp = (rect_t){.x1 = v->tile_pos.x,
                        .y1 = v->tile_pos.y,
                        .x2 = v->tile_pos.x + v->width,
                        .y2 = v->tile_pos.y + v->height};
  printf_xy(0, 80, "(%d,%d),(%d,%d),(%d,%d)",tmp.x1,tmp.y1,tmp.x2,tmp.y2, Player.pos.x, Player.pos.y);
  if (WithinBoundaryIncl(tmp, Player.pos)) {
    vec2_draw_t p = _DrawPosOfTileVF16(v, Player.pos);
    printf_xy(0, 90, "p:(%d,%d)",p.x,p.y);
    _Sprite16Mono(p.x, p.y, 16, TEST_SPRITE, SPRT_OR);
  }
}
