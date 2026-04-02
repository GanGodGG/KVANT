#pragma once
#include <INCLUDE/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

class G_SHADER_INSTANCE
{
public:
	unsigned int __tex;
	G_SHADER_INSTANCE(const char* path_vert, const char* path_frag) {
		this->vertPath = path_vert;
		this->fragPath = path_frag;
		__tex = 0;
	}
	G_SHADER_INSTANCE(const char* path_vert, const char* path_frag, const char* path_tex) {
		this->vertPath = path_vert;
		this->fragPath = path_frag;
		this->texPath = path_tex;
		__tex = 0;
	}
	G_SHADER_INSTANCE(string path_vert, string path_frag) {
		this->vertPath = path_vert;
		this->fragPath = path_frag;
		__tex = 0;
	}
	G_SHADER_INSTANCE(string path_vert, string path_frag, string path_tex) {
		this->vertPath = path_vert;
		this->fragPath = path_frag;
		this->texPath = path_tex;
		__tex = 0;
	}

	~G_SHADER_INSTANCE() {
		cout << "ENDED WORK GSI: " << this << endl;
	}

	void CreateShader(unsigned int& ID);
private:
	string vertPath;
	string fragPath;
	string texPath;
};

