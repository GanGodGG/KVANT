#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 _color;
layout (location = 2) in vec2 _texPos;
layout (location = 3) in vec3 _Normal;
uniform float Aspect;
out vec3 ourColor;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
void main()
{
	gl_Position = projection * view * model * vec4(aPos / Aspect, 1.0f);
	ourColor = _color;
}