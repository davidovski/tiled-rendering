#include <raylib.h>
#include <stdio.h>

void textureFromPixels(Texture2D *texOut, Color *pixels, int width, int height);

int readb(char *out, size_t noBytes, FILE *file);

int writeb(char *in, size_t noBytes, FILE *file);
