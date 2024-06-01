#include <SDL2/SDL.h>
#include <pthread.h>
#include "display.h"

#define MEMORY_SIZE 4096

int main(int argc, char **argv)
{
    uint8_t *memory = (uint8_t*)malloc(MEMORY_SIZE * sizeof(uint8_t));
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        memory[i] = 0;
    }

    SDL_Window* display_window;
    display_init(&display_window);

    pthread_t display_thread;
    pthread_create(&display_thread, NULL, display_loop, (void*)(memory + MEMORY_SIZE - 256)); // Pointer to the beginning of the display zone

    pthread_join (display_thread, 0);
    display_delete(display_window);
    free(memory);

    return 0;
}