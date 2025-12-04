#version 460 core

// Attributes
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 uProjection;   // orthographic projection
uniform vec2 uPosition;     // card center in NDC
uniform vec2 uSize;         // card size in NDC
uniform bool uFaceUp;
uniform int uSuit;
uniform int uRank;

void main()
{
    // Transform from local space -> NDC
    vec2 worldPos = aPos * uSize + uPosition;
    gl_Position = uProjection * vec4(worldPos, 0.0, 1.0);

    // Give the value back?
    TexCoord = aTexCoord;

    // Pass card info to fragment shader
    // For now we just use it to flip texture when face down
}
