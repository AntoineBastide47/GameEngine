#region VERTEX
#version 330 core

// --- Vertex data ---
// <vec2 position, vec2 textureCoords>
layout (location = 0) in vec4 aVertex;

// --- Instance attributes ---
// <vec2 position, vec2 scale>
layout (location = 1) in vec4 aPositionAndScale;
// <float r, float g, float b, float a>
layout (location = 2) in vec4 aColor;
// <float u, floatv, float witdth, float height>
layout (location = 3) in vec4 aRect;
// <float packedPivot, float rotation, float renderOrder, float textureIndex>
layout (location = 4) in vec4 aOther;

out vec4 vTextureData;
out vec4 vVertColor;

#define Matrices

vec2 unpackTwoFloats(float packed) {
    uint raw = floatBitsToUint(packed);
    float a = float(raw >> 16) / 65535.0;
    float b = float(raw & 0xFFFFu) / 65535.0;
    return vec2(a, b);
}

void main() {
    // Create the rotation matrix
    float cosTheta = cos(aOther.y);
    float sinTheta = sin(aOther.y);
    mat2 rotMat = mat2(cosTheta, -sinTheta, sinTheta, cosTheta);

    // Apply pivot and rotation
    vec2 pivot = unpackTwoFloats(aOther.x);
    vec2 scaledPosition = (aVertex.xy - pivot) * aPositionAndScale.zw;
    vec2 rotatedPos = scaledPosition * rotMat;

    vTextureData = vec4(aRect.xy + aVertex.zw * aRect.zw, aOther.w, 0.);
    vVertColor = aColor;

    gl_Position = uViewProjection * vec4(rotatedPos + aPositionAndScale.xy, aOther.z, 1.0);
}

#region FRAGMENT
#version 330 core
in vec4 vTextureData;
in vec4 vVertColor;

out vec4 vColor;

#define Textures

void main() {
    vColor = vVertColor * SampleTexture();
}
