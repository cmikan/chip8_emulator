#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
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

chip8 *chip8_init()
{
    chip8 *return_value = (chip8*)malloc(sizeof(chip8));
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        return_value->ram[i] = 0;
    }

    for (int i = 0; i < 16; i++)
    {
        return_value->V[i] = 0;
        return_value->stack[i] = 0;
    }

    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++)
    {
        return_value->display[i] = 0;
    }
    return_value->timer_register = 0;
    return_value->delay_register = 0;
    return_value->pc = 0;
    return_value->sp = 0;

    return return_value;
}