#version 150

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;

uniform vec3 lightPos;
uniform vec3 lightColorAmbient;
uniform vec3 lightColorSpecular;
uniform vec3 lightColorDiffuse;

uniform vec3 vColorAmbient;
uniform vec3 vColorSpecular;
uniform vec3 vColorDiffuse;

out vec4 lightColorOut;

void main()
{
   gl_Position = projection * view * model * vec4(vPosition, 1.0);

	// gouraud
	vec3 Position = vec3(model * vec4(vPosition, 1.0));
	vec3 Normal = mat3(transpose(inverse(model))) * vNormal;

	// ambient
	float ambientStrength = 0.1;
	
	vec3 lightColor = vec3(1,1,0);
	vec3 lightPos = vec3(2,1,2);
	
	vec3 ambient = ambientStrength * lightColor;

	// diffuse 
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - Position);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// specular
	float specularStrength = 1.0; // this is set higher to better show the effect of Gouraud shading 
	vec3 viewDir = normalize(viewPos - Position);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;      

	lightColorOut = vec4(ambient + diffuse + specular, 1.0);
} 