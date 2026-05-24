#pragma once
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


using namespace std;

class SimpleShader {
private:
    GLuint shader_program;
    string readFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "ERROR: Cannot open file " << filename << endl;
            return "";
        }
        stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    GLuint compileShader(const string& source, GLenum type) {
        GLuint shader = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        GLint success;
        GLchar infoLog[1024];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            cerr << "Shader compilation error: " << infoLog << endl;
        }
        return shader;
    }

public:
    bool loadFromFiles(const string& vertexPath, const string& fragmentPath) {
        string vertexSource = readFile(vertexPath);
        string fragmentSource = readFile(fragmentPath);

        if (vertexSource.empty() || fragmentSource.empty()) {
            return false;
        }

        GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
        GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

        shader_program = glCreateProgram();
        glAttachShader(shader_program, vertexShader);
        glAttachShader(shader_program, fragmentShader);
        glLinkProgram(shader_program);

        GLint success;
        GLchar infoLog[1024];
        glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader_program, 1024, nullptr, infoLog);
            cerr << "Linking error: " << infoLog << endl;
            return false;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return true;
    }

    void use() {
        glUseProgram(shader_program);
    }

    void setUniform(const string& name, float v0, float v1) {
        GLint location = glGetUniformLocation(shader_program, name.c_str());
        glUniform2f(location, v0, v1);
    }

    void setUniform(const string& name, float v0, float v1, float v2) {
        GLint location = glGetUniformLocation(shader_program, name.c_str());
        glUniform3f(location, v0, v1, v2);
    }

    void setUniform(const string& name, float v0, float v1, float v2, float v3) {
        GLint location = glGetUniformLocation(shader_program, name.c_str());
        glUniform4f(location, v0, v1, v2, v3);
    }

    void setUniform(const string& name, float v0) {
        GLint location = glGetUniformLocation(shader_program, name.c_str());
        glUniform1f(location, v0);
    }

    void setUniform(const string& name, const glm::mat4& mat) {
        GLint location = glGetUniformLocation(shader_program, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    }

    void setUniform(const string& name, const glm::mat3& mat) {
        GLint location = glGetUniformLocation(shader_program, name.c_str());
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    }

};