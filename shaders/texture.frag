#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 cameraPos;
uniform vec3 lightPos;

struct Material {
	//vec3 ambient;
	//vec3 diffuse;
	
	sampler2D baseTexture;	// Base texture
	sampler2D decal;		// Decal
	
	vec3 specular;
	float shininess;
	float decalBias;	// "Weight" of the decal
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

void main() {
	// Ambient Lighting
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * light.ambient * vec3(mix(texture(material.baseTexture, TexCoord), texture(material.decal, TexCoord), material.decalBias));

	// Diffuse Lighting
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(normalize(Normal), lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(mix(texture(material.baseTexture, TexCoord), texture(material.decal, TexCoord), material.decalBias));	// Use decal

	// Specular Lighting
	float shininess = 32;
	float specularStrength = 0.5;
	vec3 cameraDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, normalize(Normal));
	vec3 specular = specularStrength * light.specular * (material.specular * pow(max(dot(cameraDir, reflectDir), 0.0), material.shininess)); 

	vec4 lightResult = vec4((ambient + diffuse + specular), 1);
	
	FragColor = lightResult;
}