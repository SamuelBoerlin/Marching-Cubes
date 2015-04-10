//Author: Benny Bobaganoosh, Samuel Börlin

#ifndef SHADER_INCLUDED_H
#define SHADER_INCLUDED_H

#include <string>
#include <GL/glew.h>
#include <map>
#include "transform.hpp"
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	Shader(const std::string& fileName);

	static const enum UniformTypes { Matrix4fv, Uniform4fv, Uniform3fv, Uniform2fv, Uniform1fv, Uniform4f, Uniform3f, Uniform2f, Uniform1f };
	struct ShaderUniform {
	public:
		ShaderUniform(){}
		ShaderUniform(UniformTypes type, int location) {
			this->m_uniformType = type;
			this->m_location = location;
		}
		inline int GetLocation() { return m_location; }
		inline UniformTypes GetType() { return m_uniformType; }
	private:
		int m_location;
		UniformTypes m_uniformType;
	};

	void Bind();
	ShaderUniform AddUniform(const std::string& uniformName, UniformTypes uniformType);
	bool UpdateUniform(const std::string& uniformName, int count, GLfloat* vectorPtr);
	bool UpdateUniform(const std::string& uniformName, float x);
	bool UpdateUniform(const std::string& uniformName, glm::vec2 vec);
	bool UpdateUniform(const std::string& uniformName, glm::vec3 vec);
	bool UpdateUniform(const std::string& uniformName, glm::vec4 vec);
	bool UpdateUniform(const std::string& uniformName, int count, bool transpose, GLfloat* matrixPtr);
	void Update(Transform& transform, Camera& camera);

	virtual ~Shader();
protected:
private:
	static const unsigned int NUM_SHADERS = 2;
	static const unsigned int NUM_UNIFORMS = 2;
	void operator=(const Shader& shader) {}
	Shader(const Shader& shader) {}

	std::string LoadShader(const std::string& fileName);
	void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
	GLuint CreateShader(const std::string& text, unsigned int type);

	GLuint m_program;

	/**Default uniforms**/
	GLuint m_shaders[NUM_SHADERS];
	GLuint m_uniforms[NUM_UNIFORMS];

	/**Additional uniforms**/
	std::map<const std::string, ShaderUniform> m_mShaderUniforms;
};

#endif
