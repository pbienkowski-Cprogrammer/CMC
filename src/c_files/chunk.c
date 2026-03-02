#include "chunk.h"

#include "settings.h"
#include <linmath.h>
#include <stb/stb_ds.h>
#include "math_help_functions.h"
#include <open-simplex-noise.h>

struct osn_context *osn_ctx;

khash_t(chunk_map) *chunkMap;

float blockData[BLOCK_SIDES * BLOCK_SIDE_DATA] = {
    //WEST
    //position		 normals			texCoords	sideIndex
    0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0,
    0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0,
    0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0,
    0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0,
    0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0,
    0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0,

    //SOUTH
    0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1,
    1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1,
    0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1,
    0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1,
    1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1,
    1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1,

    //EAST
    1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2,
    1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 2,
    1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2,
    1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2,
    1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 2,
    1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 2,

    //NORTH
    1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 3,
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 3,
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 3,
    1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 3,
    0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 3,
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 3,

    //TOP
    1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,	0.0f, 1.0f, 4,
    0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	1.0f, 0.0f, 4,
    1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 4,
    1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,	0.0f, 1.0f, 4,
    0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,	1.0f, 1.0f, 4,
    0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	1.0f, 0.0f, 4,

    //BOTTOM
    1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 5,
    1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 5,
    0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 5,
    1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 5,
    0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 5,
    0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 5,
};

//indeces/positions on texture atlas
vec2 texPosis[TEXTURES_IN_ATLAS] = {
    { 0, 0 }, //FLOWER_GRASS_TOP_TEXTURE
    { 1, 0 }, //GRASS_SIDE_TEXTURE
    { 2, 0 }, //DIRT_TEXTURE
    { 0, 1 }, //STONE_TEXTURE
    { 1, 1 }, //GRASS_TOP_TEXTURE
};

//what texture on what block side
unsigned int blockSidesTextures[BLOCKS][6] = {
    { DIRT_TEXTURE, DIRT_TEXTURE, DIRT_TEXTURE, DIRT_TEXTURE, DIRT_TEXTURE, DIRT_TEXTURE }, //dirt_block
    { GRASS_SIDE_TEXTURE, GRASS_SIDE_TEXTURE, GRASS_SIDE_TEXTURE, GRASS_SIDE_TEXTURE, FLOWER_GRASS_TOP_TEXTURE, DIRT_TEXTURE }, //flower_grass_block
    { STONE_TEXTURE, STONE_TEXTURE, STONE_TEXTURE, STONE_TEXTURE, STONE_TEXTURE, STONE_TEXTURE }, //stone_block
    { GRASS_SIDE_TEXTURE, GRASS_SIDE_TEXTURE, GRASS_SIDE_TEXTURE, GRASS_SIDE_TEXTURE, GRASS_TOP_TEXTURE, DIRT_TEXTURE }, //grass_block
};

void addSide(Chunk* chunk, Sides side, float* blockPos, int index)
{
    for(int i = 0; i < 6; i++)
    {
        arrpush(chunk->drawData, blockData[(side * 6 + i) * 9 + 0] + blockPos[0] + chunk->pos[0] * CHUNK_SIZE);
        arrpush(chunk->drawData, blockData[(side * 6 + i) * 9 + 1] + blockPos[1]);
        arrpush(chunk->drawData, blockData[(side * 6 + i) * 9 + 2] + blockPos[2] + chunk->pos[1] * CHUNK_SIZE);
        arrpush(chunk->drawData, blockData[(side * 6 + i) * 9 + 3]);
        arrpush(chunk->drawData, blockData[(side * 6 + i) * 9 + 4]);
        arrpush(chunk->drawData, blockData[(side * 6 + i) * 9 + 5]);
        arrpush(chunk->drawData, (blockData[(side * 6 + i) * 9 + 6] + texPosis[blockSidesTextures[chunk->chunkData[index] - 1][side]][0]) / TEXTURES_IN_ATLAS_WIDTH - (blockData[(side * 6 + i) * 9 + 6] == 0.0f ? -0.0001f : 0.0001f));
        arrpush(chunk->drawData, (blockData[(side * 6 + i) * 9 + 7] + ((TEXTURES_IN_ATLAS_HEIGHT - 1) - texPosis[blockSidesTextures[chunk->chunkData[index] - 1][side]][1])) / TEXTURES_IN_ATLAS_HEIGHT - (blockData[(side * 6 + i) * 9 + 7] == 0.0f ? -0.0001f : 0.0001f));
        arrpush(chunk->drawData, blockData[(side * 6 + i) * 9 + 8]);
    }
}

