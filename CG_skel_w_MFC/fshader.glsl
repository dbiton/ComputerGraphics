#version 150

in vec4 lightColorOut;
out vec4 fColor;
uniform vec4 inColor;

void main() 
{
    if (inColor == vec4(0)){
		fColor = lightColorOut * vec4(1, 1, 1, 1);
	}
    else fColor = inColor;
} 