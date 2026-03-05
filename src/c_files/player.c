#include "player.h"

#include <stdbool.h>
#include <linmath.h>
#include <GLFW/glfw3.h>
#include "camera.h"
#include "settings.h"
#include "math_help_functions.h"

void updatePos(Player* player, bool key, float* dir, float speed)
{
    if(key)
    {
        vec3 add_pos;
        vec3_scale(add_pos, dir, speed);
        vec3_add(player->camera.pos, player->camera.pos, add_pos);
    }
};

void playerUpdate(Player* player)
{
    if(player->mouseFirstMove)
    {
        player->mouseLastX = player->mouseX;
        player->mouseLastY = player->mouseY;
        player->mouseFirstMove = false;
    }

    float xOffset = player->mouseX - player->mouseLastX;
    float yOffset = player->mouseLastY - player->mouseY;

    player->mouseLastX = player->mouseX;
    player->mouseLastY = player->mouseY;

    xOffset *= MOUSE_INPUT_SENSITIVITY;
    yOffset *= MOUSE_INPUT_SENSITIVITY;


    player->camera.yaw += xOffset;
    player->camera.pitch += yOffset;

    if(player->camera.pitch >= 90.0f)
    {
        player->camera.pitch = 89.9f;
    }

    if(player->camera.pitch <= -90.0f)
    {
        player->camera.pitch = -89.9f;
    }

    vec3 XZdir;
    XZdir[0] = player->dir[0];
    XZdir[1] = 0.0f;
    XZdir[2] = player->dir[2];
    vec3_norm(XZdir, XZdir);

    updatePos(player, player->W, XZdir, PLAYER_SPEED);
    updatePos(player, player->S, XZdir, -PLAYER_SPEED);
    updatePos(player, player->A, player->right, PLAYER_SPEED);
    updatePos(player, player->D, player->right, -PLAYER_SPEED);
    updatePos(player, player->SPACE, absoluteUp, PLAYER_SPEED);
    updatePos(player, player->SHIFT, absoluteUp, -PLAYER_SPEED);
};

bool playerInit(Player* player, float* pos, float yaw, float pitch, float FOV)
{
    player->camera.pos[0] = pos[0];
    player->camera.pos[1] = pos[1];
    player->camera.pos[2] = pos[2];

    player->camera.yaw = yaw;
    player->camera.pitch = pitch;
    player->camera.FOV = FOV;

    player->mouseFirstMove = true;

    return true;
};

void calculatePlayerData(Player* player)
{
    cameraDir(&player->camera, player->dir);
    cameraRight(player->dir, player->right);
    cameraTarget(player->camera.pos, player->dir, player->target);
};

void processPlayerMouseInput(Player* player, float xPos, float yPos)
{
    player->mouseX = xPos;
    player->mouseY = yPos;
};

void setKeyBool(bool* keyBool, int key, int targetedKey, int action)
{
    if(key == targetedKey && action == GLFW_PRESS)
    {
        *keyBool = true;
    }
    else if(key == targetedKey && action == GLFW_RELEASE)
    {
        *keyBool = false;
    }
};

void processPlayerKeyboardInput(Player* player, int key, int action)
{
    setKeyBool(&player->W, key, GLFW_KEY_W, action);
    setKeyBool(&player->S, key, GLFW_KEY_S, action);
    setKeyBool(&player->A, key, GLFW_KEY_A, action);
    setKeyBool(&player->D, key, GLFW_KEY_D, action);
    setKeyBool(&player->SPACE, key, GLFW_KEY_SPACE, action);
    setKeyBool(&player->SHIFT, key, GLFW_KEY_LEFT_SHIFT, action);
};
