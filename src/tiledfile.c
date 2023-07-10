#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>

const int i = 1;
#define is_bigendian() ( (*(char*)&i) == 0 )

void textureFromPixels(Texture2D *loc, Color *pixels, int width, int height) {
    Image checkedIm = {
        .data = pixels,
        .width = width,
        .height = height,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    };

    *loc = LoadTextureFromImage(checkedIm);
    UnloadImage(checkedIm);
}

void processTilemapTexture(Texture2D *loc, int * tilelayout, int width, int height) {
    Color *pixels = (Color*) malloc(width * height * sizeof(Color));

    for (int i = 0; i < width*height; i++) {
        pixels[i] = (Color){ tilelayout[i], 0, 0, 0 };
    }

    textureFromPixels(loc, pixels, width, height);
}

//! read rgba image from file
void readrgba(Texture2D *loc, int width, int height, FILE *file) {
    Color *pixels = malloc(width*height*4);
    fread(pixels, (size_t) width*height*4, (size_t) 1, file);
    textureFromPixels(loc, pixels, width, height);
}


//! read a big endian bytes from file
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

//! load tilemap data from file
void loadTileMap(char * filename, Texture2D * tilemap, Texture2D * atlas, int * atlasSize) {
    int width, height, tilebytes, tilesize, atlasWidth, atlasHeight;
    int * tilelayout;

    FILE * file;

    file = fopen(filename, "rb");
    // skip header 
    fseek(file, 10, SEEK_CUR);
    // 4 bytes for int width
    readb((char *)&width, 4, file);
    // 4 bytes for int height
    readb((char *)&height, 4, file);
    // 4 bytes saying how big each tile is
    readb((char *)&tilebytes, 4, file);

    fprintf(stderr, "loading %d bytes per tile for %d (%dx%d) tiles\n", tilebytes, width*height, width, height);
    tilelayout = malloc(width*height*tilebytes);
    fread(tilelayout, tilebytes, width*height, file);

    // create a texture from the tilelayout data
    processTilemapTexture(tilemap, tilelayout, width, height);

    // read the pixel size of each tile
    readb((char *)&tilesize, 4, file);
    // read the atlas size
    readb((char *)&atlasWidth, 4, file);
    readb((char *)&atlasHeight, 4, file);
    atlasSize[0] = atlasWidth;
    atlasSize[1] = atlasHeight;

    // read the atlas itself
    readrgba(atlas, atlasWidth * tilesize, atlasHeight * tilesize, file);
    fprintf(stderr, "Successfully loaded tilemap from %s\n", filename);
}
