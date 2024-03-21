#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

struct Material {
	sampler2D baseTexture;	// Base texture
	sampler2D decal;		// Decal
	sampler2D specMap;		// Specular map
	
	vec3 ambient;			// Percent(0-1) of base color
	
	float shininess;		// "Shininess" of specular shading
	float decalBias;		// "Weight" of the decal
};

uniform Material material;

void main() {
	vec3 textureColor = vec3(mix(texture(material.baseTexture, TexCoord), texture(material.decal, TexCoord), material.decalBias));

	FragColor = vec4(textureColor, 1.0);
}