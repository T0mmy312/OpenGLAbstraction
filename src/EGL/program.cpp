#include <vector>

#include <EGL/program.h>
#include <EGL/shader.h>

#include <EGL/debug.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace egl {

// ----------------------------------------------------------------------------------------------------
// class Program
// ----------------------------------------------------------------------------------------------------

// --------------------------------------------------
// protected methods
// --------------------------------------------------

void Program::_delete() {
    if (_id != 0)
        GL_CALL(glDeleteProgram(_id));
    _linked = false;
    _id = 0;
}

void Program::_check() {
    if (_id == 0)
        throw std::runtime_error("Failed to create program object!");
}

void Program::_ensure() {
    if (_id == 0)
        throw std::logic_error("Program object does not exist!");
}

std::string Program::_getError() {
    GLint length;
    GL_CALL(glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &length));
    if (length <= 1) return "";
    std::string message;
    message.resize(length);
    GL_CALL(glGetProgramInfoLog(_id, length, &length, message.data()));
    return message;
}

// --------------------------------------------------
// Constructors / Destructors
// --------------------------------------------------

Program::Program() {
    GL_CALL(_id = glCreateProgram());
    _check();
}
Program::Program(Program&& other)
    : _id(other._id), _linked(other._linked) {
    other._id = 0;
    other._linked = false;
}

Program::~Program() { _delete(); }

// --------------------------------------------------
// public methods
// --------------------------------------------------

void Program::reset() {
    _delete();
    GL_CALL(_id = glCreateProgram());
    _check();
}

bool Program::attached(const Shader& shader) {
    _ensure();
    int count;
    GL_CALL(glGetProgramiv(_id, GL_ATTACHED_SHADERS, &count));
    std::vector<unsigned int> shaders(count);
    GL_CALL(glGetAttachedShaders(_id, count, &count, shaders.data()));
    for (int i = 0; i < count; i++)
        if (shaders[i] == shader._id)
            return true;
    return false;
}

void Program::attach(const Shader& shader) {
    _ensure();
    if (attached(shader))
        throw std::runtime_error("Given shader is already attached to program, so it can't be attached!");
    GL_CALL(glAttachShader(_id, shader._id));
    _linked = false;
}

void Program::detach(const Shader& shader) {
    _ensure();
    if (!attached(shader))
        throw std::runtime_error("Given shader is not attached to program, so it can't be detached!");
    GL_CALL(glDetachShader(_id, shader._id));
    _linked = false;
}

void Program::link() {
    _ensure();
    _linked = false;

    GLint result;

    GL_CALL(glLinkProgram(_id));
    GL_CALL(glGetProgramiv(_id, GL_LINK_STATUS, &result));
    if (result == GL_FALSE) {
        _linked = false;
        std::string message = _getError();
        throw ProgramLinkError(message);
        return;
    }
    
DEBUG_ONLY(

    GL_CALL(glValidateProgram(_id));
    GL_CALL(glGetProgramiv(_id, GL_VALIDATE_STATUS, &result));
    if (result == GL_FALSE) {
        _linked = false;
        std::string message = _getError();
        throw ProgramValidateError(message);
        return;
    }

)

    _linked = true;
}

void Program::bind() {
    _ensure();
    if (!_linked)
        throw std::runtime_error("Could not bind unlinked Program!");
    GL_CALL(glUseProgram(_id));
}

void Program::unbind() {
    GL_CALL(glUseProgram(0));
}

// --------------------------------------------------
// operator overloads
// --------------------------------------------------

Program& Program::operator=(Program&& other) {
    if (&other != this) {
        _delete();
        _id = other._id;
        _linked = other._linked;
        other._id = 0;
        other._linked = false;
    }
    return *this;
}

}