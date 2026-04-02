#version 330 core


out vec4 FragColor;

in vec3 Color;
in vec3 Pos;

uniform sampler2D Texture;
void main(){
	FragColor = vec4(texture(Texture, Pos).xyz * Color);
}