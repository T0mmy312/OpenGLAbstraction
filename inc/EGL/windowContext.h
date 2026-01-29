#ifndef WINDOW_CONTEXT
#define WINDOW_CONTEXT

#include <GLFW/glfw3.h>

namespace egl {

/**
 * @brief Initialize GLFW.
 * 
 * Sets up error callbacks and calls glfwInit.
 * 
 * @note egl::initGLFW is thread safe.
 * 
 * @return true if GLFW has been successfully initialized.
 */
bool initGLFW();

/**
 * @brief Terminates GLFW.
 * 
 * @note egl::terminateGLFW is thread safe.
 */
void terminateGLFW();

/**
 * @brief An abstract class to contain an window based application.
 * 
 * Instantiates a window in the constructor using GLFW and destroys it once it goes out of scope.
 * That window can be bound and used in the child class in the WindowContext::run method implementation containing the setup and rendering loop.
 * 
 * @note Calling useContext is recomended in run to ensure the local context is used.
 * @warning WindowContext may not be shared between threads, but multiple can be instantiated and used on diffrent threads as long as the GLFW Window is not shared. 
 */
class WindowContext {
private:
    bool _ownsGLFW = false;

protected:
    /**
     * @brief GLFW window handle for low level GLFW access.
     */
    GLFWwindow* window = NULL;

    /**
     * @brief Makes the owned window the current context.
     * 
     * @throws std::runtime_error If the GLFW window is invalid.
     */
    void useContext();

    /**
     * @brief Checks if the Window should close.
     * 
     * @throws std::runtime_error If the GLFW Window is invalid.
     */
    bool shouldClose();

    /**
     * @brief Swaps the display buffers.
     * 
     * @throws std::runtime_error If the GLFW Window is invalid.
     */
    void swapBuffers();

    /**
     * @brief Poll GLFW events.
     * 
     * @throws std::runtime_error If the GLFW window is invalid.
     */
    void pollEvents();

public:
    /**
     * @brief Construct a new Window Context object with given height, width and name.
     * 
     * @throws std::runtime_error If GLFW could not create a Window.
     * @throws std::runtime_error If the GLFW Window is invalid.
     * @throws std::runtime_error If GLEW could not be initialized.
     * 
     * @param width Width of the Window in pixels.
     * @param height Height of the Window in pixels.
     * @param windowName C-style NULL terminated string window name.
     */
    WindowContext(int width, int height, const char* windowName);
    WindowContext(WindowContext&& other);
    WindowContext(const WindowContext& other) = delete;
    ~WindowContext();

    /**
     * @brief Virtual run method to overload when implementing a child class.
     * 
     * Virtual run method to contain the rendering and window handling for your Application.
     * 
     * @note Calling useContext is recomended in run to ensure the local context is used.
     */
    virtual void run() = 0;

    WindowContext& operator=(WindowContext&& other);
    WindowContext& operator=(const WindowContext& other) = delete;
};

}

#endif