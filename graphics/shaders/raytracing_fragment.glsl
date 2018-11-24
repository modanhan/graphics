
#version 430

struct camera_data {
	int width;
	int height;
	float focalLength;
};

struct sphere_data {
	vec3 position;
	float radius;
};


layout(std430, binding = 0) buffer camera_buffer { camera_data camera; };
layout(std430, binding = 1) buffer sphere_buffer { sphere_data spheres[]; };

in vec2 UV;

out vec4 FragmentColour;

//uniform sampler2D ColourTexture;

float length2(vec3 v) {
	return dot(v, v);
}

vec3 proj(vec3 u, vec3 v) {
	return v * dot(u, v) / dot(v, v);
}

vec3 ray_direction (float i, float j) {
	vec3 origin = vec3(0);
	float x = i / camera.width;
	float y = j / camera.height;
	x = -1.0 + 2.0 * x;
	y = -1.0 + 2.0 * y;
	return vec3(x, y / camera.width * camera.height, -camera.focalLength);
}

float sphere_hit (sphere_data s, vec3 origin, vec3 direction) {
	vec3 v_pc = s.position - origin;
	if (dot(v_pc, direction) < 0) {
		if (length2(v_pc) > s.radius * s.radius) {
			return 1e+38;
		}
		vec3 pc = proj(s.position - origin, direction);
		float d = sqrt(s.radius * s.radius - length2(pc - s.position));
		d -= length(origin - pc);
		return d;
	}
	else {
		vec3 pc = proj(s.position - origin, direction);
		if (length2(s.position - pc) > s.radius * s.radius) {
			return 1e+38;
		}
		float d = sqrt(s.radius * s.radius - length2(pc - s.position));
		if (length2(v_pc) > s.radius * s.radius) {
			d = length(pc - origin) - d;
		}
		else {
			d = length(pc - origin) + d;
		}
		return d;
	}
	return 1e+38;
}

void main(void)
{
	float d = 1e+38;
	for (int i = 0; i < spheres.length(); ++i) {
		float hit_d = sphere_hit(spheres[i], vec3(0), ray_direction(gl_FragCoord.x, gl_FragCoord.y));
		if (hit_d < d) {
			d = hit_d;
		}
	}
	if (d == 1e+38) {
		FragmentColour.xyz = vec3(0);
		return;
	}
	FragmentColour.xyz = vec3(1);
}
