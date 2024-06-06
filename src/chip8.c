#include <stdlib.h>
#include <string.h>

#include "chip8_utils.h"
#include "display.h"
#include "constant.h"

int file_extension_ch8(const char *filename);

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file.ch8>\n", argv[0]);
        return 1;
    }

    if (!file_extension_ch8(argv[1]))
    {
        fprintf(stderr, "Error: %s, wrong file extension.\n", argv[1]);
        return 1;
    }

    chip8 *chip8 = chip8_init();
    bool quit = false;
    bool keyboard[16] = {false};

    void *display = display_init(chip8->display, &quit, keyboard); // Create a thread
    if (display == NULL)
    {
        return 1;
    }

    if (chip8_load_rom(chip8, argv[1]))
    {
        return 1;
    }

    chip8_loop(chip8, &quit, keyboard);
    display_delete(display); // Join the previously created thread
    free(chip8);

    return 0;
}


/**
 * Check if the file extension is a ch8.
 * @param filename: A pointer to the filename to check
 * @return 1 if the file is a ch8, 0 otherwise.
 */
int file_extension_ch8(const char *filename)
{
    if (filename == NULL)
    {
        return 0;
    }

    size_t len = strlen(filename);
    if (len < 4)
    {
        return 0;
    }

    if (strcmp(filename + len - 4, ".ch8") == 0)
    {
        return 1;
    }
    return 0;
}