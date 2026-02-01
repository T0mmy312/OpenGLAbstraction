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

void Program::_check() const {
    if (_id == 0)
        throw std::runtime_error("Failed to create program object!");
}

void Program::_ensure() const {
    if (_id == 0)
        throw std::logic_error("Program object does not exist!");
}

void Program::_queryUniformData() {
    _uniformIndexMap.clear();
    _uniformLocationIndexMap.clear();
    _uniformData.clear();

    GLint numUniforms = 0;
    glGetProgramInterfaceiv(_id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);

    _uniformData.reserve(numUniforms);

    GLenum props[] = {
        GL_NAME_LENGTH,
        GL_LOCATION,
        GL_TYPE,
        GL_ARRAY_SIZE
    };

    for (int i = 0; i < numUniforms; ++i) {
        GLint params[4];
        glGetProgramResourceiv(_id, GL_UNIFORM, i, 4, props, 4, nullptr, params);
        if (params[1] == -1)
            continue;

        std::string name(params[0], '\0');
        GLsizei written = 0;
        glGetProgramResourceName(_id, GL_UNIFORM, i, params[0], &written, name.data());
        name.resize(written);

        if (params[3] > 1 && name.ends_with("[0]")) {
            name.erase(name.size() - 3);
        }

        int localIndex = _uniformData.size();
        _uniformIndexMap[name] = localIndex;
        _uniformLocationIndexMap[params[1]] = localIndex;
        _uniformData.push_back({ params[1], static_cast<GLenum>(params[2]), params[3] });
    }
}

void Program::_setupUniform(int loc, int sizeCheck, int typeCheck) const {
    _ensure();
    auto it = _uniformLocationIndexMap.find(loc);
    if (it == _uniformLocationIndexMap.end())
        throw std::invalid_argument("Location does not correspond to a uniform!");
    UniformData data = _uniformData[it->second];
    if (typeCheck != data.glType)
        throw std::runtime_error("Type of data does not correspond to the GLSL data type");
    if (sizeCheck > data.arraySize)
        throw std::invalid_argument("Size of data is greater than the size of the GLSL uniform!");
    if (sizeCheck <= 0)
        throw std::invalid_argument("Size of data to write must be greater than 0!");
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

    _queryUniformData();
}

void Program::bind() const {
    _ensure();
    if (!_linked)
        throw std::runtime_error("Could not bind unlinked Program!");
    GL_CALL(glUseProgram(_id));
}

void Program::unbind() {
    GL_CALL(glUseProgram(0));
}

int Program::getUniformLocation(const std::string& name) const {
    _ensure();
    if (!_linked)
        throw std::runtime_error("Program must be successfully linked before uniforms can be used!");
    auto it = _uniformIndexMap.find(name);
    if (it == _uniformIndexMap.end())
        throw std::invalid_argument("Uniform name: " + name + " does not exist!");

    return _uniformData[it->second].location;
}

