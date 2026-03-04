#include "window.h"
#include "game.h"
#include <stdio.h>

int main()
{
    if(!createWindow())
    {
        printf("Failed to create window\n");
        return 1;
    }

    if(!initGame())
    {
        printf("Failed to init game\n");
        return 1;
    }

    if(!startGameLoop())
    {
        printf("Failed to start game loop\n");
        return 1;
    }

    return 0;
}
