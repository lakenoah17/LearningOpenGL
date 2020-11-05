#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "tests/TestClearColor.h"
#include "tests/TestTexture2D.h"
#include "tests/TestBatchRendering.h"


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

        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glEnable(GL_BLEND));


        Renderer renderer;

        //Initializes the ImGui library
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

        test::Test* currentTest = nullptr;
        test::TestMenu* testMenu = new test::TestMenu(currentTest);
        currentTest = testMenu;

        testMenu->RegisterTest<test::TestClearColor>("Clear Color");
        testMenu->RegisterTest<test::TestTexture2D>("2D Texture");
        testMenu->RegisterTest<test::TestBatchRendering>("Batch Rendering");

        while (!glfwWindowShouldClose(window))
        {
            renderer.Clear();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (currentTest)
            {
                currentTest->OnUpdate(0.0f);
                currentTest->OnRender();

                ImGui::Begin("Test");

                if (currentTest != testMenu && ImGui::Button("<-"))
                {
                    delete currentTest;
                    currentTest = testMenu;
                }

                currentTest->OnImGuiRender();

                ImGui::End();
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            GLCall(glfwSwapBuffers(window));

            GLCall(glfwPollEvents());
        }
        
        //Cleans up memory at end of the loop
        delete currentTest;
        if (currentTest != testMenu)
        {
            delete testMenu;
        }
    }


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}