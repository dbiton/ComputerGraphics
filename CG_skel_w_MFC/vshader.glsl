#version 150

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;

out vec4 gouraudLightColorOut;

out vec3 fragPosition;
out vec3 fragNormal;
out vec3 fragNormalGouraud;
out vec2 fragTex;
out vec3 fragViewPos;

uniform int animationEffect; // 0 - none, 1 - subtle, 2 - overdone

uniform float time;

void main()
{
   gl_Position = projection * view * model * vec4(vPosition, 1.0);
	
	if (animationEffect == 1){
		gl_Position += 0.2 * vec4(sin(gl_Position.x*time), cos(gl_Position.y*time), -sin(gl_Position.z*time), 0);
	}
	else if (animationEffect == 2){
		gl_Position *= vec4(sin(time*vPosition.x), cos(tan(gl_Position.x*time)*time), -sin(time*sin(time*vPosition.z)), 1);
	}
  
   fragPosition = vec3(model * vec4(vPosition, 1.0));
   fragNormalGouraud = mat3(transpose(inverse(model))) * vNormal;
   fragNormal = vNormal;
   fragTex = vTex;
   fragViewPos = vec3(model * vec4(viewPos, 1.0));
} 