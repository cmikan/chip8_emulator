#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <SDL2/SDL.h>

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
    return_value->delay_timer = 0;
    return_value->sound_timer = 0;
    return_value->pc = 0x200;
    return_value->sp = 0;

    srand(time(NULL));

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

void chip8_opcode_handler(chip8* chip8, uint16_t opcode, bool *keyboard)
{
    //printf("%4X\n", opcode);
    switch ((opcode & 0xF000) >> 12) // First nibble
    {
        case 0X0:
        {
            if (opcode == 0x00E0) // Clear the display
            {
                for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++)
                {
                    chip8->display[i] = false;
                }
            }
            else if (opcode == 0x00EE) // Return from a subroutine
            {
                if (chip8->sp <= 0)
                {
                    fprintf(stderr, "Error at 0x%4X: Empty stack.\n", chip8->pc);
                    return;
                }
                chip8->sp--;
                chip8->pc = chip8->stack[chip8->sp];
            }
            chip8->pc += 2;
            break;
        }
        case 0X1: // Jump
        {
            chip8->pc = (opcode & 0x0FFF);
            break;
        }
        case 0X2: // Call a subroutine
        {
            if (chip8->sp > 15)
            {
                fprintf(stderr, "Error at 0x%4X: Stack is full.\n", chip8->pc);
                return;
            }
            chip8->stack[chip8->sp] = chip8->pc;
            chip8->sp++;
            chip8->pc = opcode & 0x0FFF;
            break;
        }
        case 0X3: // Skip next instruction if Vx == kk
        {
            if (chip8->V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) 
            {
                chip8->pc += 4;
            }
            else
            {
                chip8->pc += 2;
            }
            break;
        }
        case 0X4: // Skip next instruction if Vx != kk
        {
            if (chip8->V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
            {
                chip8->pc += 4;
            }
            else
            {
                chip8->pc += 2;
            }
            break;
        }
        case 0X5: // Skip next instruction if Vx == Vy
        {
            uint8_t X, Y;
            X = (opcode & 0x0F00) >> 8;
            Y = (opcode & 0x00F0) >> 4;
            if (chip8->V[X] == chip8->V[Y])
            {
                chip8->pc += 4;
            }
            else
            {
                chip8->pc += 2;
            }
            break;
        }
        case 0X6: // Set Vx = kk
        {
            chip8->V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
            chip8->pc += 2;
            break;
        }
        case 0X7: // Add kk to Vx
        {
            chip8->V[(opcode & 0x0F00) >> 8] = chip8->V[(opcode & 0x0F00) >> 8] + (opcode & 0x00FF);
            chip8->pc += 2;
            break;
        }
        case 0X8: // Operations on registers
        {
            uint8_t X, Y, operation;
            X = (opcode & 0x0F00) >> 8;
            Y = (opcode & 0x00F0) >> 4;
            operation = (opcode & 0x000F);
            switch (operation)
            {
                case 0x0: // LD
                {
                    chip8->V[X] = chip8->V[Y];
                    break;
                }
                case 0x1: // OR
                {
                    chip8->V[X] = chip8->V[X] | chip8->V[Y];
                    break;
                }
                case 0x2: // AND
                {
                    chip8->V[X] = chip8->V[X] & chip8->V[Y];
                    break;
                }
                case 0x3: // XOR
                {
                    chip8->V[X] = chip8->V[X] ^ chip8->V[Y];
                    break;
                }
                case 0x4: // ADD
                {
                    uint16_t result = chip8->V[X] + chip8->V[Y];
                    chip8->V[0xF] = (result > 255) ? 1 : 0;
                    chip8->V[X] = result & 0xFF;
                    break;
                }
                case 0x5: // SUB
                {
                    chip8->V[0xF] = (chip8->V[X] > chip8->V[Y]) ? 1 : 0;
                    chip8->V[X] = chip8->V[X] - chip8->V[Y];
                    break;
                }
                case 0x6: // SHR
                {
                    chip8->V[15] = chip8->V[X] & 0x01;
                    chip8->V[X] >>= 1;
                    break;
                }
                case 0x7: // SUBN
                {
                    chip8->V[0xF] = (chip8->V[Y] > chip8->V[X]) ? 1 : 0;
                    chip8->V[X] = chip8->V[Y] - chip8->V[X];
                    break;
                }
                case 0xE: // SHL
                {
                    if (((chip8->V[X] >> 7) & 1) == 1)
                    {
                        chip8->V[0xF] = 1;
                    }
                    else
                    {
                        chip8->V[0xF] = 0;
                    }
                    chip8->V[X] /= 2;
                    break;
                }
                default:
                {
                    printf("Unknow command %X %4X\n", operation, opcode);
                    break;
                }
            }
            chip8->pc += 2;
            break;
        }
        case 0X9: // Skip next instruction if Vx != Vy
        {
            uint8_t X, Y;
            X = (opcode & 0x0F00) >> 8;
            Y = (opcode & 0x00F0) >> 4;
            if (chip8->V[X] != chip8->V[Y])
            {
                chip8->pc += 4;
            }
            else
            {
                chip8->pc += 2;
            }
            break;
        }
        case 0XA: // Set i = kkk
        {
            chip8->I = (opcode & 0x0FFF);
            chip8->pc += 2;
            break;
        }
        case 0XB: // Jump to V0 + kkk
        {
            chip8->pc = (opcode & 0x0FFF) + chip8->V[0];
            break;
        }
        case 0XC: // Set Vx = random byte & kk
        {
            chip8->V[(opcode & 0x0F00) >> 8] = (rand() % 256) & (opcode & 0x00FF);
            chip8->pc += 2;
            break;
        }
        case 0XD: // Draw a sprite
        {
            uint8_t X, Y, n;
            X = (opcode & 0x0F00) >> 8;
            Y = (opcode & 0x00F0) >> 4;
            n = (opcode & 0x000F);
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    chip8->display[((chip8->V[Y] + i) % 32) * DISPLAY_WIDTH + ((chip8->V[X] + j) % 64)] ^= (chip8->ram[chip8->I + i] >> (7 - j)) & 0x01;
                }
            }
            chip8->pc += 2;
            break;
        }
        case 0XE:
        {
            uint8_t X = (opcode & 0x0F00) >> 8;
            uint8_t last_byte = opcode & 0x00FF;

            if (chip8->V[X] > 15 || chip8->V[X] < 0)
            {
                fprintf(stderr, "Error at 0x%4X: Wrong key index.\n", chip8->pc);
                return;
            }

            if (last_byte == 0x9E) // Skip next instruction if key with the value of Vx is pressed
            {
                if (keyboard[chip8->V[X]])
                {
                    chip8->pc += 4;
                }
                else
                {
                    chip8->pc += 2;
                }
            }
            else if (last_byte == 0xA1) // Skip next instruction if key with the value of Vx is not pressed
            {
                if (!(keyboard[chip8->V[X]]))
                {
                    chip8->pc += 4;
                }
                else
                {
                    chip8->pc += 2;
                }
            }
            break;
        }
        case 0XF:
        {
            uint8_t X = (opcode & 0x0F00) >> 8;
            uint8_t instruction = opcode & 0x00FF;
            switch (instruction)
            {
                case 0x07: // Set Vx = delay timer
                {
                    chip8->V[X] = chip8->delay_timer;
                    break;
                }
                case 0x0A: // Wait for a key press...
                {
                    uint16_t original_keyboard_state = 0;
                    uint16_t keyboard_state = 0;

                    for (int i = 0; i < 16; i++)
                    {
                        if (keyboard[i])
                        {
                            original_keyboard_state |= (1 << i);
                        }
                    }

                    while (keyboard_state <= original_keyboard_state)
                    {
                        for (int i = 0; i < 16; i++)
                        {
                            if (keyboard[i])
                            {
                                keyboard_state |= (1 << i);
                            }
                        }

                        if (original_keyboard_state > keyboard_state)
                        {
                            original_keyboard_state = keyboard_state;
                        }
                        usleep(10);
                    }

                    uint16_t diff = original_keyboard_state ^ keyboard_state;

                    int index;
                    for (index = 15; (diff & 0x01) != 1; index--)
                    {
                        diff = diff >> 1;
                    }

                    chip8->V[X] = index;
                    break;
                }
                case 0x15: // Set delay timer = Vx
                {
                    chip8->delay_timer = chip8->V[X];
                    break;
                }
                case 0x18: // Set sound timer = Vx
                {
                    chip8->sound_timer = chip8->V[X];
                    break;
                }
                case 0x1E: // Set I = I + Vx
                {
                    chip8->I += chip8->V[X];
                    break;
                }
                case 0x29: // Set I = Vx
                {
                    chip8->I = chip8->V[X];
                    break;
                }
                case 0x33:
                {
                    uint8_t val = chip8->V[X];
                    chip8->ram[chip8->I] = val / 100;
                    chip8->ram[(chip8->I) + 1] = (val / 10) % 10;
                    chip8->ram[(chip8->I) + 2] = val % 10;
                    break;
                }
                case 0x55: // Store registers V0 through Vx in memory starting at location I
                {
                    for (int i = 0; i < X; i++)
                    {
                        chip8->ram[chip8->I + i] = chip8->V[i];
                    }
                    break;
                }
                case 0x65: // Read registers V0 through Vx from memory starting at location I
                {
                    for (int i = 0; i < X; i++)
                    {
                        chip8->V[i] = chip8->ram[chip8->I + i];
                    }
                    break;
                }
            }
            chip8->pc += 2;
            break;
        }
        default:
        {
            break;
        }
    }
}

