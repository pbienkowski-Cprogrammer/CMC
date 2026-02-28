#ifndef VOXEL_ENGINE_H
#define VOXEL_ENGINE_H

#include <stdbool.h>

void engineUpdate();
void engineDraw();
bool initEngine();
bool engineExit();
void processEngineMouseInput(double xPos, double yPos);
void processEngineKeyboardInput(int key, int action);

#endif
