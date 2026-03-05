#ifndef VOXEL_ENGINE_H
#define VOXEL_ENGINE_H

#include <stdbool.h>

void voxelEngineUpdate();
void voxelEngineDraw();
bool voxelEngineInit();
bool voxelEngineExit();
void processVoxelEngineMouseInput(double xPos, double yPos);
void processVoxelEngineKeyboardInput(int key, int action);

#endif
