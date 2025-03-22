#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>
layout (location = 1) in vec2 instancePosition;
layout (location = 2) in vec2 instanceScale;
layout (location = 3) in vec4 instanceColor;
layout (location = 4) in float rotation;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;

void main() {
    TexCoords = vertex.zw;
    ParticleColor = instanceColor;

    // Compute rotation matrix
    float cosTheta = cos(rotation);
    float sinTheta = sin(rotation);

    mat2 rotationMatrix = mat2(
        cosTheta, -sinTheta,
        sinTheta, cosTheta
    );

    // Apply rotation, then scaling and translation
    vec2 localPosition = vec2(vertex.xy) * instanceScale;
    vec2 rotatedPosition = localPosition * rotationMatrix;
    gl_Position = projection * vec4(rotatedPosition + instancePosition, 0.0, 1.0);
}
