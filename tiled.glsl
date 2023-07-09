#version 430

#define MAP_H 4
#define MAP_W 16
precision highp float;

in vec3 vertexPos;
in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec2 resolution;
uniform vec2 offset;
uniform float zoom;
uniform ivec2 atlasSize;

out vec4 finalColor;

const vec4 none = vec4(0.0f, 0.0f, 0.0f, 0.0f);

bool inBounds(vec2 coords, vec2 area) {
    return coords.x > area.x
        || coords.x < 0
        || coords.y > area.y
        || coords.y < 0;
}

ivec2 calcTileOffset(int tileIndex) {
    tileIndex -= 1;
    int x = tileIndex % atlasSize.x;
    int y = (tileIndex - x) / atlasSize.y;
    return ivec2(x, y);
}

vec4 tile(vec2 coords, int tileIndex) {
    if (
        tileIndex == 0
        || inBounds(coords, vec2(1, 1))) {
        return none;
    } else {
        ivec2 tileOffset = calcTileOffset(tileIndex);
        vec2 texCoords = coords + (tileOffset);
        texCoords /= atlasSize;

        return texture(texture1, texCoords);
    }
}

void main() {
    ivec2 texSize = textureSize(texture1, 1);

    vec2 uv = fragTexCoord;
    uv.x *= resolution.x / resolution.y;

    uv *= zoom;
    uv -= offset;

    // get position in tiled world wow
    ivec2 tilemapPos = ivec2(floor(uv));

    if (inBounds(uv, vec2(MAP_W, MAP_H))) {
        finalColor = none;
    } else {
        vec2 position = mod(uv, 1);
        int tileIndex = int(256*texelFetch(texture2, tilemapPos, 0).r);
        finalColor = tile(position, tileIndex);
    }
}
