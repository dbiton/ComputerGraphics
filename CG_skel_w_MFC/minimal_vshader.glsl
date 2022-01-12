#version 150

in vec4 vPosition;
uniform mat4 modelview;

void main()
{
   gl_Position = modelview * vPosition;
}