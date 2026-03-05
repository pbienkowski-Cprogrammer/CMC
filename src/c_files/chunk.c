#include "chunk.h"

#include "settings.h"
#include "game_data.h"
#include "math_help_functions.h"
#include <open-simplex-noise.h>
#include <linmath.h>
#include <stb/stb_ds.h>

struct osn_context *osn_ctx;

void chunkDraw(Chunk* chunk)
{
    glBindVertexArray(chunk->VAO);
    glDrawArrays(GL_TRIANGLES, 0, chunk->vertices);
}

//this function push clock face data to chunk.drawData, those are data to create VBO, VAO and just draw chunk
void addBlockSide(Chunk* chunk, Sides side, float* blockPos, int index)
{
    for(int i = 0; i < 6; i++)
    {
        //on which stride from blockData we are
        int currentStride = (side * 6 + i) * BLOCK_DATA_STRIDE;

        //there we add those data with optionally some thing to them

        //vertex pos with chunk offset
        arrpush(chunk->drawData, blockData[currentStride + 0] + chunk->pos[0] * CHUNK_SIZE + blockPos[0]);
        arrpush(chunk->drawData, blockData[currentStride + 1] + blockPos[1]);
        arrpush(chunk->drawData, blockData[currentStride + 2] + chunk->pos[1] * CHUNK_SIZE + blockPos[2]);

        //normals
        arrpush(chunk->drawData, blockData[currentStride + 3]);
        arrpush(chunk->drawData, blockData[currentStride + 4]);
        arrpush(chunk->drawData, blockData[currentStride + 5]);

        //tex coords

        int blockIndex = chunk->chunkData[index] - 1;
        int textureIndex = blockSidesTextures[blockIndex][side];

        //there we treat texturePos as pos on tile map and later we normalize it for shader program
        int texturePos = blockData[currentStride + 6] + texPosis[textureIndex][0];
        float normalizedTexturePos = (float)texturePos / (float)TEXTURES_IN_ATLAS_WIDTH;

        //there we move edges of texture slighty to direction of the center,
        //because if we dont, there will be neightbour texture pixel color, where they shouldnt be,
        //its because we store textures in one atlas
        float textureCorrection = 0.0001f;

        if(blockData[currentStride + 6] > 0.0f)
        {
            textureCorrection *= -1.0f;
        }

        //we combine those two and that is final result
        arrpush(chunk->drawData, normalizedTexturePos + textureCorrection);

        //there everything is like above, but texture pos is different, because for opengl
        //image starts at bottom left center and for us image starts at top left
        texturePos = blockData[currentStride + 7] + TEXTURES_IN_ATLAS_HEIGHT - 1 - texPosis[textureIndex][1];
        normalizedTexturePos = (float)texturePos / (float)TEXTURES_IN_ATLAS_HEIGHT;

        textureCorrection = 0.0001f;

        if(blockData[currentStride + 7] > 0.0f)
        {
            textureCorrection *= -1.0f;
        }

        arrpush(chunk->drawData, normalizedTexturePos + textureCorrection);

        //face side index, it might be useful
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

void checkNegativeSideOfBlock(Chunk* currentChunk, Chunk* neighbourChunk, float* blockPos, int index, Sides side, int axe)
{
    //we make temporary version of variables
    vec3 tempPos;
    vec3_set(blockPos, tempPos);

    int tempIndex = index;

    //at first we check if we will be checking inside or outside of chunk
    if(tempPos[axe] - 1 < 0)
    {
        //there we get pos of touching block of neightbour chunk and if its the air block we add face to be drawn
        tempPos[axe] = CHUNK_SIZE - 1;
        posToIndex(tempPos, &tempIndex);

        if(neighbourChunk->chunkData[tempIndex] == AIR_BLOCK)
        {
            addBlockSide(currentChunk, side, blockPos, index);
        }

        tempPos[axe] = 0.0f;
    }
    else
    {
        //inside the chunk we do the same as the outside
        tempPos[axe]--;
        posToIndex(tempPos, &tempIndex);

        if(currentChunk->chunkData[tempIndex] == AIR_BLOCK)
        {
            addBlockSide(currentChunk, side, blockPos, index);
        }

        tempPos[axe]++;
    }
}

void checkPositiveSideOfBlock(Chunk* currentChunk, Chunk* neighbourChunk, float* blockPos, int index, Sides side, int axe)
{
    //the we do the same as in the method above, but in other way
    vec3 tempPos;
    vec3_set(blockPos, tempPos);

    int tempIndex = index;

    if(tempPos[axe] + 1 >= CHUNK_SIZE)
    {
        tempPos[axe] = 0.0f;
        posToIndex(tempPos, &tempIndex);

        if(neighbourChunk->chunkData[tempIndex] == AIR_BLOCK)
        {
            addBlockSide(currentChunk, side, blockPos, index);
        }

        tempPos[axe] = CHUNK_SIZE - 1;
    }
    else
    {
        tempPos[axe]++;
        posToIndex(tempPos, &tempIndex);

        if(currentChunk->chunkData[tempIndex] == AIR_BLOCK)
        {
            addBlockSide(currentChunk, side, blockPos, index);
        }

        tempPos[axe]--;
    }
}

void generateChunkDrawData(Chunk* chunk, Chunk* westChunk, Chunk* eastChunk, Chunk* southChunk, Chunk* northChunk)
{
    chunk->drawData = NULL;
    for(int index = 0; index < CHUNK_VOLUME; index++)
    {
        vec3 blockPos;
        indexToPos(index, blockPos);

        if(chunk->chunkData[index] > 0)
        {
            //WEST
            checkNegativeSideOfBlock(chunk, westChunk, blockPos, index, WEST_SIDE, 0);

            //SOUTH
            checkPositiveSideOfBlock(chunk, southChunk, blockPos, index, SOUTH_SIDE, 2);

            //EAST
            checkPositiveSideOfBlock(chunk, eastChunk, blockPos, index, EAST_SIDE, 0);

            //NORTH
            checkNegativeSideOfBlock(chunk, northChunk, blockPos, index, NORTH_SIDE, 2);

            //we make temporary version of variables
            vec3 tempPos;
            int tempIndex;
            tempPos[0] = blockPos[0];
            tempPos[1] = blockPos[1];
            tempPos[2] = blockPos[2];

            //TOP
            //there everything is same like above, but we dont check up and down chunk because we dont have them
            if(tempPos[1] + 1 >= CHUNK_HEIGHT)
            {
                addBlockSide(chunk, TOP_SIDE, blockPos, index);
            }
            else
            {
                tempPos[1]++;
                posToIndex(tempPos, &tempIndex);

                if(chunk->chunkData[tempIndex] == AIR_BLOCK)
                {
                    addBlockSide(chunk, TOP_SIDE, blockPos, index);
                }

                tempPos[1]--;
            }

            //BOTTOM
            if(tempPos[1] - 1 < 0)
            {
                addBlockSide(chunk, BOTTOM_SIDE, blockPos, index);
            }
            else
            {
                tempPos[1]--;
                posToIndex(tempPos, &tempIndex);

                if(chunk->chunkData[tempIndex] == AIR_BLOCK)
                {
                    addBlockSide(chunk, BOTTOM_SIDE, blockPos, index);
                }

                tempPos[1]++;
            }
        }
    }

    //and we pass number of vertices to chunk, for more optimized rendering, and we check chunk as draw data generated
    chunk->vertices = arrlen(chunk->drawData) / 6;
    chunk->generatedDrawData = true;
};

void compileChunkDrawData(Chunk* chunk)
{
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
