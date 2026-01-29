#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <EGL/program.h>
#include <EGL/shader.h>
#include <EGL/buffer.h>
#include <EGL/debug.h>

void app(GLFWwindow* window) {
    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };

    egl::Buffer vbo(egl::BufferType::Array);
    vbo.setData(6 * sizeof(float), (void*)positions, egl::BufferUsage::StaticDraw);
    vbo.bind();
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)0); // define attribute 0 of the vbo

    egl::Shader vertex(egl::ShaderType::Vertex, std::ifstream("../../res/shaders/basicTriangle/vertex.shader"));
    egl::Shader fragment(egl::ShaderType::Fragment, std::ifstream("../../res/shaders/basicTriangle/fragment.shader"));
    egl::Program program;

    program.attach(vertex);
    program.attach(fragment);

    program.link();

    program.bind();

    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

        GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 3));

        /* Swap front and back buffers */
        GL_CALL(glfwSwapBuffers(window));

        /* Poll for and process events */
        GL_CALL(glfwPollEvents());
    }
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "Could not init glew!" << std::endl;
        return -1;
    }

    std::cout << "Current GL version: " << glGetString(GL_VERSION) << std::endl;

    app(window);

    glfwTerminate();
    return 0;
}