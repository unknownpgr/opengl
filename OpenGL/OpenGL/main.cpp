#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

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
int getProgram(string vertexShaderPath, string fragmentShaderPath, int* success, string* infoLog) {

	//Load and compile shaders
	int vertexShader;
	compileShader(vertexShaderPath, &vertexShader, GL_VERTEX_SHADER, success, infoLog);
	if (!*success)	return -1;

	int fragmentShader;
	compileShader(fragmentShaderPath, &fragmentShader, GL_FRAGMENT_SHADER, success, infoLog);
	if (!*success)	return -1;

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
	cout << "Starting..." << endl;
	glfwInit();														//Inifialize glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);					//Set glfw major version(3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);					//Set glfw minor version(3)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//Set profile
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);			//For mac OS

	//Create window with size and title
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}

	//Set context
	glfwMakeContextCurrent(window);

	//Initialize glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	//Set Viewport
	glViewport(0, 0, 800, 600);

	//Add resize callback
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//============================================================
	//Define shader
	//============================================================

	//Define vertex shader
	int vertexShader;
	compileShader("vertexShader.glsl", &vertexShader, GL_VERTEX_SHADER, NULL, NULL);

	//Define fragmentShader
	int fragmentShader;
	compileShader("fragmentShader.glsl", &fragmentShader, GL_FRAGMENT_SHADER, NULL, NULL);

	//Link shaders with program
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	//Delete shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//============================================================
	//Define vertex
	//============================================================

	//New vertex
	float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
	};

	//Buffer setting
	unsigned int VBO;					//Buffer id
	glGenBuffers(1, &VBO);				//Get buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	//Bind buffer

	//Buffer data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//Other draw options : 
	//GL_DYNAMIC_DRAW
	//GL_STREAM_DRAW

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	//Set vertex attribute
	//Position of vertex attribute, vertex vector size, data type, normailize, stride, data offset
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		//Process
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);	//Set clear color
		glClear(GL_COLOR_BUFFER_BIT);			//Clear

		//============================================================
		//Drawing process
		//============================================================

		glUseProgram(shaderProgram);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		//Primitive types, array start inex, number of vertex(3 for triangles)
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);	//Refresh
		glfwPollEvents();			//Call callback event functions
	}


	glfwTerminate();
	return 0;
}