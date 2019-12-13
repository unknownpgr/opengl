#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <fstream>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

using namespace std;

class Shader {
public:
	unsigned int ID;

	Shader(const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath = NULL) {
		int success;

		//Load and compile shaders
		int vertexShader;
		success = compileShader(vertexShaderPath, &vertexShader, GL_VERTEX_SHADER);
		if (!success) return;

		int fragmentShader;
		success = compileShader(fragmentShaderPath, &fragmentShader, GL_FRAGMENT_SHADER);
		if (!success) return;

		int geometryShader;
		if (geometryShaderPath != NULL) {
			success = compileShader(geometryShaderPath, &geometryShader, GL_GEOMETRY_SHADER);
			if (!success) return;
		}

		//Create program and link shaders
		ID = glCreateProgram();
		glAttachShader(ID, vertexShader);
		glAttachShader(ID, fragmentShader);
		if (geometryShaderPath != NULL)glAttachShader(ID, geometryShader);
		glLinkProgram(ID);

		//Delete shader
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		if (geometryShaderPath != NULL)glDeleteShader(geometryShader);

		//Error
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success&&verbose) {
			char logStr[1024];
			glGetProgramInfoLog(ID, 512, NULL, logStr);
			log(logStr);
		}
	}

	void setVerbose(int verbose) {
		this->verbose = verbose;
	}

	unsigned int getUniformLocation(const char* name) {
		return glGetUniformLocation(ID, name);
	}

	int setInt(const char* name, unsigned int value) {
		unsigned int location = glGetUniformLocation(ID, name);
		updateInt(location, value);
		return location;
	}
	void updateInt(unsigned int location, int value) {
		glUniform1i(location, value);
	}

	int setMatrix4(const char* name, glm::mat4 &value) {
		unsigned int location = glGetUniformLocation(ID, name);
		updateMatrix4(location, value);
		return location;
	}
	void updateMatrix4(unsigned int location, glm::mat4 &value) {
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
	}

	void use() {
		glUseProgram(ID);
	}
	void stop() {
		glUseProgram(0);
	}

private:
	int verbose = false;

	void log(string message) {
		cout << message << endl;
	}

	string ReadStringFromFile(string path) {
		ifstream in(path);
		string contents((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
		return contents;
	}

	//This method loads the shader source from the given file paths and compile.
	int compileShader(string shaderPath, int* shader, GLenum type) {
		//Load shader source
		string str = ReadStringFromFile(shaderPath);
		const char* fragmentShaderSource = str.c_str();

		//Create shader and compile
		*shader = glCreateShader(type);
		glShaderSource(*shader, 1, &fragmentShaderSource, NULL);
		glCompileShader(*shader);

		//Error
		int success;
		glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
		if (!success&&verbose) {
			char logStr[1024];
			glGetShaderInfoLog(*shader, 512, NULL, logStr);
			log(logStr);
		}

		return success;
	}
};

#endif