void Program::setUniform(int location, float data) { _setupUniform(location, 1, GL_FLOAT); bind(); GL_CALL(glUniform1f(location, data)); }
void Program::setUniform(int location, const glm::vec2& data) { _setupUniform(location, 1, GL_FLOAT_VEC2); bind(); GL_CALL(glUniform2fv(location, 1, &data[0])); }
void Program::setUniform(int location, const glm::vec3& data) { _setupUniform(location, 1, GL_FLOAT_VEC3); bind(); GL_CALL(glUniform3fv(location, 1, &data[0])); }
void Program::setUniform(int location, const glm::vec4& data) { _setupUniform(location, 1, GL_FLOAT_VEC4); bind(); GL_CALL(glUniform4fv(location, 1, &data[0])); }
void Program::setUniform(int location, int data) {_setupUniform(location, 1, GL_INT); bind(); GL_CALL(glUniform1i(location, data)); }
void Program::setUniform(int location, const glm::ivec2& data) { _setupUniform(location, 1, GL_INT_VEC2); bind(); GL_CALL(glUniform2iv(location, 1, &data[0])); }
void Program::setUniform(int location, const glm::ivec3& data) { _setupUniform(location, 1, GL_INT_VEC3); bind(); GL_CALL(glUniform3iv(location, 1, &data[0])); }
void Program::setUniform(int location, const glm::ivec4& data) { _setupUniform(location, 1, GL_INT_VEC4); bind(); GL_CALL(glUniform4iv(location, 1, &data[0])); }
void Program::setUniform(int location, unsigned int data) { _setupUniform(location, 1, GL_UNSIGNED_INT); bind(); GL_CALL(glUniform1ui(location, data)); }
void Program::setUniform(int location, const glm::uvec2& data) { _setupUniform(location, 1, GL_UNSIGNED_INT_VEC2); bind(); GL_CALL(glUniform2uiv(location, 1, &data[0])); }
void Program::setUniform(int location, const glm::uvec3& data) { _setupUniform(location, 1, GL_UNSIGNED_INT_VEC3); bind(); GL_CALL(glUniform3uiv(location, 1, &data[0])); }
void Program::setUniform(int location, const glm::uvec4& data) { _setupUniform(location, 1, GL_UNSIGNED_INT_VEC4); bind(); GL_CALL(glUniform4uiv(location, 1, &data[0])); }
void Program::setUniform(int location, const glm::mat2& data) { _setupUniform(location, 1, GL_FLOAT_MAT2); bind(); GL_CALL(glUniformMatrix2fv(location, 1, false, &data[0][0])); }
void Program::setUniform(int location, const glm::mat3& data) { _setupUniform(location, 1, GL_FLOAT_MAT3); bind(); GL_CALL(glUniformMatrix3fv(location, 1, false, &data[0][0])); }
void Program::setUniform(int location, const glm::mat4& data) { _setupUniform(location, 1, GL_FLOAT_MAT4); bind(); GL_CALL(glUniformMatrix4fv(location, 1, false, &data[0][0])); }
void Program::setUniform(int location, const glm::mat2x3& data) { _setupUniform(location, 1, GL_FLOAT_MAT2x3); bind(); GL_CALL(glUniformMatrix2x3fv(location, 1, false, &data[0][0])); }
void Program::setUniform(int location, const glm::mat3x2& data) { _setupUniform(location, 1, GL_FLOAT_MAT3x2); bind(); GL_CALL(glUniformMatrix3x2fv(location, 1, false, &data[0][0])); }
void Program::setUniform(int location, const glm::mat2x4& data) { _setupUniform(location, 1, GL_FLOAT_MAT2x4); bind(); GL_CALL(glUniformMatrix2x4fv(location, 1, false, &data[0][0])); }
void Program::setUniform(int location, const glm::mat4x2& data) { _setupUniform(location, 1, GL_FLOAT_MAT4x2); bind(); GL_CALL(glUniformMatrix4x2fv(location, 1, false, &data[0][0])); }
void Program::setUniform(int location, const glm::mat3x4& data) { _setupUniform(location, 1, GL_FLOAT_MAT3x4); bind(); GL_CALL(glUniformMatrix3x4fv(location, 1, false, &data[0][0])); }
void Program::setUniform(int location, const glm::mat4x3& data) { _setupUniform(location, 1, GL_FLOAT_MAT4x3); bind(); GL_CALL(glUniformMatrix4x3fv(location, 1, false, &data[0][0])); }

