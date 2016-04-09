/* OpenGL dev - code
 *
 * Author: KienLTb
 * build command
 *    g++ -o 05_model  main.cpp Program.cpp Shader.cpp Bitmap.cpp platform_linux.cpp Texture.cpp Camera.cpp -lGL -lglfw -lGLEW -DGLM_FORCE_RADIANS
 *
 */

#include "platform.hpp"

// third-party libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// standard C++ libraries
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <list>

// tdogl classes
#include "Program.h"
#include "Texture.h"
#include "Camera.h"

// Data struct
struct ModelAsset {
    tdogl::Program* shaders;
    tdogl::Texture* texture;
    GLuint vbo;
    GLuint vao;
    GLenum drawType;
    GLint  drawStart;
    GLint  drawCount;

    ModelAsset() :
        shaders(NULL),
        texture(NULL),
        vbo(0),
        vao(0),
        drawType(GL_TRIANGLES),
        drawStart(0),
        drawCount(0)
    {}
};

struct ModelInstance {
    ModelAsset* asset;
    glm::mat4 transform;

    ModelInstance() :
        asset(NULL),
        transform()
    {}
};

// constants
const glm::vec2 SCREEN_SIZE(800, 600);

// globals
GLFWwindow* gWindow = NULL;

ModelAsset gWoodenCrate;
std::list<ModelInstance> gInstances;

GLfloat gDegreesRotated = 0.0f;

tdogl::Camera gCamera;
double gScrollY = 0.0;

// loads the vertex shader and fragment shader, and links them to make the global gProgram
static tdogl::Program* LoadShaders(std::string vertex_shader, std::string fragment_shader) {
    std::vector<tdogl::Shader> shaders;
    shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath(vertex_shader), GL_VERTEX_SHADER));
    shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath(fragment_shader), GL_FRAGMENT_SHADER));
    return new tdogl::Program(shaders);
}

// loads the file "wooden-crate.jpg" into gTexture
static tdogl::Texture* LoadTexture(std::string texture_file) {
    tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile(ResourcePath("wooden-crate.jpg"));
    bmp.flipVertically();
    return new tdogl::Texture(bmp);
}

