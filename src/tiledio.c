#include "tiledio.h"

const int endian = 1;
#define is_bigendian() ( (*(char*)&endian) == 0 )


void textureFromPixels(Texture2D *texOut, Color *pixels, int width, int height) {
    Image checkedIm = {
        .data = pixels,
        .width = width,
        .height = height,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    };

    *texOut = LoadTextureFromImage(checkedIm);
}


int readb(char * out, size_t noBytes, FILE * file) {
    if (!fread(out, (size_t)1, (size_t) noBytes, file))
        return 1;

    if (is_bigendian()) 
        return 0;

    int tmp;
    // reverse byte order
    for(int i = 0; i < noBytes/2; i++) {
        tmp = out[i];
        out[i] = out[noBytes-i-1];
        out[noBytes-i-1] = tmp;
    }

    return 0;
}

int writeb(char * in, size_t noBytes, FILE * file) {
    if (!is_bigendian()) {
        int tmp;
        // reverse byte order
        for(int i = 0; i < noBytes/2; i++) {
            tmp = in[i];
            in[i] = in[noBytes-i-1];
            in[noBytes-i-1] = tmp;
        }
    }

    return fwrite(in, (size_t)1, (size_t) noBytes, file);
}

