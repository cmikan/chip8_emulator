#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <SDL2/SDL.h>

int display_init(SDL_Window** window);
int display_delete(SDL_Window* window);

void *display_loop(void *arg);

#endif //DISPLAY_H_