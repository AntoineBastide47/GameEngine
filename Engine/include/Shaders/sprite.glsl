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
// <float u, float v, float witdth, float height>
layout (location = 3) in vec4 aRect;
// <float packedPivot, float rotation, float packedRenderOrderAndtextureIndex, float packedFlip>
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

    // Apply flip
    vec2 flip = unpackTwoFloats(aOther.w);
    vec2 coords = vec2(flip.x != 1.0 ? 1.0 - aVertex.z : aVertex.z, flip.y != 1.0 ? 1.0 - aVertex.w : aVertex.w);

    int textureId = int(aOther.z) & 0xFFFF;
    vTextureData = vec4(aRect.xy + coords * aRect.zw, textureId, 0.);
    vVertColor = aColor;

    int renderOrder = int(aOther.z) >> 16;
    gl_Position = uViewProjection * vec4(rotatedPos + aPositionAndScale.xy, renderOrder, 1.0);
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
