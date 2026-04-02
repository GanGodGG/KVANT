#include <INCLUDE/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>	
#include "GangodObjects.h"
#include "G_SHADER_INSTANCE.h"
using namespace std;

static int S_WIDTH = 980;
static int S_HEIGHT = 980;
ObjectManager OBJMGR;
float sensitivity = 0.25f;
float deltaTime;
Camera mainCam(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), (float)S_WIDTH / S_HEIGHT, 120.0f, 0.1f, 100.0f, OBJMGR);
void RotateCam(GLFWwindow* window, double x, double y);
int main() {
	float lastFrame = 0.0f;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(S_WIDTH, S_HEIGHT, "WILL", NULL, NULL);
	if (!window) {
		cerr << "PANIC! I CANT START UP!" << endl;
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cerr << "INIT FAIL!" << endl;
		return -1;
	}
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, RotateCam);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, S_WIDTH, S_HEIGHT);
	glm::vec3 bLightCol = glm::vec3(0.15f, 0.15f, 0.15f);
	glm::vec3 lightCol = glm::vec3(1.0f, 1.0f, 1.0f);
	SpotLight spotLight(mainCam.position, mainCam.forward, glm::vec3(9.0f, 0.5f, 0.5f), 1, 2);

	PointLight pointLight(glm::vec3(0.0f, 4.0f, -5.0f), glm::vec3(0.5f, 0.5f, 1.0f));

	DirLight mainLight(mainCam.forward, glm::vec3(1.5f, 1.5f, 1.5f));

	EngObj test("Untitled.obj", glm::vec3(0.0f, 0.0f,0.0f), "TEST", &mainCam, "shader.vert", "shader.frag", "tetoo.jpg", OBJMGR);
	EngObj test2("Untitled.obj", glm::vec3(5.0f, 5.0f, 0.0f), "TEST2", &mainCam, "shader.vert", "shader.frag", "tetoo.jpg", OBJMGR);
	OBJMGR.AddLightSrc(&mainLight);
	OBJMGR.AddLightSrc(&pointLight);
	OBJMGR.AddLightSrc(&spotLight);
	//EngObj light("Untitled.obj", glm::vec3(0.0f, 4.0f, -5.0f), "LIGHT", &mainCam, "shader_light.vert", "shader_light.frag", glm::vec3(1,1,1), OBJMGR);
	test.Rescale(glm::vec3(5.0f, 5.0f, 5.0f));
	while (!glfwWindowShouldClose(window)) {

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//Log(1 / deltaTime);
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, 1);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			mainCam.Move(Camera::FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			mainCam.Move(Camera::BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			mainCam.Move(Camera::LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			mainCam.Move(Camera::RIGHT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			mainCam.Move(Camera::UPWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			mainCam.Move(Camera::DOWNWARD, deltaTime);
		//test.Rotate((float)glfwGetTime(), glm::vec3(1.0f, 0.25f, 0.11f));
		//light.Move(glm::vec3(0.0001f, 0.0f, 0.0f));
		spotLight.pos = mainCam.position + mainCam.forward;
		spotLight.spotdir = mainCam.forward * glm::vec3(3);
		OBJMGR.UpdateAll();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	test.~EngObj();
	glfwTerminate();
	return glGetError();
}

void RotateCam(GLFWwindow* window, double xpos, double ypos) {
	// 1. Calculate offset from the center of the screen
	float Xoffset = (float)xpos - (S_WIDTH / 2.0f);
	float Yoffset = (S_HEIGHT / 2.0f) - (float)ypos;

	// 2. Apply sensitivity
	Xoffset *= sensitivity;
	Yoffset *= sensitivity;

	// 3. Update Euler angles
	mainCam.yaw += Xoffset;
	mainCam.pitch += Yoffset;

	// 4. Constrain Pitch to prevent screen flipping
	if (mainCam.pitch > 89.0f)  mainCam.pitch = 89.0f;
	if (mainCam.pitch < -89.0f) mainCam.pitch = -89.0f;

	// 5. Calculate the new direction vector
	glm::vec3 direction;
	direction.x = cos(glm::radians(mainCam.yaw)) * cos(glm::radians(mainCam.pitch));
	direction.y = sin(glm::radians(mainCam.pitch));
	direction.z = sin(glm::radians(mainCam.yaw)) * cos(glm::radians(mainCam.pitch));

	// Update camera direction
	mainCam.Rotate(0.0f, direction);

	// 6. Reset cursor to center so we don't hit the screen edge
	glfwSetCursorPos(window, S_WIDTH / 2.0f, S_HEIGHT / 2.0f);
}