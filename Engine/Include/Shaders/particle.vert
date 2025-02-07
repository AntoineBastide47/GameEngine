#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>
layout (location = 1) in vec2 instancePosition;
layout (location = 2) in vec2 instanceScale;
layout (location = 3) in vec4 instanceColor;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;

void main() {
    TexCoords = vertex.zw;
    ParticleColor = instanceColor;
    gl_Position = projection * vec4(vertex.xy * instanceScale + instancePosition, 0.0, 1.0);
}
