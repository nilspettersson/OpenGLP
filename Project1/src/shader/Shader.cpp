#include "Shader.h";

using namespace glp;

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
    Shader::ShaderProgramSouce source;
    source.vertexSource = vertexShader;
    source.fragmentSource = fragmentShader;
    createShader(source);
}

Shader::Shader(std::string filepath) {
    Shader::ShaderProgramSouce source = Shader::parseShader("res/shaders/main.shader");
    createShader(source);
}

void Shader::createShader(Shader::ShaderProgramSouce source) {
    this->shaderId = glCreateProgram();

    unsigned int vs = compileShader(source.vertexSource, GL_VERTEX_SHADER);
    unsigned int fs = compileShader(source.fragmentSource, GL_FRAGMENT_SHADER);

    glAttachShader(this->shaderId, vs);
    glAttachShader(this->shaderId, fs);
    glLinkProgram(this->shaderId);
    glValidateProgram(this->shaderId);

    glDeleteShader(vs);
    glDeleteShader(fs);

    glUseProgram(this->shaderId);
}

int Shader::getUniformLocation(std::string name) {
    return glGetUniformLocation(this->shaderId, name.c_str());
}

unsigned int Shader::getShaderId() {
    return this->shaderId;
}

void Shader::bind() {
    glUseProgram(this->shaderId);
}

void Shader::setUniform4f(std::string name, float v1, float v2, float v3, float v4) {
    int location = this->getUniformLocation(name);
    glUniform4f(location, v1, v2, v3, v4);
}
