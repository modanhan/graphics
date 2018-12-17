#version 430

in vec2 UV;

layout(location = 0) out vec4 FragmentColour;

uniform sampler2D ColourTexture;

float exponent = 32;

void main(void)
{
	vec4 center = texture2D(ColourTexture, UV);
	vec4 color = vec4(0.0);
	vec2 size = 1.0 / textureSize(ColourTexture, 0);
	vec4 total = vec4(0.0);
	total += texture2D(ColourTexture, UV + vec2(-3.0, 0) * size) * 0.00598;
	total += texture2D(ColourTexture, UV + vec2(-2.0, 0) * size) * 0.060626;
	total += texture2D(ColourTexture, UV + vec2(-1.0, 0) * size) * 0.241843;
	total += texture2D(ColourTexture, UV + vec2(0.0, 0) * size) * 0.383103;
	total += texture2D(ColourTexture, UV + vec2(1.0, 0) * size) * 0.241843;
	total += texture2D(ColourTexture, UV + vec2(2.0, 0) * size) * 0.060626;
	total += texture2D(ColourTexture, UV + vec2(3.0, 0) * size) * 0.00598;
	total.w = 1.0;
	FragmentColour = total;
}
