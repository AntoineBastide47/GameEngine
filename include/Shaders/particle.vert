#version 330 core

layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
layout (location = 1) in vec4 positionAndScale; // <vec2 position, vec2 scale>
layout (location = 2) in vec4 color;
layout (location = 3) in vec4 rect;
layout (location = 4) in vec4 pivotRotationAndOrder; // <vec2 pivot, float rotation, float renderOrder

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;

void main() {
    // Apply pivot, scale and PPU
    vec2 scaledPosition = (vertex.xy - pivotRotationAndOrder.xy) * positionAndScale.zw;

    // Apply rotation
    float cosTheta = cos(pivotRotationAndOrder.z);
    float sinTheta = sin(pivotRotationAndOrder.z);
    mat2 rotMat = mat2(cosTheta, -sinTheta, sinTheta, cosTheta);
    vec2 rotatedPos = scaledPosition * rotMat;

    vec4 pos = projection * vec4(rotatedPos + positionAndScale.xy, 0.0, 1.0);
    pos.z = pivotRotationAndOrder.w;
    gl_Position = pos;

    // Remap texCoords using the rect
    TexCoords = rect.xy + vertex.zw * rect.zw;
    ParticleColor = color;
}
