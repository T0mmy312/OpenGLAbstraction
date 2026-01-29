#ifndef EGL_DEBUG_H
#define EGL_DEBUG_H

namespace egl {

/**
 * @brief Gets a c-style string from from a OpenGL enum.
 * 
 * @note Returns "UNKNOWN_ERROR" if the error is not known.
 * @note Mainly used in the GL_CALL macro in the backend of the Easy OpenGL abstraction.
 * 
 * @param err OpenGL error enum to get the string of
 * @return c-style NULL terminated string 
 */
const char* glErrorString(unsigned int err);

/**
 * @brief Checks and prints if any OpenGL error has occured.
 * 
 * Iterates over the OpenGL error flags with glGetError and prints them until no error are found anymore.
 * 
 * @note Mainly used in the GL_CALL macro in the backend of the Easy OpenGL abstraction. 
 * 
 * @param func The line of code that is being checked as a c-style string for debug output
 * @param file The file in which this function was called as a c-style string for debug output
 * @param line The line on which this function was called for debug output
 */
void glCheckError(const char* func, const char* file, int line);

};

/** \def GL_CALL(x)
 * @brief Runs the given code and checks for errors.
 * 
 * @note For performance reasons egl::glCheckError is only called when DEBUG_BUILD is defined.
 * @warning The code is run in a scope below the current.
 * 
 * @param x The code to run before checking for errors
 */

/** \def DEBUG_ONLY(x)
 * @brief Runs given code only if DEBUG_BUILD is defined.
 */

#ifdef DEBUG_BUILD

    #define GL_CALL(x) do { x; egl::glCheckError(#x, __FILE__, __LINE__); } while(0)

    #define DEBUG_ONLY(x) x

#else

    #define GL_CALL(x) x

    #define DEBUG_ONLY(x) ((void)0)

#endif

#endif