#version 150

in vec3 frag_normal;
in vec3 frag_position;
in vec3 fPosition;
in vec4 gouraudLightColorOut;
flat in vec3 frag_fPosition;
flat in vec3 frag_fNormal;

uniform vec3 viewPos;
uniform vec4 inColor;
uniform int shadeStyle;

out vec4 fColor;

void shade(vec3 Normal, vec3 Position)
{
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
	
	fColor = vec4(ambient + diffuse + specular, 1.0);
}

void main() 
{
    if (inColor == vec4(0)) {
		if (shadeStyle == 0) fColor = gouraudLightColorOut * vec4(1, 1, 1, 1); // gouraud
		else if (shadeStyle == 1) shade(frag_fNormal, frag_fPosition); // flat
		else shade(frag_normal, frag_position); // phong
	}
    else fColor = inColor;
} 