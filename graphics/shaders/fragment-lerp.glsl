#version 430

in vec2 UV;

layout(location = 0) out vec4 FragmentColour;

layout(binding = 0) uniform sampler2D ColourTexture0;
layout(binding = 1) uniform sampler2D ColourTexture1;

void main(void)
{
	FragmentColour = mix(texture2D(ColourTexture0, UV), texture2D(ColourTexture1, UV), 0.5);
}
