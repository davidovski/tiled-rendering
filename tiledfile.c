#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>

const int i = 1;
#define is_bigendian() ( (*(char*)&i) == 0 )

Texture2D processTilemapTexture(int * tilelayout, int width, int height) {
    Color *pixels = (Color*) malloc(width * height * sizeof(Color));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int tile = tilelayout[y*width + x];
            pixels[y*width + x] = (Color){
                (int) tile, 0, 0, 0
            };
        }
    }

    Image checkedIm = {
        .data = pixels,
        .width = width,
        .height = height,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    };

    Texture2D checked = LoadTextureFromImage(checkedIm);
    UnloadImage(checkedIm);

    return checked;
}

// read a big endian bytes from file
int readb(char *out, size_t noBytes, FILE *file) {
    int s = fread(out, (size_t)1, (size_t) noBytes, file);
    
    if (!is_bigendian()) {
        int tmp;
        // reverse byte order
        for(int i = 0; i < noBytes/2; i++) {
            tmp = out[i];
            out[i] = out[noBytes-i-1];
            out[noBytes-i-1] = tmp;
        }
    }
    return s;
}


Texture2D loadTileMap(char *filename) {
    int width, height, tilebytes;
    int *tilelayout;

    FILE *file;

    file = fopen(filename, "rb");
    // skip header 
    fseek(file, 10, SEEK_CUR);
    // 4 bytes for int width
    readb((char *)&width, 4, file);
    // 4 bytes for int height
    readb((char *)&height, 4, file);
    // 1 byte saying how big each tile is
    readb((char *)&tilebytes, 4, file);
    printf("tilebytes: %d\n", tilebytes);

    tilelayout = malloc(width*height*tilebytes);
    int tile;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            readb((char *)&tile, tilebytes, file);
            tilelayout[y*width + x] = tile;
        }
    }
    return processTilemapTexture(tilelayout, width, height);
}
