#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ppm.h"

void wait_for_number(FILE *img) {
    char c = fgetc(img);
    while (c < '0' || c > '9') {
        c = fgetc(img);
    }
    fseek(img, -1L, SEEK_CUR);
}

void skip_number(FILE *img) {
    char c = fgetc(img);
    while (c >= '0' && c <= '9') {
        c = fgetc(img);
    }
    fseek(img, -1L, SEEK_CUR);
}

// .ppm spec: http://netpbm.sourceforge.net/doc/ppm.html
// We ignore the possibility of comments in the middle of the file
// because what kind of madman would add those
int find_ppm_pix_start(FILE *img) {
    char buf[3];
    fgets(buf, 3, img);
    if (strcmp(buf, "P6") != 0)
        return -1;

    wait_for_number(img);
    skip_number(img);
    wait_for_number(img);
    skip_number(img);
    wait_for_number(img);
    skip_number(img);
    fgetc(img);
    return 0;
}
