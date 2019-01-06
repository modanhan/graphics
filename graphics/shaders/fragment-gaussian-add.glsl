#version 430

in vec2 UV;

layout(location = 0) out vec4 FragmentColour;

layout(binding = 0) uniform sampler2D AddColourTexture;
layout(binding = 1) uniform sampler2D ColourTexture;

vec4 SampleBox() {
	vec2 size = 1.0 / textureSize(ColourTexture, 0);
	vec4 o; o.xy = size * -1; o.zw = size;
	return (texture2D(ColourTexture, UV + o.xy) + texture2D(ColourTexture, UV + o.zy)
	+ texture2D(ColourTexture, UV + o.xw) + texture2D(ColourTexture, UV + o.zw))
	* 0.25;
}

void main(void)
{
	vec4 color = vec4(0.0);
	vec4 total = SampleBox();
	// This makes an exponential decay, altho still technically energy conserving
	// it's hard to customize the style of the bloom and I have no idea if it's correct.
	// The alternative would be to have a shader that has N textures where N is the number
	// of downsamples; the shader then adds all the textures together, assigning each
	// texture with a weight so they sum to 1.
	FragmentColour = (total * 0.75 + texture2D(AddColourTexture, UV) * 0.25);
}
