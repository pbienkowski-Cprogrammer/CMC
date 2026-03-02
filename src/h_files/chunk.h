#ifndef CHUNK_H
#define CHUNK_H

#include "settings.h"

#include <glad.h>
#include <linmath.h>
#include <stdbool.h>
#include <stdint.h>
#include <khash.h>

extern float blockData[BLOCK_SIDES * BLOCK_SIDE_DATA];

typedef enum
{
    WEST_SIDE = 0,
    SOUTH_SIDE = 1,
    EAST_SIDE = 2,
    NORTH_SIDE = 3,
    TOP_SIDE = 4,
    BOTTOM_SIDE = 5,
} Sides;

//all textures
typedef enum
{
    FLOWER_GRASS_TOP_TEXTURE = 0,
    GRASS_SIDE_TEXTURE = 1,
    DIRT_TEXTURE = 2,
    STONE_TEXTURE = 3,
    GRASS_TOP_TEXTURE = 4
} Textures;

//indeces/positions on texture atlas
extern vec2 texPosis[TEXTURES_IN_ATLAS];

//all blocks
typedef enum
{
    AIR_BLOCK = 0,
    DIRT_BLOCK = 1,
    FLOWER_GRASS_BLOCK = 2,
    STONE_BLOCK = 3,
    GRASS_BLOCK = 4,
} Blocks;

//what texture on what block side
extern unsigned int blockSidesTextures[BLOCKS][6];

typedef struct Chunk
{
    vec2 pos;
    uint8_t chunkData[CHUNK_VOLUME];
    float* drawData;
    GLuint VBO, VAO;
    bool generatedDrawData;
    unsigned int vertices;
} Chunk;

void generateChunk(int chunkX, int chunkZ);
void generateChunkDrawData(int chunkX, int chunkZ);
void compileChunkDrawData(int chunkX, int chunkZ);

KHASH_MAP_INIT_INT64(chunk_map, Chunk*)
extern khash_t(chunk_map) *chunkMap;

extern struct osn_context *osn_ctx;

#endif
