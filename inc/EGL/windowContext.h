#ifndef WINDOW_CONTEXT
#define WINDOW_CONTEXT

#include <GLFW/glfw3.h>

namespace egl {

/**
 * @brief An abstract class to contain an window based application.
 * 
 * Instantiates a window in the constructor using GLFW and destroys it once it goes out of scope.
 * That window can be bound and used in the child class in the WindowContext::main method implementation containing the setup and rendering loop.
 * GLFW must be initialized only once before Construction of any Context and terminated after completion of every WindowContext::main.
 * Init must be called in WindowContext::main before use of any Easy OpenGL functionality to initialize glew for the current context.
 * Init may be overridden if a custom glew initialization is required, but is not required.
 * 
 * @note WindowContext requires GLFW to be Initialized.
 * @note Calling useContext is recomended in main to ensure the local context is used.
 * @warning GLFW must persist as long as any WindowContext::main is running.
 */
class WindowContext {
protected:
    /**
     * @brief GLFW window handle for low level GLFW access.
     */
    GLFWwindow* window;

    /**
     * @brief Makes the owned window to the current context.
     */
    void useContext();

    /**
     * @brief Initializes glew for the current context.
     * 
     * Should be called in once in WindowContext::main to initialize glew for the current context.
     * 
     * @warning Undefined behaviour occurs if init it is not called in WindowContext::main.
     */
    void init();

public:
    WindowContext();
    WindowContext(WindowContext&& other);
    WindowContext(const WindowContext& other) = delete;
    ~WindowContext();

    virtual void main() = 0;

    WindowContext& operator=(WindowContext&& other);
    WindowContext& operator=(const WindowContext& other) = delete;
};

}

#endif