void Program::getUniform(int location, float& data) const { _setupUniform(location, 1, GL_FLOAT); GL_CALL(glGetUniformfv(_id, location, &data)); }
void Program::getUniform(int location, glm::vec2& data) const { _setupUniform(location, 1, GL_FLOAT_VEC2); glGetnUniformfv(_id, location, sizeof(glm::vec2), &data[0]); }
void Program::getUniform(int location, glm::vec3& data) const { _setupUniform(location, 1, GL_FLOAT_VEC3); glGetnUniformfv(_id, location, sizeof(glm::vec3), &data[0]); }
void Program::getUniform(int location, glm::vec4& data) const { _setupUniform(location, 1, GL_FLOAT_VEC4); glGetnUniformfv(_id, location, sizeof(glm::vec4), &data[0]); }
void Program::getUniform(int location, int& data) const { _setupUniform(location, 1, GL_INT); glGetUniformiv(_id, location, &data); }
void Program::getUniform(int location, glm::ivec2& data) const { _setupUniform(location, 1, GL_INT_VEC2); glGetnUniformiv(_id, location, sizeof(glm::ivec2), &data[0]); }
void Program::getUniform(int location, glm::ivec3& data) const { _setupUniform(location, 1, GL_INT_VEC3); glGetnUniformiv(_id, location, sizeof(glm::ivec3), &data[0]); }
void Program::getUniform(int location, glm::ivec4& data) const { _setupUniform(location, 1, GL_INT_VEC4); glGetnUniformiv(_id, location, sizeof(glm::ivec4), &data[0]); }
void Program::getUniform(int location, unsigned int& data) const { _setupUniform(location, 1, GL_UNSIGNED_INT); glGetUniformuiv(_id, location, &data); }
void Program::getUniform(int location, glm::uvec2& data) const { _setupUniform(location, 1, GL_UNSIGNED_INT_VEC2); glGetnUniformuiv(_id, location, sizeof(glm::uvec2), &data[0]); }
void Program::getUniform(int location, glm::uvec3& data) const { _setupUniform(location, 1, GL_UNSIGNED_INT_VEC3); glGetnUniformuiv(_id, location, sizeof(glm::uvec3), &data[0]); }
void Program::getUniform(int location, glm::uvec4& data) const { _setupUniform(location, 1, GL_UNSIGNED_INT_VEC4); glGetnUniformuiv(_id, location, sizeof(glm::uvec4), &data[0]); }
void Program::getUniform(int location, glm::mat2& data) const { _setupUniform(location, 1, GL_FLOAT_MAT2); glGetnUniformfv(_id, location, sizeof(glm::mat2), &data[0][0]); }
void Program::getUniform(int location, glm::mat3& data) const { _setupUniform(location, 1, GL_FLOAT_MAT3); glGetnUniformfv(_id, location, sizeof(glm::mat3), &data[0][0]); }
void Program::getUniform(int location, glm::mat4& data) const { _setupUniform(location, 1, GL_FLOAT_MAT4); glGetnUniformfv(_id, location, sizeof(glm::mat4), &data[0][0]); }
void Program::getUniform(int location, glm::mat2x3& data) const { _setupUniform(location, 1, GL_FLOAT_MAT2x3); glGetnUniformfv(_id, location, sizeof(glm::mat2x3), &data[0][0]); }
void Program::getUniform(int location, glm::mat3x2& data) const { _setupUniform(location, 1, GL_FLOAT_MAT3x2); glGetnUniformfv(_id, location, sizeof(glm::mat3x2), &data[0][0]); }
void Program::getUniform(int location, glm::mat2x4& data) const { _setupUniform(location, 1, GL_FLOAT_MAT2x4); glGetnUniformfv(_id, location, sizeof(glm::mat2x4), &data[0][0]); }
void Program::getUniform(int location, glm::mat4x2& data) const { _setupUniform(location, 1, GL_FLOAT_MAT4x2); glGetnUniformfv(_id, location, sizeof(glm::mat4x2), &data[0][0]); }
void Program::getUniform(int location, glm::mat3x4& data) const { _setupUniform(location, 1, GL_FLOAT_MAT3x4); glGetnUniformfv(_id, location, sizeof(glm::mat3x4), &data[0][0]); }
void Program::getUniform(int location, glm::mat4x3& data) const { _setupUniform(location, 1, GL_FLOAT_MAT4x3); glGetnUniformfv(_id, location, sizeof(glm::mat4x3), &data[0][0]); }

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