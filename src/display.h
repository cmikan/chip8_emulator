#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <SDL2/SDL.h>
#include <stdbool.h>

void *display_init(bool display[]);
int display_delete(void* window);

void *display_loop(void *arg);

#endif //DISPLAY_H_