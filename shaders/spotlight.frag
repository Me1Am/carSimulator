#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 cameraPos;
uniform vec3 lightPos;

struct Material {
	sampler2D baseTexture;	// Base texture
	sampler2D decal;		// Decal
	sampler2D specMap;		// Specular Map

	float shininess;
	float decalBias;	// "Weight" of the decal
};

struct Light {
    vec3 position;
	vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
	float linear;
	float quadratic;

	float cutOff;		// Cos of the angle from camera
	float outerCutOff;	// Larger cutOff value representing the fade cone
};

uniform Material material;
uniform Light light;

void main() {
	vec3 lightDir = normalize(light.position - FragPos);		// Vector from FragPos to the light
	float theta = dot(lightDir, normalize(-light.direction));	// Angle between lightDir and the light facing vector
	float epsilon   = light.cutOff - light.outerCutOff;							// Cos diff between the inner and outer cones(used for soft edges)
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);	// Intensity of the spotlight(used for soft edges)

	vec4 lightResult;
	if(theta > light.outerCutOff){
		float distance = length(light.position - FragPos);
		float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));   
		
		// Ambient Lighting
		float ambientStrength = 0.1f;
		vec3 ambient = ambientStrength * light.ambient * vec3(mix(texture(material.baseTexture, TexCoord), texture(material.decal, TexCoord), material.decalBias));

		// Diffuse Lighting
		float diff = max(dot(normalize(Normal), lightDir), 0.0);
		vec3 diffuse = light.diffuse * diff * vec3(mix(texture(material.baseTexture, TexCoord), texture(material.decal, TexCoord), material.decalBias));	// Use decal

		// Specular Lighting
		float specStrength = 1.;
		vec3 cameraDir = normalize(cameraPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, normalize(Normal));
		float spec = pow(max(dot(cameraDir, reflectDir), 0.0), material.shininess);
		vec3 specular = specStrength * light.specular * spec * vec3(texture(material.specMap, TexCoord));

		lightResult = vec4(ambient + (attenuation * intensity * (diffuse + specular)), 1.0);	// Ambient colors are uneffected by distance and light
	} else {
		float ambientStrength = 0.5f;
		vec3 ambient = ambientStrength * light.ambient * vec3(mix(texture(material.baseTexture, TexCoord), texture(material.decal, TexCoord), material.decalBias));
		lightResult = vec4(ambient, 1.0);
	}

	FragColor = lightResult;
}