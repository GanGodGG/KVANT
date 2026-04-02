#version 330 core
out vec4 FragColor;

in vec3 Color;
in vec2 TexPos;

uniform vec3 COLOR_2;
uniform sampler2D Texture;
void main()
{
FragColor = vec4(COLOR_2, 1.0f);
}