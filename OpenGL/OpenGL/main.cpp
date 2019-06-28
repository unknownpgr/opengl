#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

void log(string str) {
	cout << str << endl;
}

const float SCR_WIDTH = 800.f;
const float SCR_HEIGHT = 600.f;

string ReadStringFromFile(string path) {
	ifstream in(path);
	string contents((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
	return contents;
}

//This method loads the shader source from the given file paths and compile.
void compileShader(string shaderPath, int* shader, GLenum type, int* success, string* infoLog) {
	//Load shader source
	string	str = ReadStringFromFile(shaderPath);
	const char* fragmentShaderSource = str.c_str();

	//Create shader and compile
	*shader = glCreateShader(type);
	glShaderSource(*shader, 1, &fragmentShaderSource, NULL);
	glCompileShader(*shader);

	//Error
	if (success != NULL) {
		glGetShaderiv(*shader, GL_COMPILE_STATUS, success);
		if (!*success&&infoLog != NULL) {
			char logStr[1024];
			glGetShaderInfoLog(*shader, 512, NULL, logStr);
			*infoLog += logStr;
			*infoLog += "\n";
		}
	}
}

//This method loades shaders from path, creates program, link shaders, and delete shaders.
//Parameters can be modified.
unsigned int getProgram(string vertexShaderPath, string fragmentShaderPath, int* success, string* infoLog) {
	//Load and compile shaders
	int vertexShader;
	compileShader(vertexShaderPath, &vertexShader, GL_VERTEX_SHADER, success, infoLog);
	if (success != NULL && !*success)	return -1;

	int fragmentShader;
	compileShader(fragmentShaderPath, &fragmentShader, GL_FRAGMENT_SHADER, success, infoLog);
	if (success != NULL && !*success)	return -1;

	//Create program and link shaders
	int program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	//Error
	if (success != NULL) {
		glGetProgramiv(program, GL_LINK_STATUS, success);
		if (!success) {
			char logStr[1024];
			glGetProgramInfoLog(program, 512, NULL, logStr);
			*infoLog += logStr;
			*infoLog += "\n";
		}
	}

	//Delete shader
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//Return
	return program;
}

//Load image from given file path, create texture, free image.
void getTexture(string texturePath, GLint type, unsigned int* texture, int* width, int* height, int* nrChannels) {
	glGenTextures(1, texture);

	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char *data = stbi_load(texturePath.c_str(), width, height, nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, type, *width, *height, 0, type, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
}

//Resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//Process input
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


int main()
{
	log("Starting...");
	glfwInit();														//Inifialize glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);					//Set glfw major version(3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);					//Set glfw minor version(3)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//Set profile
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);			//For mac OS

	//Create window with size and title
	GLFWwindow* window = glfwCreateWindow((int)SCR_WIDTH, (int)SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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
	glViewport(0, 0, 800, 600);

	//Add resize callback
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//============================================================
	//Define shader
	//============================================================

	//Link shaders with program
	unsigned int shaderProgram;
	int success = 1;
	string infoLog;
	shaderProgram = getProgram("vertexShader.glsl", "fragmentShader.glsl", &success, &infoLog);
	if (!success) {
		log(infoLog);
	}

	//============================================================
	//Define vertex
	//============================================================

	//New vertex
	float vertices[] = {
	 0.5f,  0.5f, 0.0f,	  1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
	 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
	-0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 1.0f
	};
	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	//Vertex buffer object
	unsigned int VBO;		//Buffer id
	glGenBuffers(1, &VBO);	//Get buffer

	//Veretex array object
	unsigned int VAO;					//Array id
	glGenVertexArrays(1, &VAO);			//Get array

	//Element buffer object
	unsigned int EBO;
	glGenBuffers(1, &EBO);

	//Initialze code
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	//Bind buffer
	glBindVertexArray(VAO);				//Bind array
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(0);

	//Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Textrue attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//============================================================
	//Define texture
	//============================================================
	unsigned int texture;
	int width, height, nrChannels;
	getTexture("texture.jpeg", GL_RGB, &texture, &width, &height, &nrChannels);

	unsigned int texture2;
	getTexture("awesomeface.png", GL_RGBA, &texture2, &width, &height, &nrChannels);

	glUseProgram(shaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	//============================================================
	//Define transform
	//============================================================

	mat4 model = mat4(1.0f);
	mat4 view = mat4(1.0f);
	mat4 projection = mat4(1.0f);
	model = rotate(model, radians(-55.0f), vec3(1.0f, 0.0f, 0.0f));
	view = translate(view, vec3(0.0f, 0.0f, -3.0f));
	projection = perspective(radians(45.0f), SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

	unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
	unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		//Graphic initialize Process
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);	//Set clear color
		glClear(GL_COLOR_BUFFER_BIT);			//Clear
	
		//Uniform value test
		float timeValue = (float)glfwGetTime();
		float sineValue = sin(timeValue) / 2.0f + .5f;

		int uniformValue = glGetUniformLocation(shaderProgram, "color2");
		glUniform4f(uniformValue, 1.0f, 1.0f, sineValue, sineValue);
		vertices[0] = sineValue;

		//Update buffer change
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

		//============================================================
		//Drawing process
		//============================================================

		glBindVertexArray(VAO);
		//Primitive types, array start inex, number of vertex(3 for triangles)
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		glfwSwapBuffers(window);	//Refresh
		glfwPollEvents();			//Call callback event functions
	}
	
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}