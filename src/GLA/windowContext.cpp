#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <GLA/windowContext.h>

#include <mutex>
#include <stdexcept>
#include <iostream>

namespace gla {

namespace {
    int glfwRefCount = 0;
    bool glfwInitialized = false;
}

void glfwErrorCallback(int error, const char* description) {
    std::cerr << "GLFW error (" << error << "): " << description << std::endl;
}

bool initGLFW() {
    if (!glfwInitialized) {
        glfwSetErrorCallback(glfwErrorCallback);
        if (!glfwInit())
            return false;
        glfwInitialized = true;
    }

    ++glfwRefCount;
    return true;
}

void terminateGLFW() {
     if (!glfwInitialized)
        return;

    --glfwRefCount;

    if (glfwRefCount == 0) {
        glfwTerminate();
        glfwInitialized = false;
    }
}

void pollEvents() {
    glfwPollEvents();
}

// ----------------------------------------------------------------------------------------------------
// class Program
// ----------------------------------------------------------------------------------------------------

// --------------------------------------------------
// protected methods
// --------------------------------------------------

void WindowContext::useContext() {
    if (window == NULL)
        throw std::runtime_error("GLFW Window is invalid!");
    glfwMakeContextCurrent(window);
}

bool WindowContext::shouldClose() {
    if (window == NULL)
        throw std::runtime_error("GLFW Window is invalid!");
    return glfwWindowShouldClose(window);
}

void WindowContext::swapBuffers() {
    if (window == NULL)
        throw std::runtime_error("GLFW Window is invalid!");
    glfwSwapBuffers(window);
}

// --------------------------------------------------
// static callbacks
// --------------------------------------------------

// ----------------------------
// Static wrapper callbacks
// ----------------------------

void WindowContext::_onResize(GLFWwindow* window, int width, int height) {
    WindowContext* self = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if (self) self->onResize(width, height);
}

void WindowContext::_onClose(GLFWwindow* window) {
    WindowContext* self = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if (self) self->onClose();
}

void WindowContext::_onChar(GLFWwindow* window, unsigned int codepoint) {
    WindowContext* self = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if (self) self->onChar(codepoint);
}

void WindowContext::_onCharMods(GLFWwindow* window, unsigned int codepoint, int mods) {
    WindowContext* self = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if (self) self->onCharMods(codepoint, mods);
}

void WindowContext::_onCursorEnter(GLFWwindow* window, int entered) {
    WindowContext* self = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if (self) self->onCursorEnter(entered);
}

void WindowContext::_onCursorPos(GLFWwindow* window, double xpos, double ypos) {
    WindowContext* self = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if (self) self->onCursorPos(xpos, ypos);
}

void WindowContext::_onDrop(GLFWwindow* window, int count, const char** paths) {
    WindowContext* self = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if (self) self->onDrop(count, paths);
}

void WindowContext::_onFramebufferSize(GLFWwindow* window, int width, int height) {
    WindowContext* self = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if (self) self->onFramebufferSize(width, height);
}

void WindowContext::_onKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
    WindowContext* self = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if (self) self->onKey(key, scancode, action, mods);
}

void WindowContext::_onMouseButton(GLFWwindow* window, int button, int action, int mods) {
    WindowContext* self = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if (self) self->onMouseButton(button, action, mods);
}

void WindowContext::_onScroll(GLFWwindow* window, double xoffset, double yoffset) {
    WindowContext* self = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if (self) self->onScroll(xoffset, yoffset);
}

void WindowContext::_onWindowContentScale(GLFWwindow* window, float xscale, float yscale) {
    WindowContext* self = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if (self) self->onWindowContentScale(xscale, yscale);
}

void WindowContext::_onWindowFocus(GLFWwindow* window, int focused) {
    WindowContext* self = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if (self) self->onWindowFocus(focused);
}

void WindowContext::_onWindowIconify(GLFWwindow* window, int iconified) {
    WindowContext* self = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if (self) self->onWindowIconify(iconified);
}

void WindowContext::_onWindowMaximize(GLFWwindow* window, int maximized) {
    WindowContext* self = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if (self) self->onWindowMaximize(maximized);
}

void WindowContext::_onWindowPos(GLFWwindow* window, int xpos, int ypos) {
    WindowContext* self = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if (self) self->onWindowPos(xpos, ypos);
}

void WindowContext::_onWindowRefresh(GLFWwindow* window) {
    WindowContext* self = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if (self) self->onWindowRefresh();
}

// --------------------------------------------------
// Constructors / Destructors
// --------------------------------------------------

WindowContext::WindowContext(int width, int height, const char* windowName) {
    initGLFW();
    window = glfwCreateWindow(width, height, windowName, NULL, NULL);
    if (window == NULL)
        throw std::runtime_error("Could not create a GLFW Window!");
    _ownsGLFW = true;

    useContext();
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        throw std::runtime_error("Could not initialize GLEW!");
    
    glfwSetWindowUserPointer(window, this);

    glfwSetWindowSizeCallback(window, _onResize);
    glfwSetWindowCloseCallback(window, _onClose);
    glfwSetCharCallback(window, _onChar);
    glfwSetCharModsCallback(window, _onCharMods);
    glfwSetCursorEnterCallback(window, _onCursorEnter);
    glfwSetCursorPosCallback(window, _onCursorPos);
    glfwSetDropCallback(window, _onDrop);
    glfwSetFramebufferSizeCallback(window, _onFramebufferSize);
    glfwSetKeyCallback(window, _onKey);
    glfwSetMouseButtonCallback(window, _onMouseButton);
    glfwSetScrollCallback(window, _onScroll);
    glfwSetWindowContentScaleCallback(window, _onWindowContentScale);
    glfwSetWindowFocusCallback(window, _onWindowFocus);
    glfwSetWindowIconifyCallback(window, _onWindowIconify);
    glfwSetWindowMaximizeCallback(window, _onWindowMaximize);
    glfwSetWindowPosCallback(window, _onWindowPos);
    glfwSetWindowRefreshCallback(window, _onWindowRefresh);
}

WindowContext::WindowContext(WindowContext&& other) : window(other.window), _ownsGLFW(other._ownsGLFW) {
    other.window = NULL;
    other._ownsGLFW = false;

    glfwSetWindowUserPointer(window, this);
}

WindowContext::~WindowContext() {
    if (window != NULL)
        glfwDestroyWindow(window);
    if (_ownsGLFW)
        terminateGLFW();
}

// --------------------------------------------------
// operator overloads
// --------------------------------------------------

WindowContext& WindowContext::operator=(WindowContext&& other) {
    if (this != &other) {
        if (window != NULL) {
            glfwDestroyWindow(window);
            if (_ownsGLFW) terminateGLFW();
        }
        window = other.window;
        _ownsGLFW = other._ownsGLFW;
        other.window = NULL;
        other._ownsGLFW = false;
    }
    return *this;
}

};