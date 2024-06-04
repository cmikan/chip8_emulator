#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define MEMORY_SIZE 4096
#define MS_PER_FRAME 16 // 60hz

int main()
{
    FILE *file = NULL;

    file = fopen("test_opcode.ch8", "rb");
    unsigned char chip_ram[MEMORY_SIZE];

    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        chip_ram[i] = 0;
    }

    int bytes_read = fread(chip_ram + 0x200, 1, MEMORY_SIZE - 0x200, file);
    fclose(file);

    uint16_t pc = 0x200; // Program counter

    struct timeval start_time, end_time;
    long int elapsed_ms;
    int quit = 0;

    while (!quit)
    {
        gettimeofday(&start_time, NULL);
        uint16_t opcode = chip_ram[pc] << 8 | chip_ram[i+1];


        gettimeofday(&end_time, NULL);
        elapsed_ms = (end_time.tv_sec - start_time.tv_sec) * 1000 + (end_time.tv_usec - start_time.tv_usec) / 1000;

        if (elapsed_ms < MS_PER_FRAME) {
            usleep((MS_PER_FRAME - elapsed_ms) * 1000);
        }
    }
    /*
    for (int i = 0; i < bytes_read; i += 2)
    {
        uint16_t opcode = message[i] << 8 | message[i+1];
        switch ((opcode & 0xF000) >> 12) // First nibble
        {
            case 0X0:
            {
                if (opcode == 0x00E0)
                {
                    printf("Clear the display\n");
                }
                else if (opcode == 0x00EE)
                {
                    printf("Return from a subroutine\n");
                }
                break;
            }
            case 0X1:
            {
                printf("Jump to location %3X\n", (opcode & 0x0FFF));
                break;
            }
            case 0X2:
            {
                printf("Call subroutine at %3X\n", (opcode & 0x0FFF));
                break;
            }
            case 0X3:
            {
                printf("Skip next instruction if V%1X = %2X\n", (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
                break;
            }
            case 0X4:
            {
                printf("Skip next instruction if V%1X != %2X\n", (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
                break;
            }
            case 0X5:
            {
                uint8_t X, Y;
                X = (opcode & 0x0F00) >> 8;
                Y = (opcode & 0x00F0) >> 4;

                printf("Skip next instruction if V%1X = V%1X\n", X, Y);

                break;
            }
            case 0X6:
            {
                printf("Set V%1X = %2X\n", (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
                break;
            }
            case 0X7:
            {
                printf("Set V%1X = V%1X + %2X\n", (opcode & 0x0F00) >> 8, (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
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
                    case 0x0:
                    {
                        printf("LD V%1X V%1X\n", X, Y);
                        break;
                    }
                    case 0x1:
                    {
                        printf("OR V%1X V%1X\n", X, Y);
                        break;
                    }
                    case 0x2:
                    {
                        printf("AND V%1X V%1X\n", X, Y);
                        break;
                    }
                    case 0x3:
                    {
                        printf("XOR V%1X V%1X\n", X, Y);
                        break;
                    }
                    case 0x4:
                    {
                        printf("ADD V%1X V%1X\n", X, Y);
                        break;
                    }
                    case 0x5:
                    {
                        printf("SUB V%1X V%1X\n", X, Y);
                        break;
                    }
                    case 0x6:
                    {
                        printf("SHR V%1X V%1X\n", X, Y);
                        break;
                    }
                    case 0x7:
                    {
                        printf("SUBN V%1X V%1X\n", X, Y);
                        break;
                    }
                    case 0xE:
                    {
                        printf("SHL V%1X V%1X\n", X, Y);
                        break;
                    }
                    default:
                    {
                        printf("Unknow command %X %4X\n", operation, opcode);
                        break;
                    }
                }
                
                break;
            }
            case 0X9:
            {
                uint8_t X, Y;
                X = (opcode & 0x0F00) >> 8;
                Y = (opcode & 0x00F0) >> 4;

                printf("Skip next instruction if V%1X != V%1X\n", X, Y);
                break;
            }
            case 0XA:
            {
                printf("Set I = %3X\n", (opcode & 0x0FFF));
                break;
            }
            case 0XB:
            {
                printf("Jump to %3X\n + V0", (opcode & 0x0FFF));
                break;
            }
            case 0XC:
            {
                printf("Set V%1X = random bytes AND %2X\n", (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
                break;
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
                uint4_t X = (opcode & 0x0F00) >> 8;
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
    }
    printf("\n");*/

    return 0;
}