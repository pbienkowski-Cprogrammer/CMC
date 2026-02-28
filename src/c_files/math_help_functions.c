#include "math_help_functions.h"

#include <stdio.h>
#include <stdint.h>
#include <linmath.h>
#include "settings.h"

vec3 absoluteUp = (vec3){0.0f, 1.0f, 0.0f};

void print_vec3(float* target)
{
    printf("%f %f %f\n", target[0], target[1], target[2]);
};

void indexToPos(int index, float* dest)
{
    if(index < 0 || index > CHUNK_VOLUME)
    {
        printf("chunk block index is out of range\n");
        return;
    }

    dest[0] = (float)(index % CHUNK_SIZE);
    dest[1] = (float)(index - index % CHUNK_AREA) / CHUNK_AREA;
    dest[2] = (float)((index - index % CHUNK_SIZE) % CHUNK_AREA) / CHUNK_SIZE;
};

void posToIndex(float* pos, int* index)
{
    if(pos[0] < 0 || pos[0] >= CHUNK_SIZE || pos[1] < 0 || pos[1] >= CHUNK_HEIGHT || pos[2] < 0 || pos[2] >= CHUNK_SIZE)
    {
        printf("chunk block pos is out of chunk\n");
        return;
    }

    *index = pos[0];
    *index += pos[1] * CHUNK_AREA;
    *index += pos[2] * CHUNK_SIZE;
}

float degToRad(float deg)
{
    return deg * (M_PI / 180.0f);
};

void vec2ToHashKey(int x, int z, uint64_t* key)
{
    *key = ((uint64_t)(uint32_t)x << 32) | (uint32_t)z;
}
