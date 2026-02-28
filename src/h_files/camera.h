#ifndef CAMERA_H
#define CAMERA_H

#include <linmath.h>

typedef struct Camera
{
    vec3 pos;
    float FOV;
    float pitch;
    float yaw;
} Camera;

void cameraDir(Camera* camera, float* cameraDir);
void cameraRight(float* cameraDir, float* cameraRight);
void cameraUp(float* cameraDir, float* cameraRight, float* cameraUp);
void cameraTarget(float* cameraPos, float* cameraDir, float* cameraTarget);

#endif
