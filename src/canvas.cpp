#include <canvas.h>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <iostream>
#include <assert.h>

using namespace std;

class Canvas::CanvasP {
public:
  CanvasP(Canvas * canvas, int width, int height) 
  {
    assert(CanvasP::canvas == nullptr);

    if (!glfwInit()) {
      cout << "failed to initialize GLFW." << endl;
      exit(0);
    }
    if (!glfwOpenWindow(width, height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)) {
      cout << "failed to open window." << endl;
      glfwTerminate();
      exit(0);
    }
    if (glewInit() != GLEW_OK) {
      cout << "failed to initialize GLEW." << endl;
      exit(0);
    }
    if (!GLEW_VERSION_4_0) {
      cout << "OpenGL 4.0 not supported.\n" << endl;
      exit(0);
    }

    glfwSetMousePosCallback(CanvasP::mouseMoved);
    glfwSetMouseButtonCallback(CanvasP::mouseButton);
    CanvasP::canvas = canvas;
  }

  ~CanvasP() 
  {
    CanvasP::canvas = nullptr;
    glfwTerminate();
  }

private:
  static void mouseMoved(int x, int y)
  {
    if (CanvasP::canvas) {
      CanvasP::canvas->mouseMoved(x, y);
    }
  }

  static void mouseButton(int button, int action)
  {
    if (CanvasP::canvas) {
      CanvasP::canvas->mouseButton(button, action);
    }
  }

  static Canvas * canvas;
};

Canvas * Canvas::CanvasP::canvas = nullptr;

Canvas::Canvas(int width, int height)
{
  self.reset(new CanvasP(this, width, height));
}

Canvas::~Canvas()
{

}

void 
Canvas::loop()
{
  int running = GL_TRUE;
  while (running) {
    this->renderGL();
    if (glGetError() != GL_NO_ERROR) {
      cout << "GL error\n" << endl;
    }
    glfwSwapBuffers();
    running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
  }
}