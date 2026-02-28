#include <glad.h>
#include "voxel_engine.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stb/stb_ds.h>
#include <linmath.h>
#include <khash.h>
#include <open-simplex-noise.h>
#include <stdlib.h>
#include "window.h"
#include "opengl_help_functions.h"
#include "math_help_functions.h"
#include "player.h"
#include "settings.h"

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
vec2 texPosis[TEXTURES_IN_ATLAS] = {
    { 0, 0 }, //FLOWER_GRASS_TOP_TEXTURE
    { 1, 0 }, //GRASS_SIDE_TEXTURE
    { 2, 0 }, //DIRT_TEXTURE
    { 0, 1 }, //STONE_TEXTURE
    { 1, 1 }, //GRASS_TOP_TEXTURE
};

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
unsigned int blockSidesTextures[BLOCKS][6] = {
    { DIRT_TEXTURE, DIRT_TEXTURE, DIRT_TEXTURE, DIRT_TEXTURE, DIRT_TEXTURE, DIRT_TEXTURE }, //dirt_block
    { GRASS_SIDE_TEXTURE, GRASS_SIDE_TEXTURE, GRASS_SIDE_TEXTURE, GRASS_SIDE_TEXTURE, FLOWER_GRASS_TOP_TEXTURE, DIRT_TEXTURE }, //flower_grass_block
    { STONE_TEXTURE, STONE_TEXTURE, STONE_TEXTURE, STONE_TEXTURE, STONE_TEXTURE, STONE_TEXTURE }, //stone_block
    { GRASS_SIDE_TEXTURE, GRASS_SIDE_TEXTURE, GRASS_SIDE_TEXTURE, GRASS_SIDE_TEXTURE, GRASS_TOP_TEXTURE, DIRT_TEXTURE }, //grass_block
};

typedef struct Chunk
{
    vec2 pos;
    uint8_t chunkData[CHUNK_VOLUME];
    float* drawData;
    GLuint VBO, VAO;
    unsigned int vertices;
} Chunk;

GLuint shaderProgram;
GLuint texture1;

Player player;

struct osn_context *osn_ctx;

KHASH_MAP_INIT_INT64(chunk_map, Chunk*)
khash_t(chunk_map) *chunkMap;

void generateChunk(int chunkX, int chunkZ);
void generateChunkDrawData(int chunkX, int chunkZ);
void compileChunkDrawData(int chunkX, int chunkZ);

void engineUpdate()
{
    playerUpdate(&player);
};

