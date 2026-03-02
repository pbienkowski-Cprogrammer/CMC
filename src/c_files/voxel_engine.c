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
#include "chunk.h"


/*
 TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 fix monstrocities in voxelEngine.c, especially textures spagetti in add side
 also make an something chunkmanager like
 realistic movement, i mean if i press w and a, i should be the same speeed as i would press w, not times sqrt(2)








 */

GLuint shaderProgram;
GLuint texture1;

Player player;






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

                if(!chunk->generatedDrawData)
                {
                    generateChunkDrawData(x, z);
                    compileChunkDrawData(x, z);
                }


                glBindVertexArray(chunk->VAO);
                glDrawArrays(GL_TRIANGLES, 0, chunk->vertices);
            }
        }
    }
};

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
