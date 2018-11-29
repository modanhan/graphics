// ==========================================================================
// https://github.com/spite/Wagner/blob/master/fragment-shaders/denoise-fs.glsl
// ==========================================================================
#version 430

in vec2 UV;

layout(location = 0) out vec4 FragmentColour;

uniform sampler2D ColourTexture;

vec3 aces(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

void main(void)
{
	vec3 color = texture2D(ColourTexture, UV).xyz;
	FragmentColour.xyz = aces(color);
	FragmentColour.w = 1.0;
}
