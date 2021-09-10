all: steg unsteg

steg: steg.c ppm.c
	gcc -o steg steg.c ppm.c -Wall

unsteg: unsteg.c ppm.c
	gcc -o unsteg unsteg.c ppm.c -Wall
