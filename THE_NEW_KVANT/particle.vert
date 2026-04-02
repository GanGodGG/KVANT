#version 330 core

layout (location=0) in vec2 Scale;
layout (location=1) in vec3 Position;
layout (location=2) in vec3 Color;

uniform mat4 view;
uniform mat4 projection;

out vec3 PART_COL;
void main(){
	gl_Position = projection * view * vec4(Position, 1.0f);
	PART_COL = Color;
}