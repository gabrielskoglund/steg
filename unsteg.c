#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "ppm.h"

// Reveal a byte hidden in 8 image data bytes. See steg.c for details
uint8_t unsteg_byte(FILE *img) {
    static uint8_t img_bytes[8];
    unsigned char secret_byte = 0;

    fread(img_bytes, sizeof(uint8_t), 8, img);
    for (int i = 0; i < 8; i++) {
        secret_byte <<= 1;
        secret_byte |= (img_bytes[i] & 0x1);
    }
    return secret_byte;
}

int unsteg_secret(FILE *img) {
    // Check so that the file contains the hidden header string "steg"
    // placed at the beginning of the image data
    const char *header = "steg";
    char buf[5];
    for (int i = 0; i < 5; i++)
        buf[i] = unsteg_byte(img);
    buf[4] = '\0';

    if (strcmp(header, buf) != 0)
        return -1;

    // Read four hidden bytes after the header, converting these
    // into a 32-bit number telling us how many bytes to read
    uint32_t bytes_to_reveal = 0;
    for (int shift = 24; shift >= 0; shift -= 8)
        bytes_to_reveal |= unsteg_byte(img) << shift;

    uint8_t byte;
    for (int i = 0; i < bytes_to_reveal; i++) {
        byte = unsteg_byte(img);
        fwrite(&byte, sizeof(uint8_t), 1, stdout);
    }

    return 0;
}

FILE* open_file(char *filename, char *mode) {
    FILE *file = fopen(filename, mode);
    if (!file) {
        fprintf(stderr, "Failed trying to open file %s\n", filename);
        exit(1);
    }
    return file;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: unsteg <ppm file>\n");
        exit(1);
    }

    FILE *img = open_file(argv[1], "rb");

    if (find_ppm_pix_start(img) == -1) {
        fprintf(stderr, "Error: file might not be ppm P6 format\n");
        fclose(img);
        exit(1);
    }

    if (unsteg_secret(img) == -1) {
        fprintf(stderr, "File does not appear to contain a secret\n");
    }

    fclose(img);
    return 0;
}
