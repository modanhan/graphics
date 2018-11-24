
#version 430

struct camera_data {
	int width;
	int height;
	float focalLength;
};

layout (std430, binding = 1) buffer triangle_data {
	vec3 v0;
	vec3 v1;
	vec3 v2;
};

layout(std430, binding = 0) buffer camera_buffer { camera_data camera; };

in vec2 UV;

out vec4 FragmentColour;

uniform sampler2D ColourTexture;

vec3 ray_direction (float i, float j) {
		vec3 origin = vec3(0);
		float x = i / camera.width;
		float y = j / camera.height;
		x = -1.0 + 2.0 * x;
		y = -1.0 + 2.0 * y;
		return vec3(x, y / camera.width * camera.height, -camera.focalLength);
}

void main(void)
{
    FragmentColour = texture(ColourTexture, UV);
	FragmentColour.x = ray_direction(gl_FragCoord.x, gl_FragCoord.y).x;
	FragmentColour.y = ray_direction(gl_FragCoord.x, gl_FragCoord.y).y;
}
