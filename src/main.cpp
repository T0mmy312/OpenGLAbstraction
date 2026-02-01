#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <GLA/program.h>
#include <GLA/shader.h>
#include <GLA/buffer.h>
#include <GLA/debug.h>
#include <GLA/windowContext.h>

class TestWindow : public gla::WindowContext {
public:
    TestWindow(int width, int height, const char* name) : gla::WindowContext(width, height, name) {}

    void run() override {
        useContext();

        std::vector<float> positions = {
            -0.5f, -0.5f,
             0.0f,  0.5f,
             0.5f, -0.5f,

             0.5f, -0.5f,
             0.0f,  0.5f,
             1.0f,  0.5f
        };

        gla::Buffer vbo(gla::BufferType::Array);
        vbo.setData(positions, gla::BufferUsage::StaticDraw);
        vbo.bind();
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)0); // define attribute 0 of the vbo

        gla::Shader vertex(gla::ShaderType::Vertex, std::ifstream("../../res/shaders/basicTriangle/vertex.shader"));
        gla::Shader fragment(gla::ShaderType::Fragment, std::ifstream("../../res/shaders/basicTriangle/fragment.shader"));
        gla::Program program;

        program.attach(vertex);
        program.attach(fragment);

        program.link();

        program["uColor"] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

        program.bind();

        auto start = std::chrono::system_clock::now();

        while (!shouldClose())
        {
            GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

            auto duration = start - std::chrono::system_clock::now();
            float val = (std::sin(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.0f) + 1) / 2.0f;
            program["uColor"] = glm::vec4(1.0f, val, val, 1.0f);

            GL_CALL(glDrawArrays(GL_TRIANGLES, 0, (int)(positions.size() / 2)));

            swapBuffers();

            gla::pollEvents();
        }
    }
};

int main(void)
{
    TestWindow window(640, 480, "Test Window");
    window.run();

    return 0;
}