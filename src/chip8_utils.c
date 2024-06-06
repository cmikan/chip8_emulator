#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "chip8_utils.h"
#include "constant.h"

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

void chip8_load_rom(chip8* chip8, const char* filename)
{
    FILE *file = NULL;
    file = fopen("test_opcode.ch8", "rb");
    fread(chip8->ram + 0x200, 1, MEMORY_SIZE - 0x200, file);
    fclose(file);
}