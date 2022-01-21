#version 150

in vec4 fragmentColor;
out vec4 fColor;
uniform vec4 inColor;

void main() 
{
    if (inColor == vec4(0)) fColor = fragmentColor;
    else fColor = inColor;
} 