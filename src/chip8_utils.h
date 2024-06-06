#ifndef CHIP8_UTILS_H_
#define CHIP8_UTILS_H_

#include <stdbool.h>
#include <stdint.h>
#include "constant.h"

typedef struct chip8
{
    uint8_t ram[MEMORY_SIZE];
    uint8_t V[16];
    uint16_t I;
    uint8_t timer_register;
    uint8_t delay_register;
    uint16_t pc; // Program counter
    uint8_t sp; // Stack pointer
    uint16_t stack[16];
    bool display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
} chip8;

chip8 *chip8_init();
int chip8_load_rom(chip8* chip8, const char* filename);
void chip8_loop(chip8* chip8);

#endif //CHIP8_UTILS_H_