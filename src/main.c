#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#define PIXEL_SIZE 16
#define REFRESH_RATE 60

#define DELAY 1000 / REFRESH_RATE

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "Error on SDL initialization : %s", SDL_GetError());
        return 1;
    }

    SDL_Window* display = SDL_CreateWindow("Chip-8 emulator", 100, 100, 64 * PIXEL_SIZE, 32 * PIXEL_SIZE, 0);
    if (display == NULL)
    {
        fprintf(stderr, "Error on window creation : %s", SDL_GetError());
        return 1;
    }

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

    SDL_DestroyWindow(display);
    SDL_Quit();
    return 0;
}