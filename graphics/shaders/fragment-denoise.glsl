// ==========================================================================
// https://github.com/spite/Wagner/blob/master/fragment-shaders/denoise-fs.glsl
// ==========================================================================
#version 430

in vec2 UV;

layout(location = 0) out vec4 FragmentColour;

uniform sampler2D ColourTexture;

float exponent = 8;

void main(void)
{
	vec4 center = texture2D(ColourTexture, UV);
	vec4 color = vec4(0.0);
	vec2 size = 1.0 / textureSize(ColourTexture, 0);
	float total = 0.0;
	for (float x = -4.0; x <= 4.0; x += 1.0) {
		for (float y = -4.0; y <= 4.0; y += 1.0) {
			vec4 s = texture2D(ColourTexture, UV + vec2(x, y) * size);
			float weight = 1.0 - abs(dot(s.rgb - center.rgb, vec3(0.25)));
			weight = pow(weight, exponent);
			color += s * weight;
			total += weight;
		}
	}
    FragmentColour = texture(ColourTexture, UV);
	FragmentColour = color / total;
}
