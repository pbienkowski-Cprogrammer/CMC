#include "game.h"

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <tinycthread.h>
#include "settings.h"
#include "window.h"
#include "voxel_engine.h"

const long timePerUpdate = 1000000000 / UPS;
const long timePerFrame = 1000000000 / FPS;

bool gameRunning = true;

long deltaU = 0;
long deltaF = 0;

long lastCheckTimeMilis = 0;

long currentNanoTime = 0;
long previousNanoTime = 0;

long countUPS = 0;
long countFPS = 0;

bool isDraw = true;

bool escapePressed = false;

mtx_t gameLoopMutex;

void update()
{
    if(escapePressed)
    {
        glfwSetWindowShouldClose(window, true);
    }

    voxelEngineUpdate();
};

void draw()
{
    voxelEngineDraw();
};

int gameLogicLoop(void* arg)
{
    previousNanoTime = glfwGetTime() * 1000000000;
    lastCheckTimeMilis = glfwGetTime() * 1000;

    while(true)
    {
        mtx_lock(&gameLoopMutex);
        if(!gameRunning)
        {
            break;
        }
        mtx_unlock(&gameLoopMutex);

        currentNanoTime = glfwGetTime() * 1000000000;
        deltaU += currentNanoTime - previousNanoTime;
        deltaF += currentNanoTime - previousNanoTime;
        previousNanoTime = currentNanoTime;

        if(deltaF >= timePerFrame)
        {
            deltaF -= timePerFrame;

            mtx_lock(&gameLoopMutex);
            isDraw = true;
            mtx_unlock(&gameLoopMutex);
        }

        if(deltaU >= timePerUpdate)
        {
            deltaU -= timePerUpdate;

            mtx_lock(&gameLoopMutex);

            update();
            countUPS++;

            mtx_unlock(&gameLoopMutex);
        }

        if(glfwGetTime() * 1000 - lastCheckTimeMilis >= 1000)
        {
            printf("UPS : %ld | FPS : %ld\n", countUPS, countFPS);
            countUPS = 0;
            countFPS = 0;
            lastCheckTimeMilis += 1000;
        }
    }

    return 0;
};

bool gameInit()
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to load glad\n");
        return false;
    }

    voxelEngineInit();

    return true;
};

bool startGameLoop()
{
    mtx_init(&gameLoopMutex, mtx_plain);

    thrd_t gameThread;
    thrd_create(&gameThread, gameLogicLoop, NULL);

    while(!glfwWindowShouldClose(window))
    {
        mtx_lock(&gameLoopMutex);
        if(isDraw)
        {
            mtx_unlock(&gameLoopMutex);

            draw();

            glfwSwapBuffers(window);
            mtx_lock(&gameLoopMutex);
            isDraw = false;
            countFPS++;
        }
        mtx_unlock(&gameLoopMutex);

        glfwPollEvents();
    }

    mtx_lock(&gameLoopMutex);
    gameRunning = false;
    mtx_unlock(&gameLoopMutex);

    voxelEngineExit();
    thrd_join(gameThread, NULL);
    mtx_destroy(&gameLoopMutex);

    glfwTerminate();

    return true;
};

void processGameMouseInput(double xPos, double yPos)
{
    processVoxelEngineMouseInput(xPos, yPos);
};

void processGameKeyboardInput(int key, int action)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        escapePressed = true;
    }

    processVoxelEngineKeyboardInput(key, action);
};
