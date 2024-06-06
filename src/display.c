#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

#include "constant.h"

typedef struct intern_display
{
    SDL_Window *window;
    pthread_t thread;
} intern_display;

void *display_loop(void *arg)
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

void *display_init(bool display[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "Error on SDL initialization : %s", SDL_GetError());
        return NULL;
    }

    intern_display *return_value = (intern_display*) malloc(sizeof(intern_display));

    return_value->window = SDL_CreateWindow("Chip-8 emulator", 100, 100, 64 * PIXEL_SIZE, 32 * PIXEL_SIZE, 0);
    if (return_value->window == NULL)
    {
        fprintf(stderr, "Error on window creation : %s", SDL_GetError());
        return NULL;
    }

    pthread_create(&(return_value->thread), NULL, display_loop, (void*)(&display));

    return (void*)return_value;
}

int display_delete(void* window)
{
    intern_display *display = (intern_display*)window;
    pthread_join((pthread_t)display->thread, 0);
    SDL_DestroyWindow(display->window);
    SDL_Quit();
    return 0;
}