#include <stdio.h>
#include <stdint.h>

int main()
{
    FILE *file = NULL;

    file = fopen("test_opcode2.ch8", "rb");
    unsigned char message[4096];


    int bytes_read = fread(message, 1, 4096, file);
    fclose(file);
    
    for (int i = 0; i < bytes_read; i += 2)
    {
        uint16_t opcode = message[i] << 8 | message[i+1];
        switch ((opcode & 0xF000) >> 12) // First nibble
        {
            case 0X0:
            {
                break;
            }
            case 0X1:
            {
                break;
            }
            case 0X2:
            {
                break;
            }
            case 0X3:
            {
                break;
            }
            case 0X4:
            {
                break;
            }
            case 0X5:
            {
                break;
            }
            case 0X6:
            {
                break;
            }
            case 0X7:
            {
                break;
            }
            case 0X8:
            {
                break;
            }
            case 0X9:
            {
                break;
            }
            case 0XA:
            {
                break;
            }
            case 0XB:
            {
                break;
            }
            case 0XC:
            {
                break;
            }
            case 0XD:
            {
                break;
            }
            case 0XE:
            {
                break;
            }
            case 0XF:
            {
                break;
            }
            default:
                break;
                
        }
    }
    printf("\n");

    return 0;
}