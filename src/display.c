#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

#include "constant.h"

typedef struct intern_display
{
    SDL_Window *window;
    SDL_Renderer* renderer;
    pthread_t thread;
} intern_display;

typedef struct intern_loop
{
    SDL_Renderer* renderer;
    bool *keyboard;
    bool *display;
    bool *quit;
} intern_loop;

int keyboard_helper(int scancode)
{
    switch (scancode)
    {
        case 30: // First row
            return 0;
        case 31:
            return 1;
        case 32:
            return 2;
        case 33:
            return 3;
        case 20: // Second row
            return 4;
        case 26:
            return 5;
        case 8:
            return 6;
        case 21:
            return 7;
        case 4: // Third row
            return 8;
        case 22:
            return 9;
        case 7:
            return 10;
        case 9:
            return 11;
        case 29: // Fourth row
            return 12;
        case 27:
            return 13;
        case 6:
            return 14;
        case 25:
            return 15;
        default:
            return 16;
    }
}

void *display_loop(void *arg)
{
    intern_loop *intern = (intern_loop*)arg;
    bool *keyboard = intern->keyboard;
    bool *quit = intern->quit;
    bool *display = intern->display;
    SDL_Renderer* renderer = intern->renderer;
    free(intern);

    SDL_Event event;
    while(!(*quit))
    {
        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                {
                    (*quit) = true;
                    break;
                }
                case SDL_KEYDOWN:
                {
                    int key_index = keyboard_helper(event.key.keysym.scancode);
                    if (key_index != 16)
                    {
                        keyboard[key_index] = true;
                    }
                    break;
                }
                case SDL_KEYUP:
                {
                    int key_index = keyboard_helper(event.key.keysym.scancode);
                    if (key_index != 16)
                    {
                        keyboard[key_index] = false;
                    }
                    break;
                }
                default:
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR, 1);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, FOREGROUND_COLOR, 1);

        for (int y = 0; y < DISPLAY_HEIGHT; ++y)
        {
            for (int x = 0; x < DISPLAY_WIDTH; ++x)
            {
                if (display[y * DISPLAY_WIDTH + x])
                {
                    SDL_Rect rect = { x * PIXEL_SIZE, y * PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE };
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(DELAY);
    }
    return (void*) 0;
}

void *display_init(bool display[], bool* quit, bool *keyboard)
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

    return_value->renderer = SDL_CreateRenderer(return_value->window , -1, SDL_RENDERER_ACCELERATED);
    if (return_value->renderer == NULL)
    {
        fprintf(stderr, "Error on renderer creation : %s", SDL_GetError());
        return NULL;
    }

    intern_loop *args = (intern_loop*)malloc(sizeof(intern_loop));
    args->keyboard = keyboard;
    args->display = display;
    args->quit = quit;
    args->renderer = return_value->renderer;

    pthread_create(&(return_value->thread), NULL, display_loop, (void*)(args));

    return (void*)return_value;
}

int display_delete(void* window)
{
    intern_display *display = (intern_display*)window;
    pthread_join((pthread_t)display->thread, 0);
    SDL_DestroyRenderer(display->renderer);
    SDL_DestroyWindow(display->window);
    SDL_Quit();
    return 0;
}