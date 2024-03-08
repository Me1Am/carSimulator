#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 cameraPos;
uniform vec3 lightPos;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
	// Ambient Lighting
	float ambientStrength = 0.1f;
	vec3 ambientColor = lightColor * ambientStrength;

	// Diffuse Lighting
	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 diffuse = max(dot(normalize(Normal), lightDir), 0.0) * lightColor;

	// Specular Lighting
	float shininess = 32;
	float specularStrength = 0.5;
	vec3 cameraDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, normalize(Normal));
	vec3 specular = specularStrength * pow(max(dot(cameraDir, reflectDir), 0.0), shininess) * lightColor; 

	vec4 lightResult = vec4(objectColor * (ambientColor + diffuse + specular), 1);

	FragColor = lightResult;

	//FragColor = lightResult * mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);

    // Rainbow
    // FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2) * vec4(objectColor, 1.0);
}