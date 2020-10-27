#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))


static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << "): " << function << " in " << file << " on line: "<< line <<std::endl;
        return false;
    }
    return true;
}

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filePath) {
    std::ifstream stream(filePath);

    enum class ShaderType {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];

    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] <<line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}


static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    //Creates the location of the shader
    unsigned int id = glCreateShader(type);
    //Sets up the string as a c str
    const char* src = source.c_str();
    //Sets the source code of the shader
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    //Gets the result of the compilation
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

    //Checks if the shader compiled properly
    if (result == GL_FALSE) {

        //Gets the length of the message
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));

        //Gets the error message
        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));

        std::cout << "Failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
            << " shader!" << std::endl;

        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));

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
    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    //Links the program
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    //Unnecessary to leave shaders in the code as they
    //have already been attached to the program
    //TECHNICALLY should be using detach shader not delete
    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

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

    //Sets framerate to Vsync
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        std::cout << "Error" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;


    float positions[] = { 
       -0.5f, -0.5f,
        0.5f, -0.5f,
        0.5f,  0.5f,
       -0.5f,  0.5f
    };

    unsigned int indicies[] = {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int vao;
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));


    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW));


    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));


    unsigned int ibo;
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indicies, GL_STATIC_DRAW));


    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));


    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    GLCall(glUseProgram(shader));


    //Retrieve location of uniform in shader
    GLCall(int location = glGetUniformLocation(shader, "u_Color"));
    //Makes sure uniform is there
    ASSERT(location != -1);

    //Have to have a bound shader to use uniforms
    GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));

    //Unbinds all elements
    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));



    float r = 0.0f;
    float increment = .02f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        GLCall(glUseProgram(shader));
        GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

        GLCall(glBindVertexArray(vao));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));


        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        if (r + increment > 1 || r + increment < 0)
        {
            increment *= -1;
        }
        r += increment;

        /* Swap front and back buffers */
        GLCall(glfwSwapBuffers(window));

        /* Poll for and process events */
        GLCall(glfwPollEvents());
    }

    //Shader clean up
    GLCall(glDeleteProgram(shader));

    glfwTerminate();
    return 0;
}