#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdbool.h>

#define MEMORY_SIZE 4096
#define MS_PER_FRAME 500 // 60hz
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s file.ch8", argv[0]);
    }
    FILE *file = NULL;

    file = fopen("test_opcode.ch8", "rb");
    unsigned char chip_ram[MEMORY_SIZE];

    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        chip_ram[i] = 0;
    }

    uint8_t V[16]; // Registers
    uint16_t I;

    bool display_memory[DISPLAY_WIDTH * DISPLAY_HEIGHT];

    int bytes_read = fread(chip_ram + 0x200, 1, MEMORY_SIZE - 0x200, file);
    fclose(file);

    uint16_t pc = 0x200; // Program counter

    struct timeval start_time, end_time;
    long int elapsed_ms;
    int quit = 0;

    while (!quit)
    {
        gettimeofday(&start_time, NULL);

        uint16_t opcode = chip_ram[pc] << 8 | chip_ram[pc+1];
        printf("%4X ", opcode);

        switch ((opcode & 0xF000) >> 12) // First nibble
        {
            case 0X0:
            {
                if (opcode == 0x00E0)
                {
                    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++)
                    {
                        display_memory[i] = false;
                    }
                }
                else if (opcode == 0x00EE)
                {
                    printf("Return from a subroutine\n");
                }
                else
                {
                    pc += 2;
                }
                break;
            }
            case 0X1:
            {
                pc = (opcode & 0x0FFF);
                break;
            }
            case 0X2:
            {
                printf("Call subroutine at %3X\n", (opcode & 0x0FFF));
                break;
            }
            case 0X3:
            {
                if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
                {
                    pc += 4;
                }
                else
                {
                    pc += 2;
                }
                break;
            }
            case 0X4:
            {
                if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
                {
                    pc += 4;
                }
                else
                {
                    pc += 2;
                }
                break;
            }
            case 0X5:
            {
                uint8_t X, Y;
                X = (opcode & 0x0F00) >> 8;
                Y = (opcode & 0x00F0) >> 4;
                if (V[X] == V[Y])
                {
                    pc += 4;
                }
                else
                {
                    pc += 2;
                }
                break;
            }
            case 0X6:
            {
                V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
                pc += 2;
                break;
            }
            case 0X7:
            {
                V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] + (opcode & 0x00FF);
                pc += 2;
                break;
            }
            case 0X8:
            {
                uint8_t X, Y, operation;
                X = (opcode & 0x0F00) >> 8;
                Y = (opcode & 0x00F0) >> 4;
                operation = (opcode & 0x000F);
                switch (operation)
                {
                    case 0x0: // LD
                    {
                        V[X] = V[Y];
                        break;
                    }
                    case 0x1: // OR
                    {
                        V[X] = V[X] | V[Y];
                        break;
                    }
                    case 0x2: // AND
                    {
                        V[X] = V[X] & V[Y];
                        break;
                    }
                    case 0x3: // XOR
                    {
                        V[X] = V[X] ^ V[Y];
                        break;
                    }
                    case 0x4: // ADD
                    {
                        uint16_t result = V[X] + V[Y];

                        V[0xF] = (result > 255) ? 1 : 0;
                        V[X] = result & 0xFF;
                        break;
                    }
                    case 0x5: // SUB
                    {
                        V[0xF] = (V[X] > V[Y]) ? 1 : 0;
                        V[X] = V[X] - V[Y];
                        break;
                    }
                    case 0x6: // SHR
                    {
                        if ((V[X] & 1) == 1)
                        {
                            V[0xF] = 1;
                        }
                        else
                        {
                            V[0xF] = 0;
                        }
                        V[X] /= 2;
                        break;
                    }
                    case 0x7: // SUBN
                    {
                        V[0xF] = (V[Y] > V[X]) ? 1 : 0;
                        V[X] = V[Y] - V[X];
                        break;
                    }
                    case 0xE: // SHL
                    {
                        if (((V[X] >> 7) & 1) == 1)
                        {
                            V[0xF] = 1;
                        }
                        else
                        {
                            V[0xF] = 0;
                        }
                        V[X] /= 2;
                        break;
                    }
                    default:
                    {
                        printf("Unknow command %X %4X\n", operation, opcode);
                        break;
                    }
                }
                pc += 2;
                break;
            }
            case 0X9:
            {
                uint8_t X, Y;
                X = (opcode & 0x0F00) >> 8;
                Y = (opcode & 0x00F0) >> 4;
                if (V[X] != V[Y])
                {
                    pc += 4;
                }
                else
                {
                    pc += 2;
                }
                break;
                break;
            }
            case 0XA:
            {
                I = (opcode & 0x0FFF);
                pc += 2;
                break;
            }
            case 0XB:
            {
                pc = (opcode & 0x0FFF) + V[0]
                break;
            }
            case 0XC:
            {
                printf("Set V%1X = random bytes AND %2X\n", (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
                break;
            }
            case 0XD:
            {
                uint8_t X, Y, nibble;
                X = (opcode & 0x0F00) >> 8;
                Y = (opcode & 0x00F0) >> 4;
                nibble = (opcode & 0x000F);
                printf("Display %1X-byte sprite at V%1X V%1X\n", nibble, X, Y);
                break;
            }
            case 0XE:
            {
                uint8_t X = (opcode & 0x0F00) >> 8;
                uint8_t last_byte = opcode & 0x00FF;
                if (last_byte == 0x9E)
                {
                    printf("Skip next instruction if key with the value of V%1X is pressed\n", X);
                }
                else if (last_byte == 0xA1)
                {
                    printf("Skip next instruction if key with the value of V%1X is not pressed\n", X);
                }
                break;
            }
            case 0XF:
            {
                uint8_t X = (opcode & 0x0F00) >> 8;
                uint8_t instruction = opcode & 0x00FF;
                switch (instruction)
                {
                    case 0x07:
                    {
                        break;
                    }
                    case 0x0A:
                    {
                        break;
                    }
                    case 0x15:
                    {
                        break;
                    }
                    case 0x18:
                    {
                        break;
                    }
                    case 0x1E:
                    {
                        break;
                    }
                    case 0x29:
                    {
                        break;
                    }
                    case 0x33:
                    {
                        break;
                    }
                    case 0x55:
                    {
                        break;
                    }
                    case 0x65:
                    {
                        break;
                    }
                }
                break;
            }
            default:
            {
                printf("\033[0;31mUnknown opcode\033[49m\n");
                break;
            }   
        }
        printf("\n");

        gettimeofday(&end_time, NULL);
        elapsed_ms = (end_time.tv_sec - start_time.tv_sec) * 1000 + (end_time.tv_usec - start_time.tv_usec) / 1000;
        if (elapsed_ms < MS_PER_FRAME)
        {
            usleep((MS_PER_FRAME - elapsed_ms) * 1000);
        }
    }

    return 0;
}