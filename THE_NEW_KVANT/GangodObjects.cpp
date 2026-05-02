#include "GangodObjects.h"
std::vector<G_Object*> ObjectManager::OBJS;
std::vector<EngObj*> ObjectManager::RenderingObj;




EngObj::EngObj(string ObjectPath, glm::vec3 __position, string __name, Camera* cam)
{
	ObjectManager::Add(this);

	this->position = __position;
	this->name = __name;
	this->cam = cam;

	GSI = new G_SHADER_INSTANCE("shader.vert", "shader.frag");
	GSI->CreateShader(ShaderID);

	mesh = new Mesh();
	ObjectLoader OL;
	model = glm::translate(glm::mat4(1.0f), glm::vec3(__position));
	OL.LoadModel(ObjectPath, *mesh);
	mesh->Construct();
}

EngObj::EngObj(string ObjectPath, glm::vec3 __position, string __name, Camera* cam, string VertShader, string FragShader) 
{
	ObjectManager::Add(this);

	this->position = __position;
	this->name = __name;
	this->cam = cam;

	GSI = new G_SHADER_INSTANCE(VertShader, FragShader);
	GSI->CreateShader(ShaderID);

	mesh = new Mesh();
	ObjectLoader OL;

	model = glm::translate(glm::mat4(1.0f), glm::vec3(__position));
	OL.LoadModel(ObjectPath, *mesh);
	mesh->Construct();
}

EngObj::EngObj(string ObjectPath, glm::vec3 __position, string __name, Camera* cam, string VertShader, string FragShader, string TexPath)
{
	ObjectManager::Add(this);

	this->position = __position;
	this->name = __name;
	this->cam = cam;

	GSI = new G_SHADER_INSTANCE(VertShader, FragShader, TexPath);
	GSI->CreateShader(ShaderID);

	mesh = new Mesh();
	ObjectLoader OL;
	this->model = glm::translate(glm::mat4(1.0f), glm::vec3(__position));
	OL.LoadModel(ObjectPath, *mesh);
	mesh->Construct();
}

EngObj::EngObj(string ObjectPath, glm::vec3 __position, string __name, Camera* cam, string VertShader, string FragShader, glm::vec3 color)
{
	ObjectManager::Add(this);

	this->position = __position;
	this->name = __name;
	this->cam = cam;

	GSI = new G_SHADER_INSTANCE(VertShader, FragShader);
	GSI->CreateShader(ShaderID);

	mesh = new Mesh();
	ObjectLoader OL;
	this->model = glm::translate(glm::mat4(1.0f), glm::vec3(__position));
	OL.LoadModel(ObjectPath, *mesh);
	mesh->Construct();
	this->color = color;
}

EngObj::~EngObj()
{
	mesh->~Mesh();
	GSI->~G_SHADER_INSTANCE();
	delete GSI;
	delete mesh;
}

void EngObj::Update()
{
	for (auto& comp : components) {
		comp->Update();
	}
	model = glm::scale(model, this->scale);
	return;
}



