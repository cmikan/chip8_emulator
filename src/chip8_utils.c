#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

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
    return_value->display[1000] = 1;
    return_value->timer_register = 0;
    return_value->delay_register = 0;
    return_value->pc = 0x200;
    return_value->sp = 0;

    return return_value;
}

int chip8_load_rom(chip8* chip8, const char* filename)
{
    FILE *file = NULL;
    file = fopen(filename, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Error: File doesn't exist.\n");
        return 1;
    }

    int bytes_read = fread(chip8->ram + 0x200, 1, MEMORY_SIZE - 0x200, file);
    if (bytes_read > MEMORY_SIZE - 0x200)
    {
        fprintf(stderr, "Error: File is too large.\n");
        return 1;
    }

    fclose(file);
    return 0;
}

void chip8_loop(chip8* chip8, bool *quit)
{
    struct timeval start_time, end_time;
    long int elapsed_ms;

    while (!(*quit))
    {
        gettimeofday(&start_time, NULL);

        //printf("Test\n");

        gettimeofday(&end_time, NULL);
        elapsed_ms = (end_time.tv_sec - start_time.tv_sec) * 1000 + (end_time.tv_usec - start_time.tv_usec) / 1000;
        if (elapsed_ms < REFRESH_RATE)
        {
            usleep((REFRESH_RATE - elapsed_ms) * 1000);
        }
    }
}