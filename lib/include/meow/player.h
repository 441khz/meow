#ifndef PLAYER_H
#define PLAYER_H

#include <meow/map.h>
#include <meow/menu.h>

typedef struct {
  border_pattern_t *border;
  uint8_t border_number;
  vec2_t pos;
  viewfinder_t vf;
} player_t;

/* player information */
extern player_t Player;

/* borders */
extern border_pattern_t BORDER_DEFAULT;
extern border_pattern_t BORDER_PKMN;

#endif
