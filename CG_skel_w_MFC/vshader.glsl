#version 150

#define MAX_NUM_LIGHTS 64

struct UniformMaterial {
	// colors
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 emissive;
	
	float shininess;
	float roughness;
};

struct Light {
	vec3 color;
	float brightness;
	int type; // 0 - ambient, 1 - point, 2 - parallel
	vec3 posOrDir;
};

in vec3 vPosition;
in vec3 vNormal;
in vec3 fNormal;
in vec2 vTex;

uniform mat4 model;
uniform mat4 modelInverse;
uniform mat4 view;
uniform mat4 projection;
uniform int animationEffect; // 0 - none, 1 - subtle, 2 - overdone
uniform float time;
uniform int shading; // 0 - flat, 1 - gouraud, 2 - phong
uniform vec3 viewPos;
uniform sampler2D textureData;
uniform int hasTexture;
uniform UniformMaterial material;
uniform Light lights[MAX_NUM_LIGHTS];
uniform int numLights;
uniform int colorEffect; // 0 - none, 1 - hue, 2 - stripes, 3 - crazy stuff
uniform int marbleEffect;
uniform int toonEffect; // 0 - no toon effect, 1+ - number of toon hue seperations (the bigger the more hues)
uniform int backshadows;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTex;
out vec3 fragViewPos;
out vec3 vertPosition;
out vec4 gouraudColor;

// Yes, just for gouraud we need to literally copypaste the entire
// color calculating code from the fragment shader into here. Deal with it.

float mod289(float x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec4 mod289(vec4 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec4 perm(vec4 x) { return mod289(((x * 34.0) + 1.0) * x); }

float noise(vec3 p){
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}

vec3 calcLight(vec3 normal, Light light)
{
	vec3 color = material.emissive;
	if (light.type == 0) { // ambient
		color = material.ambient * light.color * material.ambient * light.brightness;
	}
	else {
		// diffuse 
		vec3 norm = normalize(normal);
		vec3 lightDir;
		if (light.type == 1) lightDir = light.posOrDir;
		else lightDir = normalize(light.posOrDir - fragPosition);
		float angle = dot(norm, lightDir);
		vec3 diffuse = vec3(0), specular = vec3(0);
		if (angle > 0 || backshadows == 1) {
			diffuse = angle * material.roughness * material.diffuse * light.color;
			
			// specular
			vec3 viewDir = normalize(viewPos - fragPosition);
			vec3 reflectDir = reflect(-lightDir, norm);
			float spec = pow(max(-dot(viewDir, reflectDir), 0.0), material.shininess);
			specular = (1.0 - material.roughness) * spec * material.specular * light.color;      
		}
		color = light.brightness * (diffuse + specular);
	}
	if (colorEffect == 1) {
		color = color * vec3(sin(time), cos(time), -sin(time));
	}
	else if (colorEffect == 2) {
		color = color * vec3(sin(time*fragPosition.x), cos(time*fragPosition.y), -sin(time*fragPosition.z));
	}
	else if (colorEffect == 3) {
		color = color * vec3(sin(sin(time*time*fragPosition.x*fragPosition.y*fragPosition.z)), cos(time*cos(time*time*fragPosition.y*fragPosition.y)), cos(time-sin(time*time*time*fragPosition.z*fragPosition.z*fragPosition.z)));
	}
	
	if (toonEffect > 0) {
		float a = 1.0 / toonEffect;
		color.x = a * floor(color.x / a);
		color.y = a * floor(color.y / a);
		color.z = a * floor(color.z / a);
	}
	
	return color;
}

vec4 shade(vec3 normal)
{
	vec3 c = vec3(0,0,0);
	for(int i=0; i<numLights; i++){
		c += calcLight(normal, lights[i]);
	}
	vec4 color = vec4(c, 1.0);
	if (hasTexture > 0){
		color = texture(textureData, fragTex)* color;
	}
	if (marbleEffect == 0){
		float x = noise(vertPosition*10)+vertPosition.x;
		color = color * sin(x);
	}
	return color;
}

void main()
{
    gl_Position = projection * view * model * vec4(vPosition, 1.0);
	
	if (animationEffect == 1) {
		gl_Position += 0.2 * vec4(sin(gl_Position.x*time), cos(gl_Position.y*time), -sin(gl_Position.z*time), 0);
	}
	else if (animationEffect == 2) {
		gl_Position *= vec4(sin(time*vPosition.x), cos(tan(gl_Position.x*time)*time), -sin(time*sin(time*vPosition.z)), 1);
	}
    vertPosition = vPosition;
    fragPosition = (model * vec4(vPosition, 1.0)).xyz;
    fragTex = vTex;
    //fragViewPos = vec3(model * vec4(viewPos, 1.0));
	
    if (shading == 0) fragNormal = (modelInverse * vec4(fNormal, 1.0)).xyz;
    else if (shading == 1) gouraudColor = shade(vNormal);
    else fragNormal = (modelInverse * vec4(vNormal, 1.0)).xyz;
} 