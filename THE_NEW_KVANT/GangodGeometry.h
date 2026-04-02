#pragma once
#include <INCLUDE/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <iostream>
#include <vector>
using namespace std;

class Vertex {
public:
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 texture;
	glm::vec3 normal;

	Vertex(glm::vec3 pos, glm::vec3 col, glm::vec2 tex, glm::vec3 norm) {
		position = pos;
		color = col;
		texture = tex;
		normal = norm;
	}
	Vertex() {

	}
};

class Mesh {
public:
	vector<Vertex> vertices;
	vector<int> indices;

	Mesh(vector<Vertex> verts, vector<int> indices);
	Mesh();
	void Construct();
	~Mesh();

	void Render();
private:
	unsigned int VBO, VAO, EBO;


};

