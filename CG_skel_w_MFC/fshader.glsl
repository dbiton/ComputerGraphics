#version 150

#define MAX_NUM_LIGHTS 64

struct UniformMaterial{
	// colors
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float ambient_reflect;
	float shininess;
	float roughness;
};

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
	float brightness;
	int isDirectional;
};

in vec3 fragViewPos;
in vec3 fragPosition;
in vec3 fragNormalGouraud;
in vec3 fragNormal;
in vec2 fragTex;

in vec4 gouraudLightColorOut;

uniform UniformMaterial material;

uniform Light lights[MAX_NUM_LIGHTS];
uniform int numLights;

uniform vec4 inColor;
uniform int shadeGouraud;

out vec4 fColor;

vec3 calcLight(vec3 normal, Light light)
{
	// ambient	
	vec3 ambient = material.ambient_reflect * material.ambient * light.ambient;

	// diffuse 
	vec3 norm = normalize(fragNormal);
	vec3 lightDir;
	if (light.isDirectional == 1){
		lightDir = normalize(light.position);
	}
	else{
		lightDir = normalize(light.position - fragPosition);
	}
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * material.roughness * material.diffuse * light.diffuse;

	// specular
	float specularStrength = 1.0; // this is set higher to better show the effect of Gouraud shading 
	vec3 viewDir = normalize(fragViewPos - fragPosition);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = (1.0 - material.roughness) * specularStrength * spec * material.specular * light.specular;      
	
	return light.brightness * (ambient + diffuse + specular);
}

void shade(vec3 normal)
{
	vec3 c = vec3(0,0,0);
	for(int i=0; i<numLights; i++){
		c += calcLight(normal, lights[i]);
	}
	fColor = vec4(c, 1.0);
}

void main() 
{
    if (inColor == vec4(0)){
		if (shadeGouraud == 0){
			shade(fragNormal);
		}
		else{
			shade(fragNormalGouraud);
		}
	}
    else fColor = inColor;
} 