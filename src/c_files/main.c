#include "window.h"
#include "game.h"
#include <stdio.h>

int main()
{
    if(!createWindow())
    {
        printf("Failed to create window\n");
    }

    if(!initGame())
    {
        printf("Failed to init game\n");
    }

    if(!startGameLoop())
    {
        printf("Failed to start game loop\n");
    }

    return 0;
}
