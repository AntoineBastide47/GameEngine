#version 330 core
in vec2 TexCoords;
in vec4 SpriteColor;
out vec4 color;

uniform sampler2D sprite;

void main() {
    color = SpriteColor * texture(sprite, TexCoords);
}
