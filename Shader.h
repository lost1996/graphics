#ifndef SHADER_H
#define SHADER_H

#include<string>
#include<fstream>
#include<sstream>
#include<iostream>

#include<glew.h>

using namespace std;
class Shader
{
public:

	GLuint shaderProgram;

	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

	void Use();
};

#endif