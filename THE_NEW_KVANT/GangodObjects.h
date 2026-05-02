#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <type_traits>
#include <array>
#include "GangodGeometry.h"
#include "G_SHADER_INSTANCE.h"
#include "ObjectLoader.h"
#define Log(x) { cout << x << endl; }
class EngObj;
class G_Object;
class PhysBody;
class Light;
class Component;
class Hitbox;
struct Aspect {
public:
	static float aspect;
	static glm::mat4 view;
	static glm::mat4 projection;
	static float GetAspect() {
		return aspect;
	}

	static void SetAspect(float newAspect) {
		aspect = newAspect;
	}

	static void ChangeView(glm::mat4 newView) {
		view = newView;
	}

	static void ChangeProjection(glm::mat4 newProj) {
		projection = newProj;
	}
};

class Component {
public:
	virtual void Update();
	virtual void OnFix(float deltatime);
	void AddIns(G_Object *obj);
protected:
	G_Object* instance;
};

class ObjectManager {
public:
	static void Add(G_Object* add);
	static void Add(EngObj* add);
	static void UpdateAll();
	static void UpdateAllFix(float delta);
	static void AddLightSrc(Light* light);

	static ObjectManager& GetInstance() {
		static ObjectManager instance;
		return instance;
	}

	vector<G_Object*> GetAllObjects() {
		return this->OBJS;
	}

private:
    static vector<G_Object*> OBJS;
	static vector<EngObj*> RenderingObj;

	ObjectManager() {}
	ObjectManager(const ObjectManager&) = delete;
	ObjectManager& operator=(const ObjectManager&) = delete;
};

class G_Object {
public:
	glm::vec3 position;
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 rotation; 

	virtual void Update();
	virtual void OnFixUpadte(float deltatime);
	virtual void Rotate(float angle, glm::vec3 rot);
	virtual void Move(glm::vec3 move);

	bool operator ==(G_Object* obj2)
	{
		return position == obj2->position && scale == obj2->scale && rotation == obj2->rotation;
	}

	template<typename T> T* AddComponent();
	template<typename T> T* GetComponent();
protected:
	vector<Component*> components;

	G_Object(){
		ObjectManager::Add(this);
	}
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
	using Light::Light;

	glm::vec3 dir;
	DirLight(glm::vec3 dir, glm::vec3 col) {
		color = col;
		type = LightType::DIRECTIONAL;
		this->dir = dir;
	}
};
class SpotLight : public Light {
public:
	using Light::Light;

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



class ParticleManager {
public:
	void Add(Particle* part);
	void UpdateAll();

private:
	vector<Particle*> particles;
};

class Camera : public G_Object {
public:
	using G_Object::G_Object;

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

	Camera(glm::vec3 UP, glm::vec3 forward, float Aspect);
	Camera(glm::vec3 UP, glm::vec3 forward, float Aspect, glm::vec3 pos);

	Camera(glm::vec3 UP, glm::vec3 forward, float Aspect, float FOV);
	Camera(glm::vec3 UP, glm::vec3 forward, float Aspect, glm::vec3 pos, float FOV);

	Camera(glm::vec3 UP, glm::vec3 forward, float Aspect, float FOV, float minDepth, float MaxDepth);
	Camera(glm::vec3 UP, glm::vec3 forward, float Aspect, glm::vec3 pos, float FOV, float minDepth, float MaxDepth);

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
	using G_Object::G_Object;

	Mesh* mesh;
	Camera* cam;
	string name;
	unsigned int ShaderID;
	G_SHADER_INSTANCE* GSI;
	glm::vec3 color;


	EngObj(string ObjectPath, glm::vec3 __position, string __name, Camera* cam);
	EngObj(string ObjectPath, glm::vec3 __position, string __name, Camera* cam, string VertShader, string FragShader);
	EngObj(string ObjectPath, glm::vec3 __position, string __name, Camera* cam, string VertShader, string FragShader, string TexPath);
	EngObj(string ObjectPath, glm::vec3 __position, string __name, Camera* cam, string VertShader, string FragShader, glm::vec3 color);
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

struct Face {
	glm::vec3 normal;
	vector<Vertex> verts;
};

class SimpleX {
public:
	vector<glm::vec3> pts;

	void push_front(glm::vec3 point) {
		pts.insert(pts.begin(), point);
		if (pts.size() > 4) pts.pop_back();
	}

	int size() const { return pts.size(); }

	glm::vec3& operator[](int index) {
		return pts[index];
	}
	SimpleX* operator = (const initializer_list<glm::vec3>& list) {
		pts = list;
		return this;
	}
};


class Collider : public Component {
public:
	glm::vec3 size = glm::vec3(1.25f);
	glm::vec3 pos;
	glm::vec3 offset = glm::vec3(0);
	glm::mat4 model = glm::mat4();
	Collider();
	bool CollisionDetect(glm::vec3 dir, glm::vec3& normal);
	
protected:
	glm::vec3 GetSupport(Collider* a, Collider* b, glm::vec3 direction);
	virtual glm::vec3 SupportFunc(glm::vec3 direction) = 0;
	bool SameDirection(const glm::vec3& direction, const glm::vec3& ao) {
		return glm::dot(direction, ao) > 0;
	}

	bool Line(SimpleX& points, glm::vec3& direction);

	bool Triangle(SimpleX& points, glm::vec3& direction);

	bool Tetrahedron(SimpleX& points, glm::vec3& direction);
	bool NextSimplex(SimpleX& points, glm::vec3& direction) {
		switch (points.size()) {
		case 2: return Line(points, direction);
		case 3: return Triangle(points, direction);
		case 4: return Tetrahedron(points, direction);
		}
		return false;
	}
};


class BoxCollider : public Collider {
public:
	BoxCollider();

protected:
	glm::vec3 SupportFunc(glm::vec3 direction) override;
};

struct CollideInfo {
	glm::vec3 thisPos;
};
class PhysBody : public Component {
public:
	bool atX, atY, atZ;
	glm::vec3 Velocity;

	float mass = 10.0f;
	float drag = 4.0f;
	
	void Update() override;
	void OnFix(float deltatime);
private:
	glm::vec3 info;
	float elapsedTime = 0.0f;
	float g = 9.81f;
};



