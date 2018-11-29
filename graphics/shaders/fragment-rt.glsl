
#version 430
#extension GL_ARB_shading_language_include : require
#include "/shaders/math.glsl"
#include "/shaders/rt-data.glsl"
#include "/shaders/rt-algorithm.glsl"

layout(location = 0) out vec4 FragmentColour;

void main(void)
{
	vec3 direction = ray_direction(gl_FragCoord.xy);
	FragmentColour.xyz = ray_trace(direction);
}
