#version 150

#define MAX_NUM_LIGHTS 64

uniform sampler2D textureData;
uniform int hasTexture;

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

uniform float time;
uniform int colorEffect; // 0 - none, 1 - hue, 2 - stripes, 3 - crazy stuff

uniform int toonEffect; // 0 - no toon effect, 1+ - number of toon hue seperations (the bigger the more hues)

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
	
	vec3 color = light.brightness * (ambient + diffuse + specular);
	
	if (colorEffect == 1){
		color = color * vec3(sin(time), cos(time), -sin(time));
	}
	else if (colorEffect == 2){
		color = color * vec3(sin(time*fragPosition.x), cos(time*fragPosition.y), -sin(time*fragPosition.z));
	}
	else if (colorEffect == 3){
		color = color * vec3(sin(sin(time*time*fragPosition.x*fragPosition.y*fragPosition.z)), cos(time*cos(time*time*fragPosition.y*fragPosition.y)), cos(time-sin(time*time*time*fragPosition.z*fragPosition.z*fragPosition.z)));
	}
	
	if (toonEffect > 0){
		float a = 1.0 / toonEffect;
		color.x = a * floor(color.x / a);
		color.y = a * floor(color.y / a);
		color.z = a * floor(color.z / a);
	}
	
	return color;
}

void shade(vec3 normal)
{
	vec3 c = vec3(0,0,0);
	for(int i=0; i<numLights; i++){
		c += calcLight(normal, lights[i]);
	}
	fColor = vec4(c, 1.0);
	if (hasTexture > 0){
		fColor = texture(textureData, fragTex); // * fColor;
	}
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