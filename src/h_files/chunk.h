#ifndef CHUNK_H
#define CHUNK_H

#include "settings.h"

#include <glad.h>
#include <linmath.h>
#include <stdbool.h>

extern struct osn_context *osn_ctx;

typedef struct Chunk
{
    vec2 pos;
    uint8_t chunkData[CHUNK_VOLUME];
    float* drawData;
    GLuint VBO, VAO;
    bool generatedDrawData;
    unsigned int vertices;
} Chunk;

void chunkDraw(Chunk* chunk);
void generateChunk(Chunk* chunk);
void generateChunkDrawData(Chunk* chunk, Chunk* westChunk, Chunk* eastChunk, Chunk* southChunk, Chunk* northChunk);
void compileChunkDrawData(Chunk* chunk);

#endif
