#include <SDL2/SDL.h>
#include <pthread.h>
#include "display.h"

int main(int argc, char **argv)
{
    SDL_Window* display_window;
    display_init(&display_window);

    pthread_t display_thread;
    pthread_create(&display_thread, NULL, display_loop, 0);
    
    pthread_join (display_thread, 0);
    display_delete(display_window);

    return 0;
}