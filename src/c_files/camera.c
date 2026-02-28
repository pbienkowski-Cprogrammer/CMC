#include "camera.h"

#include <linmath.h>
#include "math_help_functions.h"

void cameraDir(Camera* camera, float* cameraDir)
{
    cameraDir[0] = cos(degToRad(camera->yaw)) * cos(degToRad(camera->pitch));
    cameraDir[1] = sin(degToRad(camera->pitch));
    cameraDir[2] = sin(degToRad(camera->yaw)) * cos(degToRad(camera->pitch));
    vec3_norm(cameraDir, cameraDir);
};

void cameraRight(float* cameraDir, float* cameraRight)
{
    vec3_mul_cross(cameraRight, (vec3){0.0f, 1.0f, 0.0f},cameraDir);
    vec3_norm(cameraRight, cameraRight);
};

void cameraUp(float* cameraDir, float* cameraRight, float* cameraUp)
{
    vec3_mul_cross(cameraUp, cameraDir, cameraRight);
};

void cameraTarget(float* cameraPos, float* cameraDir, float* cameraTarget)
{
    vec3_add(cameraTarget, cameraPos, cameraDir);
};
