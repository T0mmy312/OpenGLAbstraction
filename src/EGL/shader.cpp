#include <istream>
#include <sstream>
#include <iostream>

#include <EGL/shader.h>
#include <EGL/program.h>

#include <EGL/debug.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace egl {

constexpr std::string shaderTypeToString(ShaderType type) {
    switch (type) {
    case ShaderType::Fragment: return "FRAGMENT";
    case ShaderType::Vertex: return "VERTEX";
    case ShaderType::Geometry: return "GEOMETRY";
    case ShaderType::TessEvaluation: return "TESS_EVALUATION";
    case ShaderType::TessControl: return "TESS_CONTROL";
    case ShaderType::Compute: return "COMPUTE";
    default: return "INVALID";
    }
}

unsigned int toGLenum(ShaderType type) {
    switch (type) {
    case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
    case ShaderType::Vertex: return GL_VERTEX_SHADER;
    case ShaderType::Geometry: return GL_GEOMETRY_SHADER;
    case ShaderType::TessEvaluation: return GL_TESS_EVALUATION_SHADER;
    case ShaderType::TessControl: return GL_TESS_CONTROL_SHADER;
    case ShaderType::Compute: return GL_COMPUTE_SHADER;
    }
    throw std::logic_error("ShaderType with value: " + std::to_string((int)type) + " is not a valid ShaderType!");
}

// ----------------------------------------------------------------------------------------------------
// class Shader
// ----------------------------------------------------------------------------------------------------

// --------------------------------------------------
// protected methods
// --------------------------------------------------

void Shader::_delete() {
    if (_id != 0)
        GL_CALL(glDeleteShader(_id));
    _compiled = false;
    _id = 0;
}

void Shader::_check() {
    if (_id == 0)
        throw std::runtime_error("Failed to create shader object!");
}

void Shader::_ensure() {
    if (_id == 0)
        throw std::logic_error("Shader object does not exist!");
}

std::string Shader::_getError() {
    GLint length;
    GL_CALL(glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &length));
    if (length <= 1) return "";
    std::string message;
    message.resize(length);
    GL_CALL(glGetShaderInfoLog(_id, length, &length, message.data()));
    return message;
}

// --------------------------------------------------
// constructors / destructors
// --------------------------------------------------

Shader::Shader(ShaderType type) : _type(type) {
    GL_CALL(_id = glCreateShader(toGLenum(_type)));
    _check();
}
Shader::Shader(ShaderType type, const char* src) : _type(type) {
    GL_CALL(_id = glCreateShader(toGLenum(_type)));
    _check();
    compile(src);
}
Shader::Shader(ShaderType type, const std::string& src) : _type(type) {
    GL_CALL(_id = glCreateShader(toGLenum(_type)));
    _check();
    compile(src);
}
Shader::Shader(ShaderType type, std::istream& in) : _type(type) { 
    GL_CALL(_id = glCreateShader(toGLenum(_type)));
    _check();
    compile(in);
}
Shader::Shader(ShaderType type, std::istream&& in) : _type(type) { 
    GL_CALL(_id = glCreateShader(toGLenum(_type)));
    _check();
    compile(in);
}

Shader::Shader(Shader&& other) : _id(other._id), _type(other._type), _compiled(other._compiled) { other._id = 0; other._compiled = false; }

Shader::~Shader() noexcept { _delete(); }

// --------------------------------------------------
// public methods
// --------------------------------------------------

void Shader::reset() {
    _delete();
    GL_CALL(_id = glCreateShader(toGLenum(_type)));
    _check();
}

void Shader::compile(const char* src) {
    if (!src)
        throw std::invalid_argument("Shader source is null!");

    _ensure();

    _compiled = false;
    GL_CALL(glShaderSource(_id, 1, &src, NULL));
    GL_CALL(glCompileShader(_id));

    GLint result;
    GL_CALL(glGetShaderiv(_id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE) {
        std::string message = _getError();
        throw ShaderCompileError(_type, message);
        return;
    }
    _compiled = true;
}

void Shader::compile(std::istream& in) {
    if (!in.good())
        throw std::invalid_argument("Given input stream is invalid, good() failed!");
    std::ostringstream sstr;
    sstr << in.rdbuf();
    if (!in && !in.eof())
        throw std::invalid_argument("Failed while reading shader source!");
    std::string str = sstr.str();
    compile(str.c_str());
}

void Shader::compile(std::istream&& in) {
    if (!in.good())
        throw std::invalid_argument("Given input stream is invalid, good() failed!");
    std::ostringstream sstr;
    sstr << in.rdbuf();
    if (!in && !in.eof())
        throw std::invalid_argument("Failed while reading shader source!");
    std::string str = sstr.str();
    compile(str.c_str());
}

void Shader::compile(const std::string& str) { compile(str.c_str()); }

// --------------------------------------------------
// operators
// --------------------------------------------------

Shader& Shader::operator=(Shader&& other) {
    if (this != &other) {
        _delete();
        _id = other._id;
        _type = other._type;
        _compiled = other._compiled;
        other._id = 0;
        other._compiled = false;
    }
    return *this;
}

}