#version 430

precision highp float;

in vec3 vertexPos;
in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform sampler2D atlasTexture;
uniform sampler2D tilemapTexture;

uniform vec2 offset;
uniform float zoom;
uniform ivec2 atlasSize;
uniform ivec2 renderArea;

out vec4 finalColor;

const vec4 none = vec4(0.0f, 0.0f, 0.0f, 0.0f);
const vec4 gridColor = vec4(0.0f, 0.0f, 0.0f, 0.05f);

bool outBounds(vec2 coords, vec2 area) {
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

vec4 drawGrid(vec2 coords) {
    if (abs(floor(coords.x) - coords.x) < 1/16.0f 
     || abs(floor(coords.y) - coords.y) < 1/16.0f) {
        return gridColor;
    } else {
        return none;
    }
}

vec4 tile(vec2 coords, int tileIndex) {
    if (
        tileIndex == 0
        || outBounds(coords, vec2(1, 1))) {
        return drawGrid(coords);
    } else {
        ivec2 tileOffset = calcTileOffset(tileIndex);
        vec2 texCoords = coords + (tileOffset);
        texCoords /= atlasSize;

        return texture(atlasTexture, texCoords);
    }
}

void main() {
    ivec2 res = textureSize(texture0, 1);

    vec2 uv = fragTexCoord;

    uv /= zoom;
    uv -= offset;

    // get position in tiled world wow
    ivec2 tilemapPos = ivec2(floor(uv));

    if (outBounds(uv, renderArea)) {
        finalColor = none;
    } else {
        vec2 position = mod(uv, 1);
        int tileIndex = int(256*texelFetch(tilemapTexture, tilemapPos, 0).r);
        finalColor = tile(position, tileIndex);
    }
}
