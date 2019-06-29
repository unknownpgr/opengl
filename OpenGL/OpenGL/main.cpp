#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Windows.h>
#include "Shader.h"

using namespace std;
using namespace glm;

void log(string str) {
	cout << str << endl;
}

const float SCR_WIDTH = 800.f;
const float SCR_HEIGHT = 600.f;

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
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int w, h, nrChannels;
	unsigned char *data = stbi_load(texturePath.c_str(), &w, &h, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, type, w, h, 0, type, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	if (width != NULL)*width = w;
	if (height != NULL)*height = h;
	if (channels != NULL)*channels = nrChannels;

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


int main()
{
	log("Starting...");
	glfwInit();														//Inifialize glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);					//Set glfw major version(3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);					//Set glfw minor version(3)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//Set profile

	//Create window with size and title
	GLFWwindow* window = glfwCreateWindow((int)SCR_WIDTH, (int)SCR_HEIGHT, "OpenGL", NULL, NULL);
	if (window == NULL)
	{
		log("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}

	//Set context
	glfwMakeContextCurrent(window);

	//Initialize glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		log("Failed to initialize GLAD");
		return -1;
	}

	//Set Viewport
	glViewport(0, 0, (int)SCR_WIDTH, (int)SCR_HEIGHT);

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
	unsigned int VBO;		//Buffer id
	glGenBuffers(1, &VBO);	//Get buffer

	//Veretex array object
	unsigned int VAO;					//Array id
	glGenVertexArrays(1, &VAO);			//Get array

	//Element buffer object
	//unsigned int EBO;
	//glGenBuffers(1, &EBO);

	//Initialze code
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	//Bind buffer
	glBindVertexArray(VAO);				//Bind array
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
	unsigned int texture1 = getTexture("t2.jpeg", GL_RGB);
	unsigned int texture2 = getTexture("awesomeface.png", GL_RGBA);

	shader.use();
	shader.setInt("texture1", 0);
	shader.setInt("texture2", 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

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

	while (!glfwWindowShouldClose(window))
	{
		//============================================================
		//Movement and rotation
		//============================================================

		processInput(window);

		//Calculate frame
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

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

		//Update position and rotation
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		shader.updateMatrix4(modelLoc, model);
		shader.updateMatrix4(viewLoc, view);

		//Graphic initialize Process
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);				//Set clear color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//Clear

		//Update buffer change
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//============================================================
		//Drawing process
		//============================================================

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Primitive types, array start inex, number of vertex(3 for triangles)
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//Unbind
		glBindVertexArray(0);

		glfwSwapBuffers(window);	//Refresh
		glfwPollEvents();			//Call callback event functions
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}