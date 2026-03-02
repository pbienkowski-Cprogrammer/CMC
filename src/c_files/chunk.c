#include "chunk.h"

#include "settings.h"
#include "game_data.h"
#include "math_help_functions.h"
#include <open-simplex-noise.h>
#include <linmath.h>
#include <stb/stb_ds.h>

struct osn_context *osn_ctx;

void addSide(Chunk* chunk, Sides side, float* blockPos, int index)
{
    for(int i = 0; i < 6; i++)
    {
        int currentStride = (side * 6 + i) * BLOCK_DATA_STRIDE;

        //vertex pos
        arrpush(chunk->drawData, blockData[currentStride + 0] + chunk->pos[0] * CHUNK_SIZE + blockPos[0]);
        arrpush(chunk->drawData, blockData[currentStride + 1] + blockPos[1]);
        arrpush(chunk->drawData, blockData[currentStride + 2] + chunk->pos[1] * CHUNK_SIZE + blockPos[2]);

        //normal
        arrpush(chunk->drawData, blockData[currentStride + 3]);
        arrpush(chunk->drawData, blockData[currentStride + 4]);
        arrpush(chunk->drawData, blockData[currentStride + 5]);

        //tex coords
        int blockIndex = chunk->chunkData[index] - 1;
        int textureIndex = blockSidesTextures[blockIndex][side];

        int texturePos = blockData[currentStride + 6] + texPosis[textureIndex][0];
        float normalizedTexturePos = (float)texturePos / (float)TEXTURES_IN_ATLAS_WIDTH;

        float textureCorrection = 0.0001f;

        if(blockData[currentStride + 6] > 0.0f)
        {
            textureCorrection *= -1.0f;
        }

        arrpush(chunk->drawData, normalizedTexturePos + textureCorrection);

        // texturePos = blockData[currentStride + 7] + texPosis[textureIndex][1];
        // normalizedTexturePos = (float)texturePos / (float)TEXTURES_IN_ATLAS_HEIGHT;
        //
        // textureCorrection = 0.0001f;
        //
        // if(blockData[currentStride + 7] > 0.0f)
        // {
        //     textureCorrection *= -1.0f;
        // }
        //
        // arrpush(chunk->drawData, normalizedTexturePos + textureCorrection);

        float yTexCoord = (blockData[currentStride + 7] + ((TEXTURES_IN_ATLAS_HEIGHT - 1) - texPosis[blockSidesTextures[chunk->chunkData[index] - 1][side]][1])) / TEXTURES_IN_ATLAS_HEIGHT - (blockData[(side * 6 + i) * 9 + 7] == 0.0f ? -0.0001f : 0.0001f);
        arrpush(chunk->drawData, yTexCoord);

        //face side index
        arrpush(chunk->drawData, blockData[currentStride + 8]);
    }
};

void generateChunk(Chunk* chunk)
{
    for(int x = 0; x < CHUNK_SIZE; x++)
    {
        for(int z = 0; z < CHUNK_SIZE; z++)
        {
            float multiplyer = 0.01f;
            int height = CHUNK_HEIGHT * (open_simplex_noise2(osn_ctx, (x + chunk->pos[0] * CHUNK_SIZE) * multiplyer, (z + chunk->pos[1] * CHUNK_SIZE) * multiplyer) * 0.5f + 0.5f);

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
                    chunk->chunkData[index] = SAND_BLOCK;
                }
                else
                {
                    chunk->chunkData[index] = STONE_BLOCK;
                }
            }
        }
    }
};

void generateChunkDrawData(Chunk* chunk, Chunk* westChunk, Chunk* eastChunk, Chunk* southChunk, Chunk* northChunk)
{
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
};

void compileChunkDrawData(Chunk* chunk)
{
    // uint64_t key;
    // vec2ToHashKey(chunkX, chunkZ, &key);
    //
    // khint_t foundChunk = kh_get(chunk_map, chunkMap, key);
    // Chunk* chunk = kh_value(chunkMap, foundChunk);



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
};
