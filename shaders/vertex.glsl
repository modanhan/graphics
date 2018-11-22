// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

layout(location = 0) in vec2 VertexPosition;
layout(location = 1) in vec3 VertexUV;

out vec3 UV;

void main()
{
    gl_Position = vec4(VertexPosition, 0.0, 1.0);

    UV = VertexUV;
}
