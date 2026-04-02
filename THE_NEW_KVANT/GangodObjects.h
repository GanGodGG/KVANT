#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <type_traits>
#include "GangodGeometry.h"
#include "G_SHADER_INSTANCE.h"
#include "ObjectLoader.h"
#define Log(x) { cout << x << endl; }
class EngObj;

class G_Object {
public:
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation; 

	virtual void Update();
	virtual void Rotate(float angle, glm::vec3 rot);
	virtual void Move(glm::vec3 move);
};

class Particle : public G_Object {
public:
	glm::vec2 position;
	glm::vec3 color;
	glm::vec2 scale;
	string particle_path;
	Particle(glm::vec2 pos, glm::vec2 scal, glm::vec3 col, string path_tex);
	void Update() override;
private:
	float lifeTime;
	unsigned int ShaderID;
};

class Light : public G_Object {
public:
	glm::vec3 color;
	enum LightType {
		POINT,
		DIRECTIONAL,
		SPOT
	} type;
};

class PointLight : public Light {
public:
	glm::vec3 pos;
	PointLight(glm::vec3 pos, glm::vec3 col) {
		color = col;
		type = LightType::POINT;
		this->pos = pos;
	}
};
class DirLight : public Light {
public:
	glm::vec3 dir;
	DirLight(glm::vec3 dir, glm::vec3 col) {
		color = col;
		type = LightType::DIRECTIONAL;
		this->dir = dir;
	}
};
class SpotLight : public Light {
public:
	glm::vec3 pos;
	glm::vec3 spotdir;
	float angle;
	float outerangle;
	
	SpotLight(glm::vec3 pos, glm::vec3 spotDir, glm::vec3 col, float angle, float outerAngle) {
		color = col;
		this->pos = pos;
		this->spotdir = spotDir;
		this->angle = angle;
		this->outerangle = outerAngle;
		type = LightType::SPOT;
	}


};

class ObjectManager {
public:
	void Add(G_Object* add);
	void Add(EngObj* add);
	void UpdateAll();
	void AddLightSrc(Light* light);
private:
	vector<G_Object*> OBJS;
	vector<EngObj*> RenderingObj;
};

class ParticleManager {
public:
	void Add(Particle* part);
	void UpdateAll();
private:
	vector<Particle*> particles;
};

class Camera : public G_Object {
public:
	float lastX = 400.0f;
	float lastY = 400.0f;
	//поворот влево вправо
	float yaw = -90.0f;
	//поворот вверх вниз
	float pitch;
	//бесполезна в большинстве своём, но иногда всё-же используется
	float roll;
	glm::vec3 forward;
	glm::mat4 view;
	glm::mat4 projection;
	float Scr_aspect;

	glm::vec3 amb_color = glm::vec3(0.2f, 0.2f, 0.2f);

	Camera(glm::vec3 UP, glm::vec3 forward, float Aspect, ObjectManager& ObjMGR);
	Camera(glm::vec3 UP, glm::vec3 forward, float Aspect, glm::vec3 pos, ObjectManager& ObjMGR);

	Camera(glm::vec3 UP, glm::vec3 forward, float Aspect, float FOV, ObjectManager& ObjMGR);
	Camera(glm::vec3 UP, glm::vec3 forward, float Aspect, glm::vec3 pos, float FOV, ObjectManager& ObjMGR);

	Camera(glm::vec3 UP, glm::vec3 forward, float Aspect, float FOV, float minDepth, float MaxDepth, ObjectManager& ObjMGR);
	Camera(glm::vec3 UP, glm::vec3 forward, float Aspect, glm::vec3 pos, float FOV, float minDepth, float MaxDepth, ObjectManager& ObjMGR);

	void Update() override;

	enum Camera_Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UPWARD,
		DOWNWARD
	};
	void Move(Camera_Movement direction, float deltaTime);
	void Rotate(float angle, glm::vec3 rot) override;

private:
	glm::vec3 up;
};

class EngObj : public G_Object {
public:
	Mesh* mesh;
	Camera* cam;
	string name;
	unsigned int ShaderID;
	G_SHADER_INSTANCE* GSI;
	glm::vec3 color;


	EngObj(string ObjectPath, glm::vec3 __position, string __name, Camera* cam, ObjectManager& ObjMGR);
	EngObj(string ObjectPath, glm::vec3 __position, string __name, Camera* cam, string VertShader, string FragShader, ObjectManager& ObjMGR);
	EngObj(string ObjectPath, glm::vec3 __position, string __name, Camera* cam, string VertShader, string FragShader, string TexPath, ObjectManager& ObjMGR);
	EngObj(string ObjectPath, glm::vec3 __position, string __name, Camera* cam, string VertShader, string FragShader, glm::vec3 color, ObjectManager& ObjMGR);
	~EngObj();

	void Update() override;
	void Render();
	void Rotate(float angle, glm::vec3 rot) override;
	void Move(glm::vec3 move) override;
	void Rescale(glm::vec3 scale);

	void AddLightSource(Light* light);
private:
	float angle;
	glm::mat4 model;
	vector<SpotLight*> SpotLights;
	vector<PointLight*> PointLights;
	DirLight* mainLight;
};