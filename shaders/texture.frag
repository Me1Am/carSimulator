#version 330 core

out vec4 FragColor;
  
in vec2 TexCoord;

uniform vec3 objectColor;
uniform vec3 lightColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    FragColor = vec4(lightColor * objectColor, 1) * mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
	
	//FragColor = vec4(lightColor * objectColor, 1);

    // Rainbow
    // FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2) * vec4(objectColor, 1.0);
}