void engineDraw()
{
    mat4x4 trans, perspe;
    mat4x4_perspective(perspe, degToRad(player.camera.FOV), (float)windowWidth / (float)windowHeight, 0.1f, 1000.0f);

    calculatePlayerData(&player);

    mat4x4 view;
    mat4x4_look_at(view, player.camera.pos, player.target, absoluteUp);

    glClearColor(0.0f, 0.7f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    glUniformMatrix4fv(uniLoc(shaderProgram, "view"), 1, GL_FALSE, view[0]);
    glUniformMatrix4fv(uniLoc(shaderProgram, "perspe"), 1, GL_FALSE, perspe[0]);
    glUniformMatrix4fv(uniLoc(shaderProgram, "trans"), 1, GL_FALSE, trans[0]);

    float currentChunkX = player.camera.pos[0] / (float)CHUNK_SIZE;
    float currentChunkZ = player.camera.pos[2] / (float)CHUNK_SIZE;

    int startChunkX, startChunkZ;
    int endChunkX, endChunkZ;

    if(DRAW_AREA_IN_CHUNKS % 2 != 0)
    {
        startChunkX = (int)floor(currentChunkX) - (DRAW_AREA_IN_CHUNKS - 1) / 2;
        startChunkZ = (int)floor(currentChunkZ) - (DRAW_AREA_IN_CHUNKS - 1) / 2;

        endChunkX = (int)floor(currentChunkX) + (DRAW_AREA_IN_CHUNKS - 1) / 2;
        endChunkZ = (int)floor(currentChunkZ) + (DRAW_AREA_IN_CHUNKS - 1) / 2;
    }
    else
    {
        startChunkX = (int)floor(currentChunkX) - (DRAW_AREA_IN_CHUNKS - 2) / 2;
        startChunkZ = (int)floor(currentChunkZ) - (DRAW_AREA_IN_CHUNKS - 2) / 2;

        endChunkX = (int)floor(currentChunkX) + (DRAW_AREA_IN_CHUNKS - 2) / 2;
        endChunkZ = (int)floor(currentChunkZ) + (DRAW_AREA_IN_CHUNKS - 2) / 2;

        float offsetInChunkX, offsetInChunkZ;

        if(currentChunkX >= 0)
        {
            offsetInChunkX = currentChunkX - floor(currentChunkX);
            offsetInChunkZ = currentChunkZ - floor(currentChunkZ);
        }
        else
        {
            offsetInChunkX = floor(currentChunkX) * -1 - currentChunkX * -1;
            offsetInChunkZ = floor(currentChunkZ) * -1 - currentChunkZ * -1;
        }

        if(offsetInChunkX < 0.5f)
        {
            startChunkX -= 1;
        }
        else
        {
            endChunkX += 1;
        }

        if(offsetInChunkZ < 0.5f)
        {
            startChunkZ -= 1;
        }
        else
        {
            endChunkZ += 1;
        }
    }

    for(int x = startChunkX - 1; x <= endChunkX + 1; x++)
    {
        for(int z = startChunkZ - 1; z <= endChunkZ + 1; z++)
        {
            uint64_t key;
            vec2ToHashKey(x, z, &key);

            khint_t foundChunk = kh_get(chunk_map, chunkMap, key);

            if(foundChunk == kh_end(chunkMap))
            {
                generateChunk(x, z);
                generateChunkDrawData(x, z);
                compileChunkDrawData(x, z);
            }
        }
    }

    for(int x = startChunkX; x <= endChunkX; x++)
    {
        for(int z = startChunkZ; z <= endChunkZ; z++)
        {
            uint64_t key;
            vec2ToHashKey(x, z, &key);

            khint_t foundChunk = kh_get(chunk_map, chunkMap, key);

            if(foundChunk != kh_end(chunkMap))
            {
                Chunk* chunk = kh_value(chunkMap, foundChunk);

                glBindVertexArray(chunk->VAO);

                glDrawArrays(GL_TRIANGLES, 0, chunk->vertices);
            }
        }
    }
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

        kh_value(chunkMap, slot) = chunk;

        for(int x = 0; x < CHUNK_SIZE; x++)
        {
            for(int z = 0; z < CHUNK_SIZE; z++)
            {
                //int height = CHUNK_HEIGHT - (sin((x + chunkX * CHUNK_SIZE) * 0.1f) * 0.5f + 0.5f) * 5;

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
                addSide(chunk, WEST_SIDE, blockPos, i);
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
                addSide(chunk, EAST_SIDE, blockPos, i);
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
                addSide(chunk, SOUTH_SIDE, blockPos, i);
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
                addSide(chunk, NORTH_SIDE, blockPos, i);
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

bool initEngine()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    chunkMap = kh_init(chunk_map);

    if(!createShader(&shaderProgram, "src/shader_files/vert.glsl", "src/shader_files/frag.glsl"))
    {
        printf("failed to create shader\n");
        return false;
    }

    if(!createTexture2D(&texture1, "res/texture_atlas.png"))
    {
        printf("failed to create texture\n");
        return false;
    }

    activateTexture2D(texture1, GL_TEXTURE0);

    glUseProgram(shaderProgram);

    glUniform1i(uniLoc(shaderProgram, "texture1"), 0);

    initPlayer(&player, (vec3){0.0f, 100.0f, 0.0f}, 90.0f, 0.0f, 90.0f);
    calculatePlayerData(&player);

    open_simplex_noise(283458, &osn_ctx);

    return true;
};

bool engineExit()
{
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &texture1);

    for(khint_t i = kh_begin(chunkp); i != kh_end(chunkMap); ++i)
    {
        if(kh_exist(chunkMap, i))
        {
            Chunk* chunk = kh_value(chunkMap, i);

            glDeleteBuffers(1, &chunk->VBO);
            glDeleteVertexArrays(1, &chunk->VAO);

            free(chunk);
        }
    }

    kh_destroy(chunk_map, chunkMap);

    open_simplex_noise_free(osn_ctx);

    return true;
};

void processEngineMouseInput(double xPos, double yPos)
{
    processPlayerMouseInput(&player, xPos, yPos);
};

void processEngineKeyboardInput(int key, int action)
{
    processPlayerKeyboardInput(&player, key, action);
};
