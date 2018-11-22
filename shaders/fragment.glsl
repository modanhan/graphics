// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

in vec3 UV;

out vec4 FragmentColour;

void main(void)
{
    FragmentColour = vec4(UV, 0);
}
