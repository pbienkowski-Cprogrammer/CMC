#ifndef CAMERA_H
#define CAMERA_H

#include <linmath.h>

//I decided to put in camera struct only necesery stuff
typedef struct Camera
{
    vec3 pos;
    float FOV;
    float pitch;
    float yaw;
} Camera;

//basic camera utility - functions for other things
void cameraDir(Camera* camera, float* cameraDir);
void cameraRight(float* cameraDir, float* cameraRight);
void cameraUp(float* cameraDir, float* cameraRight, float* cameraUp);
void cameraTarget(float* cameraPos, float* cameraDir, float* cameraTarget);

#endif
