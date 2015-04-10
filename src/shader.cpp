//Author: Benny Bobaganoosh, Samuel Börlin

#include "shader.hpp"
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string& fileName)
{
	m_program = glCreateProgram();
	m_shaders[0] = CreateShader(LoadShader(fileName + ".vs"), GL_VERTEX_SHADER);
	m_shaders[1] = CreateShader(LoadShader(fileName + ".fs"), GL_FRAGMENT_SHADER);

	for(unsigned int i = 0; i < NUM_SHADERS; i++) {
		glAttachShader(m_program, m_shaders[i]);
	}

	glLinkProgram(m_program);
	CheckShaderError(m_program, GL_LINK_STATUS, true, "Error linking shader program");

	glValidateProgram(m_program);
	CheckShaderError(m_program, GL_LINK_STATUS, true, "Invalid shader program");

	m_uniforms[0] = glGetUniformLocation(m_program, "modelview");
	m_uniforms[1] = glGetUniformLocation(m_program, "projection");
}

Shader::~Shader()
{
	for(unsigned int i = 0; i < NUM_SHADERS; i++)
    {
        glDetachShader(m_program, m_shaders[i]);
        glDeleteShader(m_shaders[i]);
    }

	glDeleteProgram(m_program);
}

void Shader::Bind()
{
	glUseProgram(m_program);
}

void Shader::Update(Transform& transform, Camera& camera)
{
	glUniformMatrix4fv(m_uniforms[0], 1, GL_FALSE, &transform.GetModel()[0][0]);
	glUniformMatrix4fv(m_uniforms[1], 1, GL_FALSE, &camera.GetViewProjection()[0][0]);
}

std::string Shader::LoadShader(const std::string& fileName)
{
    std::ifstream file;
    file.open((fileName).c_str());

    std::string output;
    std::string line;

    if(file.is_open())
    {
        while(file.good())
        {
            getline(file, line);
			output.append(line + "\n");
        }
    }
    else
    {
		std::cerr << "Unable to load shader: " << fileName << std::endl;
    }

    return output;
}

void Shader::CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage)
{
    GLint success = 0;
    GLchar error[1024] = { 0 };

    if(isProgram)
        glGetProgramiv(shader, flag, &success);
    else
        glGetShaderiv(shader, flag, &success);

    if(success == GL_FALSE)
    {
        if(isProgram)
            glGetProgramInfoLog(shader, sizeof(error), NULL, error);
        else
            glGetShaderInfoLog(shader, sizeof(error), NULL, error);

        std::cerr << errorMessage << ": '" << error << "'" << std::endl;
    }
}

GLuint Shader::CreateShader(const std::string& text, unsigned int type)
{
    GLuint shader = glCreateShader(type);

    if(shader == 0)
		std::cerr << "Error compiling shader type " << type << std::endl;

    const GLchar* p[1];
    p[0] = text.c_str();
    GLint lengths[1];
    lengths[0] = text.length();

    glShaderSource(shader, 1, p, lengths);
    glCompileShader(shader);

    CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error compiling shader!");

    return shader;
}

Shader::ShaderUniform Shader::AddUniform(const std::string& uniformName, UniformTypes uniformType) {
	int location = glGetUniformLocation(m_program, uniformName.c_str());
	Shader::ShaderUniform shaderUniform(uniformType, location);
	m_mShaderUniforms[uniformName] = shaderUniform;
	return shaderUniform;
}

bool Shader::UpdateUniform(const std::string& uniformName, int count, GLfloat* vectorPtr) {
	Shader::ShaderUniform shaderUniform = m_mShaderUniforms[uniformName];
	int location = shaderUniform.GetLocation();
	switch(shaderUniform.GetType()) {
	case(Uniform4fv):
		glUniform4fv(location, count, vectorPtr);
		return true;
	case(Uniform3fv):
		glUniform3fv(location, count, vectorPtr);
		return true;
	case(Uniform2fv):
		glUniform2fv(location, count, vectorPtr);
		return true;
	case(Uniform1fv):
		glUniform1fv(location, count, vectorPtr);
		return true;
	}
	return false;
}

bool Shader::UpdateUniform(const std::string& uniformName, float x) {
	Shader::ShaderUniform shaderUniform = m_mShaderUniforms[uniformName];
	int location = shaderUniform.GetLocation();
	switch(shaderUniform.GetType()) {
	case(Uniform1f):
		glUniform1f(location, x);
		return true;
	}
	return false;
}

bool Shader::UpdateUniform(const std::string& uniformName, glm::vec2 vec) {
	Shader::ShaderUniform shaderUniform = m_mShaderUniforms[uniformName];
	int location = shaderUniform.GetLocation();
	switch(shaderUniform.GetType()) {
	case(Uniform2f):
		glUniform2f(location, vec.x, vec.y);
		return true;
	}
	return false;
}

bool Shader::UpdateUniform(const std::string& uniformName, glm::vec3 vec) {
	Shader::ShaderUniform shaderUniform = m_mShaderUniforms[uniformName];
	int location = shaderUniform.GetLocation();
	switch(shaderUniform.GetType()) {
	case(Uniform3f):
		glUniform3f(location, vec.x, vec.y, vec.z);
		return true;
	}
	return false;
}

bool Shader::UpdateUniform(const std::string& uniformName, glm::vec4 vec) {
	Shader::ShaderUniform shaderUniform = m_mShaderUniforms[uniformName];
	int location = shaderUniform.GetLocation();
	switch(shaderUniform.GetType()) {
	case(Uniform4f):
		glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
		return true;
	}
	return false;
}

bool Shader::UpdateUniform(const std::string& uniformName, int count, bool transpose, GLfloat* matrixPtr) {
	Shader::ShaderUniform shaderUniform = m_mShaderUniforms[uniformName];
	int location = shaderUniform.GetLocation();
	switch(shaderUniform.GetType()) {
	case(Matrix4fv):
		glUniformMatrix4fv(location, count, transpose ? GL_TRUE : GL_FALSE, matrixPtr);
		return true;
	}
	return false;
}