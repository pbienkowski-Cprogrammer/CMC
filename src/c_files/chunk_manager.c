#include "chunk_manager.h"

#include "settings.h"
#include "math_help_functions.h"
#include "chunk.h"
#include <open-simplex-noise.h>
#include <khash.h>

KHASH_MAP_INIT_INT64(chunk_map, Chunk*)
khash_t(chunk_map) *chunkMap;

void chunkManagerDraw(float playerPosX, float playerPosZ)
{
    float currentChunkX = playerPosX / (float)CHUNK_SIZE;
    float currentChunkZ = playerPosZ / (float)CHUNK_SIZE;

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

    for(int chunkX = startChunkX - 1; chunkX <= endChunkX + 1; chunkX++)
    {
        for(int chunkZ = startChunkZ - 1; chunkZ <= endChunkZ + 1; chunkZ++)
        {
            uint64_t key;
            vec2ToHashKey(chunkX, chunkZ, &key);

            int ret;
            khint_t foundChunk = kh_put(chunk_map, chunkMap, key, &ret);

            if(ret)
            {
                Chunk* chunk = malloc(sizeof(Chunk));
                chunk->pos[0] = chunkX;
                chunk->pos[1] = chunkZ;
                chunk->generatedDrawData = false;

                kh_value(chunkMap, foundChunk) = chunk;

                generateChunk(chunk);
            }
        }
    }

    for(int chunkX = startChunkX; chunkX <= endChunkX; chunkX++)
    {
        for(int chunkZ = startChunkZ; chunkZ <= endChunkZ; chunkZ++)
        {
            uint64_t key;
            vec2ToHashKey(chunkX, chunkZ, &key);

            khint_t foundChunk = kh_get(chunk_map, chunkMap, key);

            if(foundChunk != kh_end(chunkMap))
            {
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

                if(!chunk->generatedDrawData)
                {
                    generateChunkDrawData(chunk, westChunk, eastChunk, southChunk, northChunk);
                    compileChunkDrawData(chunk);
                }

                chunkDraw(chunk);
            }
        }
    }
};

void chunkManagerInit()
{
    chunkMap = kh_init(chunk_map);
    open_simplex_noise(283458, &osn_ctx);
};

void chunkManagerExit()
{
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
};
