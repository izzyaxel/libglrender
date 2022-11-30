#include "glrShader.hh"

#include <glad/gl.hh>

namespace GLRender
{
	/*Shader::Shader(std::string const &name, std::vector<uint8_t> const &vertShader, std::vector<uint8_t> const &fragShader) : Shader(name, std::string{vertShader.begin(), vertShader.end()}, std::string{fragShader.begin(), fragShader.end()})
	{
		
	}
	
	Shader::Shader(std::string const &name, std::vector<uint8_t> const &compShader) : Shader(name, std::string{compShader.begin(), compShader.end()})
	{
		
	}*/
	
	Shader::Shader(std::string const &name, std::string const &vertShader, std::string const &fragShader)
	{
		uint32_t vertHandle = glCreateShader(GL_VERTEX_SHADER), fragHandle = glCreateShader(GL_FRAGMENT_SHADER);
		this->m_handle = glCreateProgram();
		char const *vertSource = vertShader.data(), *fragSource = fragShader.data();
		glShaderSource(vertHandle, 1, &vertSource, nullptr);
		glShaderSource(fragHandle, 1, &fragSource, nullptr);
		glCompileShader(vertHandle);
		int32_t success = 0;
		glGetShaderiv(vertHandle, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			int32_t maxLen = 0;
			glGetShaderiv(vertHandle, GL_INFO_LOG_LENGTH, &maxLen);
			std::vector<char> error;
			error.resize(maxLen * sizeof(GLchar));
			glGetShaderInfoLog(vertHandle, maxLen, &maxLen, error.data());
			std::string errorStr{error.begin(), error.end()};
			printf("Vert/Frag shader error: %s failed to compile: %s\n", name.c_str(), errorStr.c_str());
			return;
		}
		glCompileShader(fragHandle);
		success = 0;
		glGetShaderiv(fragHandle, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			int32_t maxLen = 0;
			glGetShaderiv(fragHandle, GL_INFO_LOG_LENGTH, &maxLen);
			std::vector<char> error;
			error.resize(maxLen * sizeof(GLchar));
			glGetShaderInfoLog(fragHandle, maxLen, &maxLen, error.data());
			std::string errorStr{error.begin(), error.end()};
			printf("Vert/Frag shader error: %s failed to compile: %s\n", name.c_str(), errorStr.c_str());
			return;
		}
		glAttachShader(this->m_handle, vertHandle);
		glAttachShader(this->m_handle, fragHandle);
		glLinkProgram(this->m_handle);
		success = 0;
		glGetProgramiv(this->m_handle, GL_LINK_STATUS, &success);
		if(!success)
		{
			int32_t maxLen = 0;
			glGetShaderiv(this->m_handle, GL_INFO_LOG_LENGTH, &maxLen);
			std::vector<char> error;
			error.resize(maxLen * sizeof(GLchar));
			glGetShaderInfoLog(this->m_handle, maxLen, &maxLen, error.data());
			std::string errorStr{error.begin(), error.end()};
			printf("Vert/Frag shader program error: %s failed to link\n", name.c_str());
			return;
		}
		glDetachShader(this->m_handle, vertHandle);
		glDetachShader(this->m_handle, fragHandle);
		glDeleteShader(vertHandle);
		glDeleteShader(fragHandle);
	}
	
	Shader::Shader(std::string const &name, std::string const &compShader)
	{
		uint32_t compHandle = glCreateShader(GL_COMPUTE_SHADER);
		this->m_handle = glCreateProgram();
		char const *compSource = compShader.data();
		glShaderSource(compHandle, 1, &compSource, nullptr);
		glCompileShader(compHandle);
		int32_t success = 0;
		glGetShaderiv(compHandle, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			int32_t maxLen = 0;
			glGetShaderiv(compHandle, GL_INFO_LOG_LENGTH, &maxLen);
			std::vector<char> error;
			error.resize(maxLen * sizeof(GLchar));
			glGetShaderInfoLog(compHandle, maxLen, &maxLen, error.data());
			std::string errorStr{error.begin(), error.end()};
			printf("Compute shader error: %s failed to compile: %s\n", name.c_str(), errorStr.c_str());
			return;
		}
		
		glAttachShader(this->m_handle, compHandle);
		glLinkProgram(this->m_handle);
		success = 0;
		glGetProgramiv(this->m_handle, GL_LINK_STATUS, &success);
		if(!success)
		{
			int32_t maxLen = 0;
			glGetShaderiv(this->m_handle, GL_INFO_LOG_LENGTH, &maxLen);
			std::vector<char> error;
			error.resize(maxLen * sizeof(GLchar));
			glGetShaderInfoLog(this->m_handle, maxLen, &maxLen, error.data());
			std::string errorStr{error.begin(), error.end()};
			printf("Compute shader program error: %s failed to link: %s\n", name.c_str(), errorStr.c_str());
			return;
		}
		glDetachShader(this->m_handle, compHandle);
		glDeleteShader(compHandle);
	}
	
	Shader::~Shader()
	{
		glDeleteProgram(this->m_handle);
	}
	
	Shader::Shader(Shader &other)
	{
		this->m_handle = other.m_handle;
		other.m_handle = 0;
	}
	
	Shader& Shader::operator=(Shader other)
	{
		this->m_handle = other.m_handle;
		other.m_handle = 0;
		return *this;
	}
	
	Shader::Shader(Shader &&other) noexcept
	{
		this->m_handle = other.m_handle;
		other.m_handle = 0;
	}
	
	Shader& Shader::operator=(Shader &&other) noexcept
	{
		this->m_handle = other.m_handle;
		other.m_handle = 0;
		return *this;
	}
	
	void Shader::use() const
	{
		glUseProgram(this->m_handle);
	}
	
	int32_t Shader::getUniformHandle(std::string const &location)
	{
		auto it = this->m_uniforms.find(location);
		if(it == this->m_uniforms.end()) it = this->m_uniforms.emplace(location, glGetUniformLocation(this->m_handle, location.data())).first;
		return it->second;
	}
	
	void Shader::sendFloat(std::string const &location, float val)
	{
		glUniform1f(this->getUniformHandle(location), val);
	}
	
	void Shader::sendInt(std::string const &location, int32_t val)
	{
		glUniform1i(this->getUniformHandle(location), val);
	}
	
	void Shader::sendUInt(std::string const &location, uint32_t val)
	{
		glUniform1ui(this->getUniformHandle(location), val);
	}
	
	void Shader::sendVec2f(std::string const &location, float* val)
	{
		glUniform2fv(this->getUniformHandle(location), 1, val);
	}
	
	void Shader::sendVec3f(std::string const &location, float* val)
	{
		glUniform3fv(this->getUniformHandle(location), 1, val);
	}
	
	void Shader::sendVec4f(std::string const &location, float* val)
	{
		glUniform4fv(this->getUniformHandle(location), 1, val);
	}
	
	void Shader::sendMat3f(std::string const &location, float* val)
	{
		glUniformMatrix3fv(this->getUniformHandle(location), 1, GL_FALSE, val);
	}
	
	void Shader::sendMat4f(std::string const &location, float* val)
	{
		glUniformMatrix4fv(this->getUniformHandle(location), 1, GL_FALSE, val);
	}
}
