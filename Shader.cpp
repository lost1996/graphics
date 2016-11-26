#include "Shader.h"

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	string vertexCode;
	string fragmentCode;
	try
	{
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		vShaderFile.exceptions(ifstream::badbit);
		fShaderFile.exceptions(ifstream::badbit);
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		const GLchar* vShaderCode = vertexCode.c_str();
		const GLchar* fShaderCode = fragmentCode.c_str();

	//	cout << vShaderCode << endl;
	//	cout << fShaderCode << endl;

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vShaderCode, NULL);
		glCompileShader(vertexShader);
		GLint success;
		GLchar infoLog[512];

		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::vertexShader::COMPILATION_FAILED\n" << infoLog << std::endl;
		};

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::fragmentShader::COMPILATION_FAILED\n" << infoLog << std::endl;
		};

		this->shaderProgram = glCreateProgram();
		glAttachShader(this->shaderProgram, vertexShader);
		glAttachShader(this->shaderProgram, fragmentShader);

		glLinkProgram(this->shaderProgram);
		glGetShaderiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::shaderProgram::COMPILATION_FAILED\n" << infoLog << std::endl;
		};

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	catch (ifstream::failure e)
	{
		cout << "ERROR::fileread" << endl;
	}





}

void Shader::Use()
{
	glUseProgram(this->shaderProgram);
}