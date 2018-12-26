#version 430

uniform image2D destTex;

void main() {
	ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
	imageStore(destTex, storePos, vec4(0.0));
}