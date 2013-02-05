#include <canvas.h>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <iostream>
#include <assert.h>

using namespace std;

static Canvas * canvas = nullptr;

static void mouseMovedCB(int x, int y)
{
  if (canvas) {
    canvas->mouseMoved(x, y);
  }
}

static void mouseButtonCB(int button, int action)
{
  if (canvas) {
    canvas->mouseButton(button, action);
  }
}

Canvas::Canvas(int width, int height)
{
  if (!glfwInit()) {
    cout << "failed to initialize GLFW." << endl;
  }
  if (!glfwOpenWindow(width, height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)) {
    cout << "failed to open window." << endl;
    glfwTerminate();
  }
  if (glewInit() != GLEW_OK) {
    cout << "failed to initialize GLEW." << endl;
  }
  if (!GLEW_VERSION_4_2) {
    cout << "OpenGL 4.2 not supported.\n" << endl;
  }

  glfwSetMousePosCallback(mouseMovedCB);
  glfwSetMouseButtonCallback(mouseButtonCB);

  assert(canvas == nullptr);
  canvas = this;
}

Canvas::~Canvas()
{
  canvas = nullptr;
  glfwTerminate();
}

void 
Canvas::loop()
{
  int running = 1;
  while (running) {
    this->renderGL();
    if (glGetError() != GL_NO_ERROR) {
      cout << "GL error\n" << endl;
    }
    glfwSwapBuffers();
    running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
  }
}