static void LoadWoodenCrateAsset() {
    gWoodenCrate.shaders = LoadShaders("vertex-shader.txt", "fragment-shader.txt");
    gWoodenCrate.drawType = GL_TRIANGLES;
    gWoodenCrate.drawStart = 0;
    gWoodenCrate.drawCount = 6 * 2 * 3;
    gWoodenCrate.texture = LoadTexture("wooden-crate.jpg");
    glGenBuffers(1, &gWoodenCrate.vbo);
    glGenVertexArrays(1, &gWoodenCrate.vao);

    // bind the VAO
    glBindVertexArray(gWoodenCrate.vao);

    // bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate.vbo);

    // Make a cube out of triangles (two triangles per side)
    GLfloat vertexData[] = {
        //  X     Y     Z       U     V
        // bottom
        -1.0f, -1.0f, -1.0f,   0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,   1.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,   0.0f, 1.0f,
        1.0f, -1.0f, -1.0f,   1.0f, 0.0f,
        1.0f, -1.0f, 1.0f,   1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,   0.0f, 1.0f,

        // top
        -1.0f, 1.0f, -1.0f,   0.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
        1.0f, 1.0f, -1.0f,   1.0f, 0.0f,
        1.0f, 1.0f, -1.0f,   1.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
        1.0f, 1.0f, 1.0f,   1.0f, 1.0f,

        // front
        -1.0f, -1.0f, 1.0f,   1.0f, 0.0f,
        1.0f, -1.0f, 1.0f,   0.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,   0.0f, 0.0f,
        1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,

        // back
        -1.0f, -1.0f, -1.0f,   0.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,   0.0f, 1.0f,
        1.0f, -1.0f, -1.0f,   1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,   1.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,   0.0f, 1.0f,
        1.0f, 1.0f, -1.0f,   1.0f, 1.0f,

        // left
        -1.0f, -1.0f, 1.0f,   0.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,   1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,   0.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,   0.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,   1.0f, 0.0f,

        // right
        1.0f, -1.0f, 1.0f,   1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,   1.0f, 0.0f,
        1.0f, 1.0f, -1.0f,   0.0f, 0.0f,
        1.0f, -1.0f, 1.0f,   1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,   0.0f, 0.0f,
        1.0f, 1.0f, 1.0f,   0.0f, 1.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    // connect the xyz to the "vert" attribute of the vertex shader
    glEnableVertexAttribArray(gWoodenCrate.shaders->attrib("vert"));
    glVertexAttribPointer(gWoodenCrate.shaders->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);

    // connect the uv coords to the "vertTexCoord" attribute of the vertex shader
    glEnableVertexAttribArray(gWoodenCrate.shaders->attrib("vertTexCoord"));
    glVertexAttribPointer(gWoodenCrate.shaders->attrib("vertTexCoord"), 2, GL_FLOAT, GL_TRUE,  5 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

    // unbind the VAO
    glBindVertexArray(0);

}

// convenience function that returns a translation matrix
glm::mat4 translate(GLfloat x, GLfloat y, GLfloat z) {
    return glm::translate(glm::mat4(), glm::vec3(x, y, z));
}

// convenience function that returns a scaling matrix
glm::mat4 scale(GLfloat x, GLfloat y, GLfloat z) {
    return glm::scale(glm::mat4(), glm::vec3(x, y, z));
}


// Create WoodenCrate from 6 Instance
static void CreateInstances() {
    ModelInstance dot;
    dot.asset = &gWoodenCrate;
    dot.transform = glm::mat4();
    gInstances.push_back(dot);

    ModelInstance i;
    i.asset = &gWoodenCrate;
    i.transform = translate(0, -4, 0) * scale(1, 2, 1);
    gInstances.push_back(i);

    ModelInstance hLeft;
    hLeft.asset = &gWoodenCrate;
    hLeft.transform = translate(-8, 0, 0) * scale(1, 6, 1);
    gInstances.push_back(hLeft);

    ModelInstance hRight;
    hRight.asset = &gWoodenCrate;
    hRight.transform = translate(-4, 0, 0) * scale(1, 6, 1);
    gInstances.push_back(hRight);

    ModelInstance hMid;
    hMid.asset = &gWoodenCrate;
    hMid.transform = translate(-6, 0, 0) * scale(2, 1, 0.8f);
    gInstances.push_back(hMid);
}

static void RenderInstance(const ModelInstance& inst) {
    ModelAsset* asset = inst.asset;
    tdogl::Program* shaders = asset->shaders;

    // bind the shaders
    shaders->use();

    // set the shader uniforms
    shaders->setUniform("camera", gCamera.matrix());
    shaders->setUniform("model", inst.transform);
    shaders->setUniform("tex", 0);

    // bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, asset->texture->object());

    // bind VAO and draw
    glBindVertexArray(asset->vao);
    glDrawArrays(asset->drawType, asset->drawStart, asset->drawCount);

    // unbind everything
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    shaders->stopUsing();
}

// draws a single frame
static void Render() {
    // clear everything
    glClearColor(0, 0, 0, 1); // black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::list<ModelInstance>::const_iterator item;
    for ( item = gInstances.begin(); item != gInstances.end(); ++item)
        RenderInstance(*item);
    // swap the display buffers (displays what was just drawn)
    glfwSwapBuffers(gWindow);
}

// update the scene based on the time elapsed since last update
void Update(float secondsElapsed) {
    const GLfloat degreesPerSecond = 180.0f;
    gDegreesRotated += secondsElapsed * degreesPerSecond;
    while (gDegreesRotated > 360.0f) gDegreesRotated -= 360.0f;
    gInstances.front().transform = glm::rotate(glm::mat4(), gDegreesRotated, glm::vec3(0, 1, 0));

    // Move position of camera base on WASD keys
    const float moveSpeed = 2.0; // Units per second;
    if (glfwGetKey(gWindow, 'S')) {
        gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.forward());
    } else if (glfwGetKey(gWindow, 'W')) {
        gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.forward());
    }

    if (glfwGetKey(gWindow, 'A')) {
        gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.right());
    } else if (glfwGetKey(gWindow, 'D')) {
        gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.right());
    }

    if (glfwGetKey(gWindow, 'Z')) {
        gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.up());
    } else if (glfwGetKey(gWindow, 'X')) {
        gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.up());
    }

    //rotate camera based on mouse movement
    const float mouseSensitivity = 0.1f;
    double mouseX, mouseY;
    glfwGetCursorPos(gWindow, &mouseX, &mouseY);
    gCamera.offsetOrientation(mouseSensitivity * (float)mouseY, mouseSensitivity * (float)mouseX);
    glfwSetCursorPos(gWindow, 0, 0); //reset the mouse, so it doesn't go out of the window

    //increase or decrease field of view based on mouse wheel
    const float zoomSensitivity = -0.2f;
    float fieldOfView = gCamera.fieldOfView() + zoomSensitivity * (float)gScrollY;
    if (fieldOfView < 5.0f) fieldOfView = 5.0f;
    if (fieldOfView > 130.0f) fieldOfView = 130.0f;
    gCamera.setFieldOfView(fieldOfView);
    gScrollY = 0;
}

