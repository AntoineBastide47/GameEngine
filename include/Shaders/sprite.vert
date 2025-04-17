#version 330 core

// Vertex data: position + texCoords
layout (location = 0) in vec4 vertex;
// Instance attributes for the model matrix (each column is a vec4)
layout (location = 1) in vec4 scaleAndRotation;
layout (location = 2) in vec4 positionAndPivot; // <vec2 position, vec2 pivot>
layout (location = 3) in vec4 color;
layout (location = 4) in vec4 rect;
layout (location = 5) in vec4 renderOrderAndPPU; // <float renderOrder, float PPU, 0, 0>

out vec2 TexCoords;
out vec4 SpriteColor;

uniform mat4 projection;

void main() {
    mat4 model = mat4(
    vec4(scaleAndRotation.x, scaleAndRotation.z, 0, 0), // column 0
    vec4(scaleAndRotation.y, scaleAndRotation.w, 0, 0), // column 1
    vec4(0, 0, 1, 0), // column 2
    vec4(positionAndPivot.x, positionAndPivot.y, renderOrderAndPPU.x, 1.0) // column 3: translation + renderOrder
    );

    vec2 scaledPosition = (vertex.xy - positionAndPivot.zw) / renderOrderAndPPU.y;
    vec4 worldPosition = model * vec4(scaledPosition, 0.0, 1.0);
    gl_Position = projection * worldPosition;

    TexCoords = rect.xy + vertex.zw * rect.zw;
    SpriteColor = color;
}
