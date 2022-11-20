#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

class Shader
{
public:
	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath)
	{
		string vertexCode;
		string fragmentCode;
		ifstream vShaderFile;
		ifstream fShaderFile;

		// Set which state flags to throw member type 'failure'
		vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		try
		{
			// Open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			stringstream vShaderStream, fShaderStream;
			// Read file buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// Close files
			vShaderFile.close();
			fShaderFile.close();
			// Convert stream to string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (ifstream::failure e)
		{
			cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		//Compilation
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vShaderCode, NULL);
		glCompileShader(vertexShader);

		checkCompilationErrors(vertexShader);

		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);

		checkCompilationErrors(fragmentShader);

		// Link
		ID = glCreateProgram();

		glAttachShader(ID, vertexShader);
		glAttachShader(ID, fragmentShader);
		glLinkProgram(ID);

		checkCompilationErrors(ID, false);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	void use()
	{
		glUseProgram(ID);
	}

	void deleteProgram()
	{
		glDeleteProgram(ID);
	}

	void setBool(const string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	void setInt(const string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setFloat(const string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setMat4(const string& name, glm::mat4& value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
	}

	void checkCompilationErrors(unsigned int object, bool isShader = true)
	{
		int success;
		char infoLog[512];
		isShader
			? glGetShaderiv(object, GL_COMPILE_STATUS, &success)
			: glGetProgramiv(object, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			isShader
				? glGetShaderInfoLog(object, 512, NULL, infoLog)
				: glGetProgramInfoLog(object, 512, NULL, infoLog);
			std::cout << "Compilation failed\n" << infoLog << std::endl;
		}
	}
};