
#version 430
#extension GL_ARB_shading_language_include : require
#include "/shaders/math.glsl"
#include "/shaders/rt-data.glsl"
#include "/shaders/rt-algorithm.glsl"

layout(location = 0) in vec2 UV;
layout(location = 0) uniform sampler2D rt_Texture;

layout(location = 0) out vec4 FragmentColour;

void main(void)
{
	float intensity = (length(fwidth(texture(rt_Texture, UV + vec2(0.5 / camera.width, 0.5 / camera.height))).xyz)
					+ length(fwidth(texture(rt_Texture, UV + vec2(-0.5 / camera.width, 0.5 / camera.height))).xyz)
					+ length(fwidth(texture(rt_Texture, UV + vec2(0.5 / camera.width, -0.5 / camera.height))).xyz)
					+ length(fwidth(texture(rt_Texture, UV + vec2(-0.5 / camera.width, -0.5 / camera.height))).xyz));
	vec3 d = gl_FragCoord.xyz;
	vec3 acc = texture(rt_Texture, UV).xyz;
	int count = 1;
	for (float i = 0.1; i < intensity; i += 0.05) {
		d = _random(d) * 0.5;
		vec3 direction = ray_direction(gl_FragCoord.xy + d.xy);
		acc += ray_trace(direction);
		count++;
		if (count > 64) break;
	}
	FragmentColour.xyz = acc / count;

	//	Check the number of samples taken by uncommenting the following line
	//	FragmentColour = vec4(count / 64.0);
}
