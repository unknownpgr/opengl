#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Windows.h>
#include "Shader.h"
#include "stb_image.h"
#include <chrono>
#include <windows.h>
#include "wglext.h"

using namespace std;
using namespace glm;

//============================================================
// For debugging
//============================================================

void log(string str) {
	cout << str << endl;
}

int index = 0;
chrono::time_point<chrono::steady_clock> listTimestamp[100];
string listTimestampLog[100];

void checkTime(string log) {
	listTimestamp[index] = chrono::high_resolution_clock::now();
	listTimestampLog[index] = log;
	index++;
	if (index > 99)index = 0;
}

void listTimeSpent() {
	chrono::time_point<chrono::steady_clock> bef = listTimestamp[0];
	for (int i = 0;i < index;i++) {
		int dt = (int)std::chrono::duration_cast<std::chrono::nanoseconds>(listTimestamp[i] - bef).count();
		cout << listTimestampLog[i] << "\t" << dt / 1000 << " us" << endl;
	}
}

//============================================================

const float SCR_WIDTH = 800.f;
const float SCR_HEIGHT = 600.f;

bool WGLExtensionSupported(const char *extension_name)
{
	// this is pointer to function which returns pointer to string with list of all wgl extensions
	PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;

	// determine pointer to wglGetExtensionsStringEXT function
	_wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");

	if (strstr(_wglGetExtensionsStringEXT(), extension_name) == NULL)
	{
		// string was not found
		return false;
	}

	// extension is supported
	return true;
}

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float lastFrame = 0.0f;
float deltaTime = 0.0f;

float dx = 0;
float dy = 0;
float vy = 0;

//Load image from given file path, create texture, free image.
unsigned int getTexture(string texturePath, GLint type, unsigned int* width = NULL, unsigned int* height = NULL, unsigned int* channels = NULL) {
	unsigned int texture;
	glGenTextures(1, &texture);				//Generate empty texture
	glBindTexture(GL_TEXTURE_2D, texture);	//Bind texture to TEXTURE_2D

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int w, h, c;																	//Width, height, channels
	unsigned char *data = stbi_load(texturePath.c_str(), &w, &h, &c, 0);			//Load image
	glTexImage2D(GL_TEXTURE_2D, 0, type, w, h, 0, type, GL_UNSIGNED_BYTE, data);	//Assign image
	glGenerateMipmap(GL_TEXTURE_2D);												//Generate mipmap

	stbi_image_free(data);

	if (width != NULL)*width = w;
	if (height != NULL)*height = h;
	if (channels != NULL)*channels = c;

	return texture;
}

//Resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//Process input
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && abs(vy) < 0.01) vy += 10;
}

//Initializing process
GLFWwindow* init(int width, int height) {
	glfwInit();														//Inifialize glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);					//Set glfw major version(3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);					//Set glfw minor version(3)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//Set profile
	//glfwWindowHint(GLFW_REFRESH_RATE, 0);							//Set FPS

	//Create window with size and title
	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
	if (window == NULL)
	{
		log("Failed to create GLFW window");
		glfwTerminate();
		return NULL;
	}

	//Set context
	glfwMakeContextCurrent(window);

	//Initialize glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		log("Failed to initialize GLAD");
		return NULL;
	}

	//Set Viewport
	glViewport(0, 0, width, height);
	return window;
}

int main()
{
	log("Starting...");
	GLFWwindow* window = init((int)SCR_WIDTH, (int)SCR_HEIGHT);
	if (window == NULL) return -1;

	//Add resize callback
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//============================================================
	//Define vertex
	//============================================================

	//New vertex
	float vertices[] = {
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	//Vertex buffer object
	unsigned int VBO;					//Buffer id
	glGenBuffers(1, &VBO);				//Get buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	//Bind buffer

	//Veretex array object
	unsigned int VAO;					//Array id
	glGenVertexArrays(1, &VAO);			//Get array
	glBindVertexArray(VAO);				//Bind array

	//Initialze code
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Element buffer object
	//unsigned int EBO;
	//glGenBuffers(1, &EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(0);

	//Textrue attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//============================================================
	//Define texture
	//============================================================

	//============================================================
	//Define shader
	//============================================================

	//Link shaders with program

	Shader shader = Shader("vertexShader.glsl", "fragmentShader.glsl");
	unsigned int texture1 = getTexture("texture.jpeg", GL_RGB);
	unsigned int texture2 = getTexture("awesomeface.png", GL_RGBA);

	shader.use();
	shader.setInt("texture1", 0);
	//shader.setInt("texture2", 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, texture2);

	//============================================================
	//Define transform
	//============================================================

	mat4 model = mat4(1.0f);
	mat4 view = mat4(1.0f);
	mat4 projection = mat4(1.0f);

	model = rotate(model, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	projection = perspective(radians(45.0f), SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

	unsigned int modelLoc = shader.setMatrix4("model", model);
	unsigned int viewLoc = shader.setMatrix4("view", view);
	unsigned int projectionLoc = shader.setMatrix4("projection", projection);

	glEnable(GL_DEPTH_TEST);

	POINT cursorPosition;

	float yaw = 0;
	float pitch = 0;
	float offset = 500;
	SetCursorPos((int)offset, (int)offset);

	//glfwSwapInterval(1);					//Set interval
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);	//Set clear color

	//============================================================
	//Virtual sync
	//============================================================
	PFNWGLSWAPINTERVALEXTPROC       wglSwapIntervalEXT = NULL;
	PFNWGLGETSWAPINTERVALEXTPROC    wglGetSwapIntervalEXT = NULL;

	if (WGLExtensionSupported("WGL_EXT_swap_control"))
	{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");
	}

	//Turn on virtual sync
	if (wglSwapIntervalEXT != NULL) wglSwapIntervalEXT(0);


	while (!glfwWindowShouldClose(window))
	{
		//============================================================
		//Movement and rotation
		//============================================================


		//Calculate frame
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		//Calculate mouse movement
		GetCursorPos(&cursorPosition);
		dx = cursorPosition.x - offset;
		dy = offset - cursorPosition.y;
		SetCursorPos((int)offset, (int)offset);

		//Calculate pitch and yaw
		float sensitivity = 1.0f;
		yaw += dx * sensitivity*deltaTime;
		pitch += dy * sensitivity*deltaTime;
		if (pitch > 89.9f) pitch = 89.9f;
		if (pitch < -89.9f) pitch = -89.9f;

		//Calculate front vector
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(front);

		//Implement gravity
		vy -= 20 * deltaTime;
		cameraPos.y += vy * deltaTime;
		if (cameraPos.y < 0) {
			cameraPos.y = 0;
			vy = 0;
		}

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		//============================================================
		//GPU associated part
		//============================================================

		//Graphic initialize Process
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//Clear

		//Use shader
		shader.use();

		//Update position and rotation

		//shader.updateMatrix4(modelLoc, model);
		shader.updateMatrix4(viewLoc, view);

		//Update buffer change
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//============================================================
		//Drawing process
		//============================================================

		glBindVertexArray(VAO);

		//Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(0);

		//Textrue attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Uncommenet here swhen use element buffer object
		//Primitive types, array start inex, number of vertex(3 for triangles)
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//Release resource
		glBindVertexArray(0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		shader.stop();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}