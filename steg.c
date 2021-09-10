#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "ppm.h"

// Hide a byte in 8 bytes of the image file, by modifying the lowest bit
// of each of the image bytes. The byte is written in msb order
void steg_byte(uint8_t steg_byte, FILE *img) {
    static uint8_t img_bytes[8];
    static uint8_t new_byte;

    fread(img_bytes, sizeof(char), 8, img);
    for (int i = 0; i < 8; i++) {
        new_byte = img_bytes[i];
        new_byte = (new_byte & (0xff << 1)) | ((steg_byte >> (7 - i)) & 0x1);
        img_bytes[i] = new_byte;
    }

    fseek(img, -8L, SEEK_CUR);
    fwrite(img_bytes, sizeof(char), 8, img);
}

void steg_secret(FILE *secret, FILE *img) {
    // We write a header to the hidden data to allow us to verify that
    // the file contains a secret before attempting to unsteg it
    const char *header = "steg";
    for (int i = 0; i < 5; i++)
        steg_byte(header[i], img);

    // We write a unsigned 32-bit integer to let us know how many secret bytes
    // are hidden in the file bytes are written in msb order
    fseek(secret, 0L, SEEK_END);
    uint32_t secret_sz = (uint32_t)ftell(secret);
    fseek(secret, 0L, SEEK_SET);
    for(int shift = 24; shift >= 0; shift -= 8)
        steg_byte((uint8_t)(secret_sz >> shift), img);

    // Write the secret data to the image
    int byte;
    while ((byte = getc(secret)) != EOF)
        steg_byte(byte, img);
}

FILE* open_file(char *filename, char *mode) {
    FILE *file = fopen(filename, mode);
    if (!file) {
        fprintf(stderr, "Failed trying to open file %s\n", filename);
        exit(1);
    }
    return file;
}

int ensure_size(FILE *secret, FILE *img) {
    fseek(img, 0L, SEEK_END);
    fseek(secret, 0L, SEEK_END);

    if ((ftell(secret) + 20)*8 > ftell(img))
        return -1;

    fseek(img, 0L, SEEK_SET);
    fseek(secret, 0L, SEEK_SET);
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: steg <ppm file> <input data file> \n");
        exit(1);
    }

    FILE *img = open_file(argv[1], "rb+");
    FILE *secret = open_file(argv[2], "rb");

    if (ensure_size(secret, img) == -1) {
        fprintf(stderr, "Error: file %s is not big enough to hide %s\n",
                argv[1], argv[2]);
        exit(1);
    }


    if (find_ppm_pix_start(img) == -1) {
        fprintf(stderr, "Error: file might not be ppm P6 format\n");
        exit(1);
    }

    steg_secret(secret, img);
    printf("Hid %ld bytes of data in the file %s\n", ftell(secret), argv[1]);

    fclose(img);
    fclose(secret);
    return 0;
}
