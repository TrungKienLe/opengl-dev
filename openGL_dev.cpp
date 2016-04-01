/* OpenGL dev - code
 *
 * Author: KienLTb
 */

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

/* Check shader compiler errors - throw exception if fail*/
 void check_shader_compile_status(GLuint obj){
    GLint status;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        std::string msg("Compile failure in shader:\n");

        GLint infoLogLength;
        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* strInfoLog = new char[infoLogLength + 1];
        glGetShaderInfoLog(obj, infoLogLength, NULL, strInfoLog);
        msg += strInfoLog;
        delete[] strInfoLog;

        glDeleteShader(obj); obj = 0;
        throw std::runtime_error(msg);
    }
 }

/* Check and displa for shader linker error - throw exception if fail*/
void check_program_link_status(GLuint obj){
    GLint status;
    glGetProgramiv(obj, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        std::string msg("Program linking failure: ");

        GLint infoLogLength;
        glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* strInfoLog = new char[infoLogLength + 1];
        glGetProgramInfoLog(obj, infoLogLength, NULL, strInfoLog);
        msg += strInfoLog;
        delete[] strInfoLog;

        glDeleteProgram(obj); obj = 0;
        throw std::runtime_error(msg);
    }
}

void OnError(int errorCode, const char* msg)
{
    throw std::runtime_error(msg);
}

void SetupWindow(int width, int hight)
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

GLuint shader_program;

void LoadShader()
{
    const char *source;
    int length;

    /* Create and compiler vertex shader*/
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    source = vertex_source.c_str();
    length = vertex_source.size();
    glShaderSource(vertex_shader, 1, &source, &length);
    glCompileShader(vertex_shader);
    check_shader_compile_status(vertex_shader);


    /* Create and compile fragment shader*/
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    source = fragment_source.c_str();
    length = fragment_source.size();
    glShaderSource(fragment_shader, 1, &source, &length);
    glCompileShader(fragment_shader);
    check_shader_compile_status(fragment_shader);

    /* Create program*/
    shader_program = glCreateProgram();

    /* Attach shaders */
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    /* Link the program and check error*/
    glLinkProgram(shader_program);
    check_program_link_status(shader_program);
}

void LoadTriangle()
{
    /*Make and bind the VAO*/
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);

    /* Make and bind VBO*/
    glGenBuffers(1, &gVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO);

    /* Put the three triangle vertices in to VBO*/
    GLfloat vertexData[] = {
        //  X     Y     Z
         0.0f, 0.8f, 0.0f,
        -0.8f,-0.8f, 0.0f,
         0.8f,-0.8f, 0.0f,
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    /* Connect the xyz to the "vert" attribute of the vertex shader */
    // set up generic attrib pointers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

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
    glUseProgram(shader_program);

    // bind the VAO (the triangle)
    glBindVertexArray(gVAO);

    // draw the VAO
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // unbind the VAO
    glBindVertexArray(0);

    // unbind the program
    glUseProgram(0);

    // swap the display buffers (displays what was just drawn)
    glfwSwapBuffers(gWindow);
}


int AppMain()
{
    int width = 800;
    int hight = 600;

    SetupWindow(width, hight);

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
