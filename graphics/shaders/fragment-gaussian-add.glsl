#version 430

in vec2 UV;

layout(location = 0) out vec4 FragmentColour;

uniform sampler2D ColourTexture;
uniform sampler2D AddColourTexture;
layout(location = 1) uniform vec2 direction;

float exponent = 32;

void main(void)
{
	vec4 color = vec4(0.0);
	vec2 size = 1.0 / textureSize(ColourTexture, 0);
	vec4 total = vec4(0.0);
	vec2 position = direction * -3.0 * size;
	total += texture2D(ColourTexture, UV + position + direction * 0 * size) * 0.00598;
	total += texture2D(ColourTexture, UV + position + direction * 1 * size) * 0.060626;
	total += texture2D(ColourTexture, UV + position + direction * 2 * size) * 0.241843;
	total += texture2D(ColourTexture, UV + position + direction * 3 * size) * 0.383103;
	total += texture2D(ColourTexture, UV + position + direction * 4 * size) * 0.241843;
	total += texture2D(ColourTexture, UV + position + direction * 5 * size) * 0.060626;
	total += texture2D(ColourTexture, UV + position + direction * 6 * size) * 0.00598;
	total.w = 1.0;
	FragmentColour = (total + texture2D(AddColourTexture, UV)) * 0.5;
}
