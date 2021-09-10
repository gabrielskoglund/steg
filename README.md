# steg - a very simple steganography program

Steg is my attempt at creating a program that can hide arbitrary data in an image so that the image does not appear to be modified upon visual inspection. It does this by hiding a byte in 8 other bytes of the RGB data of the image, modifying the least significant bit of each image byte.

This implementation is very basic, and uses the old bad but oh so simple image format `.ppm`. The basic idea can of course be extended to more complex file formats, and has been already been done better by people smarter than me, but it's kind of neat nonetheless.
