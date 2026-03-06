#ifndef GAME_DATA_C
#define GAME_DATA_C

#include "settings.h"
#include <linmath.h>

typedef enum
{
    WEST_SIDE = 0,
    SOUTH_SIDE = 1,
    EAST_SIDE = 2,
    NORTH_SIDE = 3,
    TOP_SIDE = 4,
    BOTTOM_SIDE = 5,
} Side;

//all textures
typedef enum
{
    FLOWER_GRASS_TOP_TEXTURE = 0,
    GRASS_SIDE_TEXTURE = 1,
    DIRT_TEXTURE = 2,
    STONE_TEXTURE = 3,
    GRASS_TOP_TEXTURE = 4,
    SAND_TEXTURE = 5
} Texture;

//all blocks
typedef enum
{
    AIR_BLOCK = 0,
    DIRT_BLOCK = 1,
    FLOWER_GRASS_BLOCK = 2,
    STONE_BLOCK = 3,
    GRASS_BLOCK = 4,
    SAND_BLOCK = 5
} Block;

extern float blockData[BLOCK_SIDES * BLOCK_SIDE_DATA];

extern vec2 texPosis[TEXTURES_IN_ATLAS];

extern unsigned int blockSidesTextures[BLOCKS][6];

#endif
