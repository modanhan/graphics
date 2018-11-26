
#version 430

layout(location = 0) in vec2 VertexPosition;
layout(location = 1) in vec2 VertexUV;

out vec2 UV;

void main()
{
    gl_Position = vec4(VertexPosition, 0.0, 1.0);
	UV = VertexUV;
}
