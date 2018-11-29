
#version 430

in vec2 UV;

out vec4 FragmentColour;

uniform sampler2D ColourTexture;

void main(void)
{
    FragmentColour = texture(ColourTexture, UV);
}