// Fréquence du son à jouer (en Hz)
const int frequency = 440;

// Durée du son (en millisecondes)
const int duration = 1000;

void chip8_loop(chip8* chip8, bool *quit, bool *keyboard)
{
    struct timeval start_time, end_time;
    long int elapsed_ms;

    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioSpec spec;
    SDL_AudioDeviceID device;

    SDL_zero(spec);
    spec.freq = 44100;
    spec.format = AUDIO_S16SYS;
    spec.channels = 1;
    spec.samples = 2048;
    spec.callback = NULL;
    int length = duration * spec.freq / 1000;

    device = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
    if (device == 0)
    {
        fprintf(stderr, "Cannot open audio: %s\n", SDL_GetError());
        return;
    }

    SDL_PauseAudioDevice(device, 0);

    while (!(*quit))
    {
        gettimeofday(&start_time, NULL);

        uint16_t opcode = chip8->ram[chip8->pc] << 8 | chip8->ram[(chip8->pc)+1];

        chip8_opcode_handler(chip8, opcode, keyboard);

        if (chip8->delay_timer != 0)
        {
            chip8->delay_timer--;
        }

        if (chip8->sound_timer != 0)
        {
            chip8->sound_timer--;
            for (int i = 0; i < length; i++) {
                Sint16 sample = (Sint16)(32760.0 * sin(2 * M_PI * frequency * i / spec.freq));
                SDL_QueueAudio(device, &sample, sizeof(sample));
            }
        }

        gettimeofday(&end_time, NULL);
        elapsed_ms = (end_time.tv_sec - start_time.tv_sec) * 1000 + (end_time.tv_usec - start_time.tv_usec) / 1000;
        if (elapsed_ms < REFRESH_RATE)
        {
            usleep((REFRESH_RATE - elapsed_ms) * 10); // TO FIX
        }
    }
}