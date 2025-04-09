#version 330 core
// Vertex data: position + texCoords
layout (location = 0) in vec4 vertex;
// Instance attributes for the model matrix (each column is a vec4)
layout (location = 1) in vec4 instanceModel0;
layout (location = 2) in vec4 instanceModel1;
layout (location = 3) in vec4 instanceModel2;
layout (location = 4) in vec4 instanceModel3;
// Instance attribute for sprite color
layout (location = 5) in vec4 instanceColor;
layout (location = 6) in vec4 instanceRect;
layout (location = 7) in vec2 instancePivot;
layout (location = 8) in float instancePPU;

out vec2 TexCoords;
out vec4 SpriteColor;

uniform mat4 projection;

void main() {
    mat4 model = mat4(instanceModel0, instanceModel1, instanceModel2, instanceModel3);

    vec2 pivotAdjusted = vertex.xy - instancePivot;
    vec2 scaledPosition = pivotAdjusted / instancePPU;

    vec4 worldPosition = model * vec4(scaledPosition, 0.0, 1.0);
    gl_Position = projection * worldPosition;

    TexCoords = instanceRect.xy + vertex.zw * instanceRect.zw;
    SpriteColor = instanceColor;
}