void generateChunk(int chunkX, int chunkZ)
{
    uint64_t key;
    vec2ToHashKey(chunkX, chunkZ, &key);

    int isNew;
    khint_t slot = kh_put(chunk_map, chunkMap, key, &isNew);

    if(isNew)
    {
        Chunk* chunk = malloc(sizeof(Chunk));
        chunk->pos[0] = chunkX;
        chunk->pos[1] = chunkZ;
        chunk->generatedDrawData = false;

        kh_value(chunkMap, slot) = chunk;

        for(int x = 0; x < CHUNK_SIZE; x++)
        {
            for(int z = 0; z < CHUNK_SIZE; z++)
            {
                float multiplyer = 0.01f;
                int height = CHUNK_HEIGHT * (open_simplex_noise2(osn_ctx, (x + chunkX * CHUNK_SIZE) * multiplyer, (z + chunkZ * CHUNK_SIZE) * multiplyer) * 0.5f + 0.5f);

                for(int y = 0; y < CHUNK_HEIGHT; y++)
                {
                    int index;
                    posToIndex((vec3){x, y, z}, &index);

                    if(y > height)
                    {
                        chunk->chunkData[index] = AIR_BLOCK;
                    }
                    else if(y == height)
                    {
                        chunk->chunkData[index] = GRASS_BLOCK;
                    }
                    else if(y < height && y > height - 3)
                    {
                        chunk->chunkData[index] = DIRT_BLOCK;
                    }
                    else
                    {
                        chunk->chunkData[index] = STONE_BLOCK;
                    }
                }
            }
        }
    }
}

