#include <glad.h>
#include "voxel_engine.h"

#include <stdio.h>
#include <stdbool.h>
#include "window.h"
#include "opengl_help_functions.h"
#include "math_help_functions.h"
#include "player.h"
#include "chunk_manager.h"


/*
 TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 CHANGE to voxelengineINIT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 also with other things like player camera etc.
 make a chunk draw function
 tweak with settings
 change Sides to Side
 put seed for open simplex noise in settings, and change file name to just noise instead of open simplex noise
 realistic movement, i mean if i press w and a, i should be the same speeed as i would press w, not times sqrt(2)
 comment everything








 */

GLuint shaderProgram;
GLuint texture_atlas;

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

    chunkManagerDraw(player.camera.pos[0], player.camera.pos[2]);
};

bool initEngine()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    if(!createShader(&shaderProgram, "src/shader_files/vert.glsl", "src/shader_files/frag.glsl"))
    {
        printf("failed to create shader\n");
        return false;
    }

    if(!createTexture2D(&texture_atlas, "res/texture_atlas.png"))
    {
        printf("failed to create texture\n");
        return false;
    }

    activateTexture2D(texture_atlas, GL_TEXTURE0);

    glUseProgram(shaderProgram);

    glUniform1i(uniLoc(shaderProgram, "texture_atlas"), 0);

    initPlayer(&player, (vec3){0.0f, 100.0f, 0.0f}, 90.0f, 0.0f, 90.0f);
    calculatePlayerData(&player);

    chunkManagerInit();

    return true;
};

bool engineExit()
{
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &texture_atlas);

    chunkManagerExit();

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
