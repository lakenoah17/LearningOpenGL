#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Learning OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //Sets framerate to Vsync
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        std::cout << "Error" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    //Scopes the draw call so there is no infinite loop from the index 
    //buffer not being destroyed before glfw is terminated causing the 
    //error checker to loop infinetly because it doesn't have a valid context
    {
        float positions[] = {
            -50.0f, -50.0f, 0.0f, 0.0f,
             50.0f, -50.0f, 1.0f, 0.0f,
             50.0f,  50.0f, 1.0f, 1.0f,
            -50.0f,  50.0f, 0.0f, 1.0f
        };

        unsigned int indicies[] = {
            0, 1, 2,
            2, 3, 0
        };

        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glEnable(GL_BLEND));

        VertexArray va;
        VertexBuffer vb(positions, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indicies, 6);

        Renderer renderer;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));


        glm::vec3 translationA(200, 200, 0);
        glm::vec3 translationB(400, 200, 0);

        glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));


        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        

        Texture texture("res/textures/destroyer.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        //Unbinds all elements
        va.UnBind();
        shader.UnBind();
        vb.UnBind();
        ib.UnBind();

        float r = 0.0f;
        float increment = .02f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            ImGui_ImplOpenGL3_NewFrame();

            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();


            //What is entailed in a draw call
            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
                glm::mat4 mvp = proj * view * model;
                shader.Bind();
                shader.SetUniformMat4f("u_MVP", mvp);

                renderer.Draw(va, ib, shader);
            }


            //What is entailed in a draw call
            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
                glm::mat4 mvp = proj * view * model;
                shader.Bind();
                shader.SetUniformMat4f("u_MVP", mvp);

                renderer.Draw(va, ib, shader);
            }

            if (r + increment > 1 || r + increment < 0)
            {
                increment *= -1;
            }
            r += increment;


            {
                ImGui::SliderFloat3("Translation A", &translationA.x, 0.0f, 960.0f);
                ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f, 960.0f);
                ImGui::Text("Application avg %.3f", 1000.0f / ImGui::GetIO().Framerate);
            }


            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            GLCall(glfwSwapBuffers(window));

            /* Poll for and process events */
            GLCall(glfwPollEvents());
        }
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}