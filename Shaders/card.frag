#version 460 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2DArray uCardTextures;  // 54 layers: 52 cards + back + empty
uniform int uCardIndex;                // 0..53  (calculated on CPU)
uniform bool uFaceUp;

void main()
{
    int layer = uFaceUp ? uCardIndex : 52;  // 52 = back, 53 = optional empty

    vec3 texColor = texture(uCardTextures, vec3(TexCoord, layer)).rgb;

    // Optional: slight rounding of corners (simple discard)
    vec2 uv = abs(TexCoord - 0.5) * 2.0;
    float corner = length(max(uv - vec2(0.8), 0.0));
    if (corner > 0.2) discard;  // adjust for roundness

    FragColor = vec4(texColor, 1.0);
}
