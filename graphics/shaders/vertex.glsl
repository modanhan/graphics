// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 430

layout(location = 0) in vec2 VertexPosition;

out vec2 UV;

void main()
{
    gl_Position = vec4(VertexPosition, 0.0, 1.0);
}
