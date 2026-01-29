#ifndef EGL_PROGRAM_H
#define EGL_PROGRAM_H

#include <string>
#include <stdexcept>

namespace egl {

class Shader;

/**
 * @brief Exception thrown when Program linking fails.
 */
class ProgramLinkError : public std::runtime_error {
public:
    /**
     * @brief Construct a new Program Link Error object.
     * 
     * @param infoLog InfoLog buffer from glGetProgramInfoLog.
     */
    ProgramLinkError(const std::string& infoLog)
        : std::runtime_error(
              "Program link failed:\n" + infoLog) {}
};

/**
 * @brief Exception thrown when Program validation fails, mainly used in debug builds.
 */
class ProgramValidateError : public std::runtime_error {
public:
    /**
     * @brief Construct a new Program Validate Error object.
     * 
     * @param infoLog InfoLog buffer from glGetProgramInfoLog.
     */
    ProgramValidateError(const std::string& infoLog)
        : std::runtime_error(
              "Program validation failed:\n" + infoLog) {}
};

/**
 * @brief Program class to abstract OpenGL Shader Programs.
 * 
 * @warning Program must be deconstructed before the OpenGL context is destroyed.
 * @warning This class is not guaranteed to be thread-safe.
 * 
 * @note This class owns the underlying OpenGL Program object and
 *       releases it upon destruction or reset().
 */
class Program {
protected:
    unsigned int _id = 0;
    bool _linked = false;

    void _delete();
    void _check();
    void _ensure();
    std::string _getError();

public:
    /**
     * @brief Construct an empty Program object.
     */
    Program();
    Program(Program&& other);
    Program(const Program&) = delete; // OpenGL Programs are not copy safe
    ~Program();

    /**
     * @brief Resets the Program to an empty State.
     * 
     * @throws std::runtime_error If OpenGL failed to create a new Program.
     */
    void reset();

    /**
     * @brief Checks if the given Shader is attached to the Program.
     * 
     * @param shader The Shader to check.
     * 
     * @throws std::logic_error If the current Program object does not exist (reset() is recommended to return to a valid state).
     * 
     * @return true if the Shader is attached, false otherwise.
     */
    bool attached(const Shader& shader);

    /**
     * @brief Attaches Shader to Program for linking.
     * 
     * @note Attaching a Shader invalidates the current link state.
     * 
     * @warning Attaching a destroyed Shader is undefined behavior.
     * @warning Attached Shaders must outlive the Program (at least until linking).
     * 
     * @throws std::runtime_error If the Shader has already been attached.
     * @throws std::logic_error If the current Program object does not exist (reset() is recommended to return to a valid state).
     * 
     * @param shader The Shader that should be attached to the Program.
     */
    void attach(const Shader& shader);

    /**
     * @brief Detaches Shader from Program for linking.
     * 
     * @note Detaching a Shader invalidates the current link state.
     * 
     * @throws std::runtime_error If the Shader was not attached.
     * @throws std::logic_error If the current Program object does not exist (reset() is recommended to return to a valid state).
     * 
     * @param shader The Shader that should be detached from the Program.
     */
    void detach(const Shader& shader);

    /**
     * @brief Gets if the Program has been successfully linked.
     * 
     * @return true if the Program is linked, false otherwise.
     */
    bool linked() const { return _linked; }

    /**
     * @brief Links all attached Shaders and creates a valid Program.
     * 
     * @throws std::logic_error If the current Program object does not exist (reset() is recommended to return to a valid state).
     * @throws egl::ProgramLinkError If the Program fails to link. The Program remains in a valid state afterwards.
     * @throws egl::ProgramValidateError If the Program fails to validate. This only occurs when DEBUG_BUILD is defined else the validity is not checked.
     */
    void link();

    /**
     * @brief Binds this Program.
     * 
     * @throws std::logic_error If the current Program object does not exist (reset() is recommended to return to a valid state).
     * @throws std::runtime_error If the Program was not linked to avoid invalid usage.
     */
    void bind();

    /**
     * @brief Unbinds a Program by binding id 0.
     */
    static void unbind();

    Program& operator=(Program&& other);
    Program& operator=(const Program&) = delete; // OpenGL Programs are not copy safe
};

}

#endif