void EngObj::Render() 
{
	glUseProgram(ShaderID);
	int uni_asp = glGetUniformLocation(ShaderID, "Aspect");
	glUniform1f(uni_asp, cam->Scr_aspect);
	int uni_model = glGetUniformLocation(ShaderID, "model");
	int uni_view = glGetUniformLocation(ShaderID, "view");
	int uni_proj = glGetUniformLocation(ShaderID, "projection");

	glUniformMatrix4fv(uni_model, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(uni_view, 1, GL_FALSE, &(cam->view)[0][0]);
	glUniformMatrix4fv(uni_proj, 1, GL_FALSE, &(cam->projection)[0][0]);

	if (color != glm::vec3(0, 0, 0)) {
		int col_uni = glGetUniformLocation(ShaderID, "COLOR_2");
		glUniform3f(col_uni, color.x, color.y, color.z);
	}

	if (mainLight) {
		glUniform3f(glGetUniformLocation(ShaderID, "ambientLight"), cam->amb_color.x, cam->amb_color.y, cam->amb_color.z);
		glUniform3f(glGetUniformLocation(ShaderID, "CameraPos"), cam->position.x, cam->position.y, cam->position.z);
		//dir
		glUniform3f(glGetUniformLocation(ShaderID, "dl.color"), mainLight->color.x, mainLight->color.y, mainLight->color.z);
		glUniform3f(glGetUniformLocation(ShaderID, "dl.dir"), mainLight->dir.x, mainLight->dir.y, mainLight->dir.z);
		//points
		for (int i = 0; i < PointLights.size(); ++i) {
			PointLight* LT = PointLights[i];
			string start = "pl[" + to_string(i) + "]";
			glUniform3f(glGetUniformLocation(ShaderID, (start + ".color").c_str()), PointLights[i]->color.x, PointLights[i]->color.y, PointLights[i]->color.z);
			glUniform3f(glGetUniformLocation(ShaderID, (start + ".position").c_str()), PointLights[i]->pos.x, PointLights[i]->pos.y, PointLights[i]->pos.z);
		}
		//spots
		for (int i = 0; i < SpotLights.size(); ++i) {
			SpotLight* LT = SpotLights[i];
			string start = "sl[" + to_string(i) + "]";
			glUniform3f(glGetUniformLocation(ShaderID, (start + ".color").c_str()), SpotLights[i]->color.x, SpotLights[i]->color.y, SpotLights[i]->color.z);
			glUniform3f(glGetUniformLocation(ShaderID, (start + ".position").c_str()), SpotLights[i]->pos.x, SpotLights[i]->pos.y, SpotLights[i]->pos.z);
			glUniform3f(glGetUniformLocation(ShaderID, (start + ".spotDir").c_str()), SpotLights[i]->spotdir.x, SpotLights[i]->spotdir.y, SpotLights[i]->spotdir.z);
			glUniform1f(glGetUniformLocation(ShaderID, (start + ".angle").c_str()), SpotLights[i]->angle);
			glUniform1f(glGetUniformLocation(ShaderID, (start + ".outerAngle").c_str()), SpotLights[i]->outerangle);
		}

	}

	mesh->Render();
}

void EngObj::Rotate(float angle, glm::vec3 rot)
{
	this->angle += angle;
	rotation += rot;
	model = glm::rotate(glm::mat4(1.0f), this->angle, rotation);
}

template<typename T>
T* G_Object::AddComponent()
{
	Log("Added component.");
    static_assert(std::is_base_of<Component, T>::value, "GComponent error! AddComponent<T>, where [T] MUST inherit from [Component]");
    T* comp = new T();
	comp->AddIns(this);
    components.push_back(comp);
    return comp;
}

template<typename T>
T* G_Object::GetComponent()
{
	for(auto& comp : components) {
		T* casted = dynamic_cast<T*>(comp);
		
		if (casted) {

			return dynamic_cast<T*>(comp);
		}
	}
	return nullptr;
}

template PhysBody* G_Object::AddComponent<PhysBody>();
template BoxCollider* G_Object::AddComponent<BoxCollider>();

template PhysBody* G_Object::GetComponent<PhysBody>();
template BoxCollider* G_Object::GetComponent<BoxCollider>();

void EngObj::Move(glm::vec3 move)
{
	this->position += move;
	model = glm::translate(glm::mat4(1.0f), position);
}

void EngObj::Rescale(glm::vec3 scale)
{
	this->scale = scale;

}

void EngObj::AddLightSource(Light* light)
{
	switch (light->type) {
	case Light::DIRECTIONAL:
		Log("Added Dir!");
		mainLight = (DirLight*)light;
		break;
	case Light::POINT:
		Log("Added Point!");
		PointLights.push_back((PointLight*)light);
		break;
	case Light::SPOT:
		Log("Added Spot!");
		SpotLights.push_back((SpotLight*)light);
		break;
	}
}

Camera::Camera(glm::vec3 UP, glm::vec3 forward, float Aspect)
{
	this->up = UP;
	this->forward = forward;
	this->Scr_aspect = Aspect;

	view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
}

Camera::Camera(glm::vec3 UP, glm::vec3 forward, float Aspect, glm::vec3 pos) 
{
	this->up = UP;
	this->forward = forward;
	this->Scr_aspect = Aspect;
	this->position = pos;

	view = glm::lookAt(this->position, pos + forward, UP);
}

Camera::Camera(glm::vec3 UP, glm::vec3 forward, float Aspect, float FOV)
{
	this->up = UP;
	this->forward = forward;
	this->Scr_aspect = Aspect;

	view = glm::lookAt(this->position, glm::vec3(0.0f, 0.0f, 0.0f) + forward, UP);
	projection = glm::perspective(glm::radians(FOV), Aspect, 0.1f, 100.0f);
}

Camera::Camera(glm::vec3 UP, glm::vec3 forward, float Aspect, glm::vec3 pos, float FOV) 
{
	this->up = UP;
	this->forward = forward;
	this->Scr_aspect = Aspect;
	this->position = pos;

	view = glm::lookAt(this->position, pos + forward, UP);
	projection = glm::perspective(glm::radians(FOV), Aspect, 0.1f, 100.0f);
}

Camera::Camera(glm::vec3 UP, glm::vec3 forward, float Aspect, float FOV, float minDepth, float MaxDepth) 
{
	this->up = UP;
	this->forward = forward;
	this->Scr_aspect = Aspect;

	view = glm::lookAt(this->position, glm::vec3(0.0f, 0.0f, 0.0f) + forward, UP);
	projection = glm::perspective(glm::radians(FOV), Aspect, minDepth, MaxDepth);
}

Camera::Camera(glm::vec3 UP, glm::vec3 forward, float Aspect, glm::vec3 pos, float FOV, float minDepth, float MaxDepth) 
{
	this->up = UP;
	this->forward = forward;
	this->Scr_aspect = Aspect;
	this->position = pos;

	view = glm::lookAt(this->position, pos + forward, UP);
	projection = glm::perspective(glm::radians(FOV), Aspect, minDepth, MaxDepth);
}

void Camera::Update()
{
	this->view = glm::lookAt(this->position, this->position + this->forward, this->up);
}



void Component::AddIns(G_Object *obj)
{
	instance = obj;
}

void Particle::Update() {
	return;
}

void G_Object::Update() {
	return;
}

void G_Object::OnFixUpadte(float deltatime)
{
	for (auto& comp : components) {
		comp->OnFix(deltatime);
	}
}


void G_Object::Rotate(float angle, glm::vec3 rot)
{
}

void G_Object::Move(glm::vec3 move)
{
	position += move;
}


void Camera::Move(Camera_Movement direction, float deltaTime)
{
	float velocity = 5.0f * deltaTime; // Adjust '5.0f' to change speed

	// Calculate the Right vector (Local X-axis)
	glm::vec3 right = glm::normalize(glm::cross(forward, up));

	if (direction == FORWARD)
		this->position += forward * velocity;
	if (direction == BACKWARD)
		this->position -= forward * velocity;
	if (direction == LEFT)
		this->position -= right * velocity;
	if (direction == RIGHT)
		this->position += right * velocity;
	if (direction == UPWARD)
		this->position += up * velocity;
	if (direction == DOWNWARD)
		this->position -= up * velocity;
}

void Camera::Rotate(float angle, glm::vec3 rot)
{
	forward = glm::normalize(rot);
}



void ParticleManager::Add(Particle* part)
{
	particles.push_back(part);
}
void ParticleManager::UpdateAll()
{
	for (int i = 0; i < particles.size(); i++) {
		if (particles[i] != nullptr) {
			particles[i]->Update();
		}
		else {
			particles.erase(particles.begin() + i);
			continue;
		}
	}
}
void ObjectManager::Add(G_Object* add)
{
	OBJS.push_back(add);
}

void ObjectManager::Add(EngObj* add)
{
	RenderingObj.push_back(add);
	OBJS.push_back(add);
}

void ObjectManager::UpdateAll()
{
	for (int i = 0; i < OBJS.size(); i++) {
		if (OBJS[i] != nullptr) {
			
			OBJS[i]->Update();
		}
		else {
			OBJS.erase(OBJS.begin() + i);
			continue;
		}
	}

	for(int i = 0; i < RenderingObj.size(); i++) {
		if (RenderingObj[i] != nullptr) {
			RenderingObj[i]->Render();
		}
		else {
			RenderingObj.erase(RenderingObj.begin() + i);
			continue;
		}
	}
}

void ObjectManager::UpdateAllFix(float delta)
{
	for (int i = 0; i < OBJS.size(); i++) {
		if (OBJS[i] != nullptr) {
			OBJS[i]->OnFixUpadte(delta);
		}
	}
}

void ObjectManager::AddLightSrc(Light* light)
{
	for (const auto& obj : RenderingObj) {
		obj->AddLightSource(light);
	}
}

Particle::Particle(glm::vec2 pos, glm::vec2 scal, glm::vec3 col, string path_tex) : position(pos), color(col), scale(scal)
{
	G_SHADER_INSTANCE GSI("particle.vert", "particle.frag", path_tex.c_str());
	GSI.CreateShader(ShaderID);
	if (scal != glm::vec2(0)) {
		scal = glm::vec2(1.0f, 1.0f);
	}


}

void Component::Update() {
}

void Component::OnFix(float deltatime) {
}

void PhysBody::Update(){
}

void PhysBody::OnFix(float deltatime)
{
	Velocity.y += (deltatime / 100) * -g;
	Velocity.y *= (1.0f - (drag * deltatime/100));

	if(Collider* hit = instance->GetComponent<Collider>()) {
		if (hit->CollisionDetect(Velocity, info)) {
			Velocity.y = 0;
		}
	}

	instance->Move(Velocity);
}


Collider::Collider()
{
}

bool Collider::CollisionDetect(glm::vec3 dir, glm::vec3& normal) {
	SimpleX simplex;
	pos = instance->position + offset;
	model = glm::translate(glm::mat4(1.0f), pos);
	model = glm::scale(model, size);
	for (auto& obj : ObjectManager::GetInstance().GetAllObjects()) {
		if (auto other = obj->GetComponent<Collider>()) {
			glm::vec3 dir = other->pos - this->pos;
			simplex.push_front(GetSupport(this, other, dir));
			dir = -simplex[0];
			glm::vec3 A = GetSupport(this, other, dir);

			// Если новая точка не перешла за (0,0,0), столкновения нет
			if (glm::dot(A, dir) < 0) return false;

			simplex.push_front(A);

			// 4. Функция, которая проверяет симплекс и обновляет 'dir'
			if (NextSimplex(simplex, dir)) {
				Log("Detected collision");
				return true;
			}
		}
	}
	return false;
}

BoxCollider::BoxCollider()
{
}

glm::vec3 BoxCollider::SupportFunc(glm::vec3 direction)
{
	glm::vec3 localDir = glm::inverse(glm::mat3(model)) * direction;
	return model * glm::vec4(
		(localDir.x > 0 ? size.x : -size.x) * 0.5f,
		(localDir.y > 0 ? size.y : -size.y) * 0.5f,
		(localDir.z > 0 ? size.z : -size.z) * 0.5f,
		1
	);
}

glm::vec3 Collider::GetSupport(Collider* a, Collider* b, glm::vec3 direction)
{
	return a->SupportFunc(direction) - b->SupportFunc(direction);
}


bool Collider::Line(SimpleX& points, glm::vec3& direction)
{
	glm::vec3 a = points[0];
	glm::vec3 b = points[1];

	glm::vec3 ab = b - a;
	glm::vec3 ao = -a;

	if (SameDirection(ab, ao)) {
		direction = glm::cross(glm::cross(ab, ao), ab);
	}
	else {
		points.pts = { a }; // Точка B не нужна
		direction = ao;
	}
	return false; // Коллизия еще не подтверждена
}

bool Collider::Triangle(SimpleX& points, glm::vec3& direction)
{
	glm::vec3 a = points[0];
	glm::vec3 b = points[1];
	glm::vec3 c = points[2];

	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 ao = -a;

	glm::vec3 abc = glm::cross(ab, ac);

	if (SameDirection(glm::cross(abc, ac), ao)) {
		if (SameDirection(ac, ao)) {
			points.pts = { a, c };
			direction = glm::cross(glm::cross(ac, ao), ac);
		}
		else {
			points = { a, b };
			return Line(points, direction);
		}
	}
	else {
		if (SameDirection(glm::cross(ab, abc), ao)) {
			points = { a, b };
			return Line(points, direction);
		}
		else {
			if (SameDirection(abc, ao)) {
				direction = abc;
			}
			else {
				points.pts = { a, c, b }; // Меняем порядок для правильной нормали
				direction = -abc;
			}
		}
	}
	return false;
}

bool Collider::Tetrahedron(SimpleX& points, glm::vec3& direction)
{
	glm::vec3 a = points[0];
	glm::vec3 b = points[1];
	glm::vec3 c = points[2];
	glm::vec3 d = points[3];

	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 ad = d - a;
	glm::vec3 ao = -a;

	glm::vec3 abc = glm::cross(ab, ac);
	glm::vec3 acd = glm::cross(ac, ad);
	glm::vec3 adb = glm::cross(ad, ab);

	if (SameDirection(abc, ao)) {
		points.pts = { a, b, c };
		return Triangle(points, direction);
	}
	if (SameDirection(acd, ao)) {
		points.pts = { a, c, d };
		return Triangle(points, direction);
	}
	if (SameDirection(adb, ao)) {
		points.pts = { a, d, b };
		return Triangle(points, direction);
	}

	return true; // Начало координат внутри всех граней!
}
