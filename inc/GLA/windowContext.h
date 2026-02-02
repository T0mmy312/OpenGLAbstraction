#ifndef GLA_WINDOW_CONTEXT
#define GLA_WINDOW_CONTEXT

#include <GLFW/glfw3.h>

namespace gla {

/**
 * @brief Initialize GLFW.
 * 
 * Sets up error callbacks and calls glfwInit.
 * 
 * @note gla::initGLFW is not thread safe and may only be called from the main thread.
 * 
 * @return true if GLFW has been successfully initialized.
 */
bool initGLFW();

/**
 * @brief Terminates GLFW.
 * 
 * @note gla::terminateGLFW is not thread safe and may only be called from the main thread.
 */
void terminateGLFW();

/**
 * @brief Polls GLFW events.
 * 
 * @note gla::pollEvents is not thread safe and may only be called from the main thread.
 */
void pollEvents();

/**
 * @brief An abstract class to contain an window based application.
 * 
 * Instantiates a window in the constructor using GLFW and destroys it once it goes out of scope.
 * That window can be bound and used in the child class in the WindowContext::run method implementation containing the setup and rendering loop.
 * 
 * @note Calling useContext is recomended in run to ensure the local context is used.
 * @note gla::WindowContext is not thread safe and may only be used on the main thread.
 */
class WindowContext {
private:
    bool _ownsGLFW = false;

    static void _onResize(GLFWwindow* window, int width, int height);
    static void _onClose(GLFWwindow* window);
    static void _onChar(GLFWwindow* window, unsigned int codepoint);
    static void _onCharMods(GLFWwindow* window, unsigned int codepoint, int mods);
    static void _onCursorEnter(GLFWwindow* window, int entered);
    static void _onCursorPos(GLFWwindow* window, double xpos, double ypos);
    static void _onDrop(GLFWwindow* window, int count, const char** paths);
    static void _onFramebufferSize(GLFWwindow* window, int width, int height);
    static void _onKey(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void _onMouseButton(GLFWwindow* window, int button, int action, int mods);
    static void _onScroll(GLFWwindow* window, double xoffset, double yoffset);
    static void _onWindowContentScale(GLFWwindow* window, float xscale, float yscale);
    static void _onWindowFocus(GLFWwindow* window, int focused);
    static void _onWindowIconify(GLFWwindow* window, int iconified);
    static void _onWindowMaximize(GLFWwindow* window, int maximized);
    static void _onWindowPos(GLFWwindow* window, int xpos, int ypos);
    static void _onWindowRefresh(GLFWwindow* window);

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
     * \defgroup WindowCallbacks Window Callback Virtual Methods
     * 
     * Virtual methods to override for window events.
     * 
     * @warning These are callbacks, so their execution time is not guaranteed. Modifying global state within them (e.g., calling useContext or binding a Buffer) may cause undefined behavior. Use a flag to control or defer such changes.
     */
    /// @ingroup WindowCallbacks
    virtual void onResize(int width, int height) {}                     ///< Called when the window is resized.
    /// @ingroup WindowCallbacks
    virtual void onClose() {}                                           ///< Called when the window is requested to close.
    /// @ingroup WindowCallbacks
    virtual void onChar(unsigned int codepoint) {}                      ///< Called when a Unicode character is input.
    /// @ingroup WindowCallbacks
    virtual void onCharMods(unsigned int codepoint, int mods) {}        ///< Called when a character with modifiers is input.
    /// @ingroup WindowCallbacks
    virtual void onCursorEnter(int entered) {}                          ///< Called when the cursor enters or leaves the window.
    /// @ingroup WindowCallbacks
    virtual void onCursorPos(double xpos, double ypos) {}               ///< Called when the cursor moves.
    /// @ingroup WindowCallbacks
    virtual void onDrop(int count, const char** paths) {}               ///< Called when files are dropped on the window.
    /// @ingroup WindowCallbacks
    virtual void onFramebufferSize(int width, int height) {}            ///< Called when framebuffer size changes.
    /// @ingroup WindowCallbacks
    virtual void onKey(int key, int scancode, int action, int mods) {}  ///< Called when a key is pressed, repeated, or released.
    /// @ingroup WindowCallbacks
    virtual void onMouseButton(int button, int action, int mods) {}     ///< Called when a mouse button is pressed or released.
    /// @ingroup WindowCallbacks
    virtual void onScroll(double xoffset, double yoffset) {}            ///< Called when the scroll wheel is used.
    /// @ingroup WindowCallbacks
    virtual void onWindowContentScale(float xscale, float yscale) {}    ///< Called when the window content scale changes.
    /// @ingroup WindowCallbacks
    virtual void onWindowFocus(int focused) {}                          ///< Called when the window gains or loses focus.
    /// @ingroup WindowCallbacks
    virtual void onWindowIconify(int iconified) {}                      ///< Called when the window is iconified or restored.
    /// @ingroup WindowCallbacks
    virtual void onWindowMaximize(int maximized) {}                     ///< Called when the window is maximized or restored.
    /// @ingroup WindowCallbacks
    virtual void onWindowPos(int xpos, int ypos) {}                     ///< Called when the window position changes.
    /// @ingroup WindowCallbacks
    virtual void onWindowRefresh() {}                                   ///< Called when the window needs to be redrawn.

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