// records how far the y axis has been scrolled
void OnScroll(GLFWwindow* window, double deltaX, double deltaY) {
    gScrollY += deltaY;
}

void OnError(int errorCode, const char* msg) {
    throw std::runtime_error(msg);
}

// the program starts here
void AppMain() {
    // initialise GLFW
    glfwSetErrorCallback(OnError);
    if (!glfwInit())
        throw std::runtime_error("glfwInit failed");

    // open a window with GLFW
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    gWindow = glfwCreateWindow((int)SCREEN_SIZE.x, (int)SCREEN_SIZE.y, "OpenGL Tutorial", NULL, NULL);
    if (!gWindow)
        throw std::runtime_error("glfwCreateWindow failed. Can your hardware handle OpenGL 3.2?");

    // GLFW settings
    glfwMakeContextCurrent(gWindow);
    glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(gWindow, 0, 0);
    glfwSetScrollCallback(gWindow, OnScroll);
    glfwMakeContextCurrent(gWindow);

    // initialise GLEW
    glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
    if (glewInit() != GLEW_OK)
        throw std::runtime_error("glewInit failed");

    // GLEW throws some errors, so discard all the errors so far
    while (glGetError() != GL_NO_ERROR) {}

    // print out some info about the graphics drivers
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    // make sure OpenGL version 3.2 API is available
    if (!GLEW_VERSION_3_2)
        throw std::runtime_error("OpenGL 3.2 API is not available.");

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialise the gWoodenCrate asset
    LoadWoodenCrateAsset();

    // Create all instance in 3D scene base on the gWoodenCrate asset
    CreateInstances();

    // Init camera
    gCamera.setPosition(glm::vec3(-4, 0, 17));
    gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);

    // run while the window is open
    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(gWindow)) {
        // process pending events
        glfwPollEvents();

        // update the scene based on the time elapsed since last update
        double thisTime = glfwGetTime();
        Update((float)(thisTime - lastTime));
        lastTime = thisTime;

        // draw one frame
        Render();

        // check for errors
        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
            std::cerr << "OpenGL Error " << error << std::endl;

        if (glfwGetKey(gWindow, GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(gWindow, GL_TRUE);
    }

    // clean up and exit
    glfwTerminate();
}


int main(int argc, char *argv[]) {
    try {
        AppMain();
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
