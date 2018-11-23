// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

in vec2 UV;

out vec4 FragmentColour;

//uniform sampler2D ColourTexture;

void main(void)
{
    //FragmentColour = texture(ColourTexture, UV);
    FragmentColour = vec4(UV, 0.0, 1.0);
}
