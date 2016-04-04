/* OpenGL dev - code
 *
 * Author: KienLTb
 * build command:
 *   g++ -o 01_skeleton  01_skeleton.cpp Program.cpp Shader.cpp -lGL -lglfw -lGLEW
 */

#include "Program.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdlib> // EXIT_FAILURE, EXIT_SUCCESS


// globals
GLFWwindow* gWindow = NULL;
GLuint gVAO = 0;
GLuint gVBO = 0;

krogl::Program* gProgram = NULL;

// shader source code
std::string vertex_source =
    "#version 150\n"
    "in vec3 vert;\n"
    "void main() {"
    "    // does not alter the verticies at all\n"
    "   gl_Position = vec4(vert, 1);\n"
    "}\n";

std::string fragment_source =
    "#version 150\n"
    "out vec4 finalColor;\n"
    "void main() {\n"
    "    finalColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
    "}\n";

void OnError(int errorCode, const char* msg)
{
    throw std::runtime_error(msg);
}

void CreateWindow(int width, int hight)
{
    // Initialse GLFW
    glfwSetErrorCallback(OnError);
    if(!glfwInit())
        throw std::runtime_error("glfwInit failed");

    // Open a window with GLFW
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    gWindow = glfwCreateWindow(width, hight, "OpenGL test", NULL, NULL);
    if(!gWindow)
        throw std::runtime_error("glfwCreateWindow failed. Can your hardware handle OpenGL 3.2?");
}

void InitGlfw()
{
    // GLFW settings
    glfwMakeContextCurrent(gWindow);

    // initialise GLEW
    glewExperimental = GL_TRUE; // Stops glew crashing on OSX
    if(glewInit() != GLEW_OK)
        throw std::runtime_error("glewInit failed");

    // Print out some info about the graphics drivers
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    // make sure OpenGL version 3.2 API is available
    if(!GLEW_VERSION_3_2)
        throw std::runtime_error("OpenGL 3.2 API is not available");
}


void LoadShader()
{
     std::vector<krogl::Shader> Shaders;
     Shaders.push_back(krogl::Shader(vertex_source, GL_VERTEX_SHADER));
     Shaders.push_back(krogl::Shader(fragment_source, GL_FRAGMENT_SHADER));

     /* Linked shader*/
     gProgram = new krogl::Program(Shaders);
}

void LoadTriangle()
{
    // make and bind the VAO
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);

    // make and bind the VBO
    glGenBuffers(1, &gVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO);

    // Put the three triangle verticies into the VBO
    // GLfloat vertexData[] = {
    //     //  X     Y     Z
    //      0.0f, 0.8f, 0.0f,
    //     -0.8f,-0.8f, 0.0f,
    //      0.8f,-0.8f, 0.0f,
    // };

    // Put the four square verticies into VBO
    // Draw 2 triangles for draw square
    GLfloat vertexData[] = {
        //  X     Y     Z
        -0.8f, -0.8f, 0.0f,
        -0.8f,  0.8f, 0.0f,
         0.8f,  0.8f, 0.0f,

         0.8f,  0.8f, 0.0f,
         0.8f, -0.8f, 0.0f,
         -0.8f, -0.8f, 0.0f,

    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    // connect the xyz to the "vert" attribute of the vertex shader
    glEnableVertexAttribArray(gProgram->attrib("vert"));
    glVertexAttribPointer(gProgram->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // unbind the VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Render()
{
    // clear everything
    glClearColor(0, 0, 0, 1); // black
    glClear(GL_COLOR_BUFFER_BIT);

    // bind the program (the shaders)
    glUseProgram(gProgram->object());

    // bind the VAO (the triangle)
    glBindVertexArray(gVAO);

    // draw the VAO
    // glDrawArrays(GL_TRIANGLES, 0, 3);

    /* Draw square */
    glDrawArrays(GL_TRIANGLES, 0, 2*3);// Draw 2 triangles for draw square

    // unbind the VAO
    glBindVertexArray(0);

    // unbind the program
    glUseProgram(0);

    // swap the display buffers (displays what was just drawn)
    glfwSwapBuffers(gWindow);
}


int AppMain()
{
    /* Create a window*/
    CreateWindow(800, 600);

    /* Init and check verison of GLFW lib */
    InitGlfw();

    /* Load and compile shader */
    LoadShader();

    /* Load triangle to VAO */
    LoadTriangle();

    /* Run while the window is open*/
    while(!glfwWindowShouldClose(gWindow)){
        // process pending events
        glfwPollEvents();

        // draw one frame
        Render();
    }

    // clean up and exit
    glfwTerminate();

    return 0;
}

int main(int argc, char const *argv[])
{
    try{
        AppMain();
    }catch(const std::exception& e){
        std::cerr << "ERROR " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
