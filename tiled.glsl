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
uniform int tilemap[4][4];

out vec4 finalColor;

ivec2 calcTileOffset(int tileIndex) {
    tileIndex -= 1;
    int x = tileIndex % atlasSize.x;
    int y = (tileIndex - x) / atlasSize.y;
    return ivec2(x, y);
}

vec4 tile(vec2 coords, int tileIndex) {
    ivec2 texSize = textureSize(texture1, 1);

    // for now just render the first tile
    

    vec2 tileSize = texSize / atlasSize;

    if (tileIndex == 0
        || coords.x > tileSize.x
        || coords.x < 0
        || coords.y > tileSize.y
        || coords.y < 0) {
        return vec4(0.0f, 0.0f, 0.0f, 0.0f);
    } else {
        ivec2 tileOffset = calcTileOffset(tileIndex);
        vec2 texCoords = coords + (tileOffset*tileSize);
        texCoords /= texSize;

        return texture(texture1, texCoords);
    }
}

void main() {
    vec2 uv = fragTexCoord;
    uv.x *= resolution.x / resolution.y;

    uv *= zoom;
    uv -= offset;

    finalColor = tile(uv, tilemap[2][2]);
}
