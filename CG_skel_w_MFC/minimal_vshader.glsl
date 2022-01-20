#version 150

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTex;
uniform mat4 modelview;

void main()
{
   gl_Position = modelview * vec4(vPosition, 1.0);
} 