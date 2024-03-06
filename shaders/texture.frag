#version 330 core

out vec4 FragColor;
  
in vec2 TexCoord;

uniform vec3 objectColor;
uniform vec3 lightColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
	// Ambient Lighting
	float ambientStrength = 0.1f;
	vec3 ambientColor = lightColor * ambientStrength;

	vec3 result = ambientColor * objectColor;
	FragColor = vec4(result, 1);
    
	//FragColor = vec4(lightColor * objectColor, 1) * mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);

    // Rainbow
    // FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2) * vec4(objectColor, 1.0);
}