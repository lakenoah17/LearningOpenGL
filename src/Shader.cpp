#include "Shader.h"
#include "GL/glew.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"


Shader::Shader(const std::string& filepath)
	: m_FilePath(filepath), m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID));
}


ShaderProgramSource Shader::ParseShader(const std::string& filePath) {
    std::ifstream stream(filePath);

    enum class ShaderType {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];

    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
    //Creates the location of the shader
    unsigned int id = glCreateShader(type);
    //Sets up the string as a c str
    const char* src = source.c_str();
    //Sets the source code of the shader
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    //Gets the result of the compilation
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

    //Checks if the shader compiled properly
    if (result == GL_FALSE) {

        //Gets the length of the message
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));

        //Gets the error message
        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));

        std::cout << "Failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
            << " shader!" << std::endl;

        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));

        return 0;
    }

    //Returns the location of the compiled shader
    return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    //Creates the vertex shader
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    //Creates the fragment shader
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    //Attaches the vertex and frag shaders to the program
    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));

    //Links the program
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    //Unnecessary to leave shaders in the code as they
    //have already been attached to the program
    //TECHNICALLY should be using detach shader not delete
    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

void Shader::Bind() const
{
    GLCall(glUseProgram(m_RendererID));
}

void Shader::UnBind() const
{
    GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    GLCall(glUniform1i(GetUniformLocations(name), value));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
    GLCall(glUniform1f(GetUniformLocations(name), value));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GLCall(glUniform4f(GetUniformLocations(name), v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    GLCall(glUniformMatrix4fv(GetUniformLocations(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::GetUniformLocations(const std::string& name) const
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    {
        return m_UniformLocationCache[name];
    }

    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));

    if (location == -1)
    {
        std::cout << "Warning: uniform '" << name << "' doesn't exist" << std::endl;
    }

    m_UniformLocationCache[name] = location;
	return location;
}