void generateChunkDrawData(int chunkX, int chunkZ)
{
    uint64_t key;
    vec2ToHashKey(chunkX, chunkZ, &key);

    khint_t foundChunk = kh_get(chunk_map, chunkMap, key);
    Chunk* chunk = kh_value(chunkMap, foundChunk);

    vec2ToHashKey(chunkX - 1, chunkZ, &key);
    foundChunk = kh_get(chunk_map, chunkMap, key);
    Chunk* westChunk = kh_value(chunkMap, foundChunk);

    vec2ToHashKey(chunkX + 1, chunkZ, &key);
    foundChunk = kh_get(chunk_map, chunkMap, key);
    Chunk* eastChunk = kh_value(chunkMap, foundChunk);

    vec2ToHashKey(chunkX, chunkZ + 1, &key);
    foundChunk = kh_get(chunk_map, chunkMap, key);
    Chunk* southChunk = kh_value(chunkMap, foundChunk);

    vec2ToHashKey(chunkX, chunkZ - 1, &key);
    foundChunk = kh_get(chunk_map, chunkMap, key);
    Chunk* northChunk = kh_value(chunkMap, foundChunk);

    chunk->drawData = NULL;
    for(int i = 0; i < CHUNK_VOLUME; i++)
    {
        vec3 blockPos;
        indexToPos(i, blockPos);

        if(chunk->chunkData[i] > 0)
        {
            //this is a really monstrous code, but its the most optimized version of it

            vec3 tempPos;
            int tempIndex;
            tempPos[0] = blockPos[0];
            tempPos[1] = blockPos[1];
            tempPos[2] = blockPos[2];

            //WEST
            if(tempPos[0] - 1 < 0)
            {
                tempPos[0] = CHUNK_SIZE - 1;
                posToIndex(tempPos, &tempIndex);

                if(westChunk->chunkData[tempIndex] == AIR_BLOCK)
                {
                    addSide(chunk, WEST_SIDE, blockPos, i);
                }

                tempPos[0] = 0.0f;
            }
            else
            {
                tempPos[0]--;
                posToIndex(tempPos, &tempIndex);

                if(chunk->chunkData[tempIndex] == AIR_BLOCK)
                {
                    addSide(chunk, WEST_SIDE, blockPos, i);
                }

                tempPos[0]++;
            }

            //EAST
            if(tempPos[0] + 1 >= CHUNK_SIZE)
            {
                tempPos[0] = 0.0f;
                posToIndex(tempPos, &tempIndex);

                if(eastChunk->chunkData[tempIndex] == AIR_BLOCK)
                {
                    addSide(chunk, EAST_SIDE, blockPos, i);
                }

                tempPos[0] = CHUNK_SIZE - 1;
            }
            else
            {
                tempPos[0]++;
                posToIndex(tempPos, &tempIndex);

                if(chunk->chunkData[tempIndex] == AIR_BLOCK)
                {
                    addSide(chunk, EAST_SIDE, blockPos, i);
                }

                tempPos[0]--;
            }

            //SOUTH
            if(tempPos[2] + 1 >= CHUNK_SIZE)
            {
                tempPos[2] = 0.0f;
                posToIndex(tempPos, &tempIndex);

                if(southChunk->chunkData[tempIndex] == AIR_BLOCK)
                {
                    addSide(chunk, SOUTH_SIDE, blockPos, i);
                }

                tempPos[2] = CHUNK_SIZE - 1;
            }
            else
            {
                tempPos[2]++;
                posToIndex(tempPos, &tempIndex);

                if(chunk->chunkData[tempIndex] == AIR_BLOCK)
                {
                    addSide(chunk, SOUTH_SIDE, blockPos, i);
                }

                tempPos[2]--;
            }

            //NORTH
            if(tempPos[2] - 1 < 0)
            {
                tempPos[2] = CHUNK_SIZE - 1;
                posToIndex(tempPos, &tempIndex);

                if(northChunk->chunkData[tempIndex] == AIR_BLOCK)
                {
                    addSide(chunk, NORTH_SIDE, blockPos, i);
                }

                tempPos[2] = 0.0f;
            }
            else
            {
                tempPos[2]--;
                posToIndex(tempPos, &tempIndex);

                if(chunk->chunkData[tempIndex] == AIR_BLOCK)
                {
                    addSide(chunk, NORTH_SIDE, blockPos, i);
                }

                tempPos[2]++;
            }

            //TOP
            if(tempPos[1] + 1 >= CHUNK_HEIGHT)
            {
                addSide(chunk, TOP_SIDE, blockPos, i);
            }
            else
            {
                tempPos[1]++;
                posToIndex(tempPos, &tempIndex);

                if(chunk->chunkData[tempIndex] == AIR_BLOCK)
                {
                    addSide(chunk, TOP_SIDE, blockPos, i);
                }

                tempPos[1]--;
            }

            //BOTTOM
            if(tempPos[1] - 1 < 0)
            {
                addSide(chunk, BOTTOM_SIDE, blockPos, i);
            }
            else
            {
                tempPos[1]--;
                posToIndex(tempPos, &tempIndex);

                if(chunk->chunkData[tempIndex] == AIR_BLOCK)
                {
                    addSide(chunk, BOTTOM_SIDE, blockPos, i);
                }

                tempPos[1]++;
            }
        }
    }

    chunk->vertices = arrlen(chunk->drawData) / 6;
    chunk->generatedDrawData = true;
}

void compileChunkDrawData(int chunkX, int chunkZ)
{
    uint64_t key;
    vec2ToHashKey(chunkX, chunkZ, &key);

    khint_t foundChunk = kh_get(chunk_map, chunkMap, key);
    Chunk* chunk = kh_value(chunkMap, foundChunk);

    glEnable(GL_DEPTH_TEST);

    glGenBuffers(1, &chunk->VBO);
    glGenVertexArrays(1, &chunk->VAO);

    glBindVertexArray(chunk->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, chunk->VBO);
    glBufferData(GL_ARRAY_BUFFER, arrlen(chunk->drawData) * sizeof(float), chunk->drawData, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, BLOCK_DATA_STRIDE * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, BLOCK_DATA_STRIDE * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);

    arrfree(chunk->drawData);
}
