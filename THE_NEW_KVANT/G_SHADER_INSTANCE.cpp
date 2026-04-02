#include "G_SHADER_INSTANCE.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
void G_SHADER_INSTANCE::CreateShader(unsigned int& ID)
{
	string Code_vert;
	string Code_frag;

	ifstream Stream_vert;
	ifstream Stream_frag;

	Stream_vert.open(PROJECT_DIR + vertPath);
	Stream_frag.open(PROJECT_DIR + fragPath);

	stringstream SS_vert, SS_frag;

	SS_vert << Stream_vert.rdbuf();
	SS_frag << Stream_frag.rdbuf();

	Stream_vert.close();
	Stream_frag.close();

	Code_vert = SS_vert.str();
	Code_frag = SS_frag.str();

	const char* Clean_vert = Code_vert.c_str();
	const char* Clean_frag = Code_frag.c_str();

	unsigned int vert_Shader = glCreateShader(GL_VERTEX_SHADER);
	int sucess;
	char info[512];
	glShaderSource(vert_Shader, 1, &Clean_vert, NULL);
	glCompileShader(vert_Shader);

	glGetShaderiv(vert_Shader, GL_COMPILE_STATUS, &sucess);

	if (!sucess) {
		glGetShaderInfoLog(vert_Shader, 512, NULL, info);
		cerr << "CLOUDN'T COMPILE!, love, new GSI! (VERTEX) " << info << endl;
	}
	unsigned int frag_Shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_Shader, 1, &Clean_frag, NULL);
	glCompileShader(frag_Shader);

	glGetShaderiv(frag_Shader, GL_COMPILE_STATUS, &sucess);

	if (!sucess) {
		glGetShaderInfoLog(frag_Shader, 512, NULL, info);
		cerr << "CLOUDN'T COMPILE!, love, new GSI! (FRAGMENT) " << info << endl;
	}

	ID = glCreateProgram();
	glAttachShader(ID, vert_Shader);
	glAttachShader(ID, frag_Shader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &sucess);
	if (!sucess) {
		glGetProgramInfoLog(ID, 512, NULL, info);
		cerr << "CLOUDN'T COMPILE!, love, new GSI!" << info << endl;
	}

	glDeleteShader(vert_Shader);
	glDeleteShader(frag_Shader);

	if (texPath != "") {
		int H, W, C_CH;
		unsigned char* data = stbi_load((PROJECT_DIR + texPath).c_str(), &W, &H, &C_CH, 0);
		glGenTextures(1, &__tex);
		glBindTexture(GL_TEXTURE_2D, __tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if (data) {
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			cerr << "Cant load tex. Love, GSI." << endl;
		}

		stbi_image_free(data);
	}
}
