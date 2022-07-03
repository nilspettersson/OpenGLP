#include "Shader.h";
#include "../util/Util.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

using namespace glp;

static std::string defaultVertexShader = "#version 330 core\n"
"\n"
"layout(location = 0) in vec3 position;\n"
"layout(location = 1) in vec2 texCoord;\n"
"layout(location = 2) in vec3 normal;\n"
"\n"
"uniform mat4 u_mvp;\n"
"out vec2 v_texCoord;\n"
"\n"
"void main() {\n"
"    gl_Position = u_mvp * vec4(position, 1);\n"
"};";

static std::string defaultFragmentShader = "#version 330 core\n"
"layout(location = 0) out vec4 colorOutput;\n"
"\n"
"uniform vec3 color;\n"
"\n"
"in vec2 v_texCoord;\n"
"void main() {\n"
"    colorOutput = vec4(1, 0, 1, 1);\n"
"};\n";

Shader::ShaderProgramSouce Shader::parseShader(const std::string& filepath) {
    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    ShaderType type = ShaderType::NONE;

    std::string line;
    std::stringstream shaderStream[2];
    while (std::getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            shaderStream[(int)type] << line << "\n";
        }
    }
    return { shaderStream[0].str(), shaderStream[1].str() };
}


unsigned int Shader::compileShader(std::string& source, unsigned int type) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "shader error" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);

        return 0;
    }

    return id;
}

Shader::Shader(std::string vertexShader, std::string fragmentShader) {
    this->uniformLocations = std::unordered_map<std::string, int>();
    Shader::ShaderProgramSouce source;
    source.vertexSource = vertexShader;
    source.fragmentSource = fragmentShader;
    createShader(source);
}

Shader::Shader(std::string filepath) {
    this->uniformLocations = std::unordered_map<std::string, int>();
    Shader::ShaderProgramSouce source = Shader::parseShader(filepath);
    if (source.fragmentSource.empty() || source.vertexSource.empty()) {
        source.vertexSource = defaultVertexShader;
        source.fragmentSource = defaultFragmentShader;
        createShader(source);
    }
    else {
        createShader(source);
    }
}

void Shader::createShader(Shader::ShaderProgramSouce source) {
    this->shaderId = glCreateProgram();

    unsigned int vs = compileShader(source.vertexSource, GL_VERTEX_SHADER);
    unsigned int fs = compileShader(source.fragmentSource, GL_FRAGMENT_SHADER);

    if (vs == 0 || fs == 0) {
        source.vertexSource = defaultVertexShader;
        source.fragmentSource = defaultFragmentShader;
        createShader(source);
        return;
    }

    glAttachShader(this->shaderId, vs);
    glAttachShader(this->shaderId, fs);
    glLinkProgram(this->shaderId);
    glValidateProgram(this->shaderId);

    glDeleteShader(vs);
    glDeleteShader(fs);

    glUseProgram(this->shaderId);
}

int Shader::getUniformLocation(std::string name) {
    if (this->uniformLocations.find(name) != this->uniformLocations.end()) {
        return this->uniformLocations[name];
    }
    int location = glGetUniformLocation(this->shaderId, name.c_str());
    this->uniformLocations[name] = location;
    return location;
}

unsigned int Shader::getShaderId() {
    return this->shaderId;
}

void Shader::bind() {
    glUseProgram(this->shaderId);

    const int sampler[20] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    setUniform1iv("sampler", sampler);
}

void Shader::setUniform4f(std::string name, float v1, float v2, float v3, float v4) {
    int location = this->getUniformLocation(name);
    glUniform4f(location, v1, v2, v3, v4);
}

void glp::Shader::setUniform3f(std::string name, float v1, float v2, float v3) {
    int location = this->getUniformLocation(name);
    glUniform3f(location, v1, v2, v3);
}

void glp::Shader::setUniform2f(std::string name, float v1, float v2) {
    int location = this->getUniformLocation(name);
    glUniform2f(location, v1, v2);
}

void glp::Shader::setUniform1f(std::string name, float value) {
    int location = this->getUniformLocation(name);
    glUniform1f(location, value);
}

void glp::Shader::setUniform1i(std::string name, int value){
    int location = this->getUniformLocation(name);
    glUniform1i(location, value);
}

void glp::Shader::setUniform1iv(std::string name, const int* values) {
    int location = this->getUniformLocation(name);
    glUniform1iv(location, sizeof(values), (const GLint*)values);
}

void glp::Shader::setUniformMat4f(std::string name, const glm::mat4& matrix) {
    int location = this->getUniformLocation(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
}
