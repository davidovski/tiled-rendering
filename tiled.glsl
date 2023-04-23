#version 430
precision highp float;

in vec3 vertexPos;
in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform vec2 resolution;
uniform vec2 offset;
uniform float zoom;
uniform ivec2 atlasSize;
uniform int tilemap[4][8];

out vec4 finalColor;

const vec4 none = vec4(0.0f, 0.0f, 0.0f, 0.0f);

ivec2 calcTileOffset(int tileIndex) {
    tileIndex -= 1;
    int x = tileIndex % atlasSize.x;
    int y = (tileIndex - x) / atlasSize.y;
    return ivec2(x, y);
}

vec4 tile(vec2 coords, int tileIndex, vec2 tileSize) {

    if (tileIndex == 0
        || coords.x > tileSize.x
        || coords.x < 0
        || coords.y > tileSize.y
        || coords.y < 0) {
        return none;
    } else {
        ivec2 tileOffset = calcTileOffset(tileIndex);
        vec2 texCoords = coords + (tileOffset*tileSize);
        texCoords /= tileSize;
        texCoords /= atlasSize;

        return texture(texture1, texCoords);
    }
}

void main() {
    ivec2 texSize = textureSize(texture1, 1);
    vec2 tileSize = texSize / atlasSize;

    vec2 uv = fragTexCoord * tileSize;
    uv.x *= resolution.x / resolution.y;

    uv *= zoom;
    uv -= offset;

    // get position in tiled world wow
    ivec2 tilemapPos = ivec2(floor(uv.x / tileSize.x), floor(uv.y / tileSize.y));

    if (uv.x < 0
    || uv.y < 0
    || uv.x > 8*tileSize.x
    || uv.y > 8*tileSize.y) {

        finalColor = none;
    } else {
        vec2 position = mod(uv,tileSize);
        finalColor = tile(position, tilemap[tilemapPos.y][tilemapPos.x], tileSize);
    }
}
