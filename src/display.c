#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#define PIXEL_SIZE 16
#define REFRESH_RATE 60

#define DELAY 1000 / REFRESH_RATE

int display_init(SDL_Window** window)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "Error on SDL initialization : %s", SDL_GetError());
        return 1;
    }

    *window = SDL_CreateWindow("Chip-8 emulator", 100, 100, 64 * PIXEL_SIZE, 32 * PIXEL_SIZE, 0);
    if (*window == NULL)
    {
        fprintf(stderr, "Error on window creation : %s", SDL_GetError());
        return 1;
    }

    return 0;
}

int display_delete(SDL_Window* window)
{
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void *display_loop()
{
    SDL_Event event;
    bool quit = false;
    while(!quit)
    {
        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                {
                    quit = true;
                }
            }
        }
        SDL_Delay(DELAY);
    }
    return (void*) 0;
}