#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    //Creates the location of the shader
    unsigned int id = glCreateShader(type);
    //Sets up the string as a c str
    const char* src = source.c_str();
    //Sets the source code of the shader
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    //Gets the result of the compilation
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    //Checks if the shader compiled properly
    if (result == GL_FALSE) {

        //Gets the length of the message
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        //Gets the error message
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);

        std::cout << "Failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
            << " shader!" << std::endl;

        std::cout << message << std::endl;
        glDeleteShader(id);

        return 0;
    }

    //Returns the location of the compiled shader
    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    //Creates the vertex shader
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    //Creates the fragment shader
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    //Attaches the vertex and frag shaders to the program
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    //Links the program
    glLinkProgram(program);
    glValidateProgram(program);

    //Unnecessary to leave shaders in the code as they
    //have already been attached to the program
    //TECHNICALLY should be using detach shader not delete
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
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
        std::cout << "Error" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;


    float positions[6] = { 
       -0.5f, -0.5f,
        0.5f, -0.5f,
        0.0f,  0.5f 
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    //Vertex shader code
    std::string vertexShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec4 position;"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = position;\n"
        "}\n";

    //Fragment shader code
    std::string fragmentShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 color;"
        "\n"
        "void main()\n"
        "{\n"
        "   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";

    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    //Shader clean up
    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}