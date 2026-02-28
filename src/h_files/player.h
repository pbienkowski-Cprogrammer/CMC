#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include "camera.h"

typedef struct Player
{
    Camera camera;
    vec3 dir;
    vec3 right;
    vec3 target;

    float mouseX;
    float mouseY;
    float mouseLastX;
    float mouseLastY;
    bool mouseFirstMove;

    bool W;
    bool A;
    bool S;
    bool D;
    bool SPACE;
    bool SHIFT;
} Player;

void playerUpdate(Player* player);
bool initPlayer(Player* player, float* pos, float yaw, float pitch, float FOV);
void calculatePlayerData(Player* player);
void processPlayerMouseInput(Player* player, float xPos, float yPos);
void processPlayerKeyboardInput(Player* player, int key, int action);

#endif
