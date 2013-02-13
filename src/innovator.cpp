#include <innovator.h>
#include <file.h>
#include <viewer.h>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <luawrapper.h>
#include <iostream>
#include <nodes.h>
#include <assert.h>

using namespace std;
static unique_ptr<Viewer> viewer = nullptr;

static void resizeCB(int width, int height)
{
  if (viewer) {
    viewer->resize(width, height);
    viewer->scheduleRedraw();
  }
}

static void mouseMovedCB(int x, int y)
{
  if (viewer) {
    viewer->mouseMoved(x, y);
  }
}

static void mouseButtonCB(int button, int action)
{
  if (viewer) {
    viewer->mouseButton(button, action);
  }
}

class glfwScope {
public:
  glfwScope() {
    if (!glfwInit()) {
      throw std::runtime_error("failed to initialize GLFW.");
    }
  }
  ~glfwScope() {
    glfwTerminate();
  }
};

template<typename T>
class InitScope {
public:
  InitScope() { T::init(); }
  ~InitScope() { T::exit(); }
};

class Innovator::InnovatorP {
public:
  InnovatorP()
    : lua(new InitScope<Lua>),
      file(new InitScope<File>),
      glfw(new glfwScope)
  {}
  ~InnovatorP() {}

  unique_ptr<InitScope<Lua>> lua;
  unique_ptr<InitScope<File>> file;
  unique_ptr<glfwScope> glfw;
};

Innovator::Innovator(int width, int height)
  : self(new InnovatorP)
{
  if (!glfwOpenWindow(width, height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)) {
    throw std::runtime_error("failed to open GLFW window.");
  }
  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("failed to initialize GLEW.");
  }
  if (!GLEW_VERSION_4_2) {
    throw std::runtime_error("OpenGL 4.2 not supported.");
  }

  glfwDisable(GLFW_AUTO_POLL_EVENTS);
  glfwSetWindowSizeCallback(resizeCB);
  glfwSetMousePosCallback(mouseMovedCB);
  glfwSetMouseButtonCallback(mouseButtonCB);

  viewer.reset(new Viewer(width, height));
  viewer->setSceneGraph(File::readAll("../../src/scene.lua"));
}

Innovator::~Innovator()
{
  viewer.reset(nullptr);
}

void 
Innovator::loop()
{
  while (true) {
    glfwWaitEvents();
    if (glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED))
      break;

    if (viewer->needRedraw()) {
      viewer->renderGL();
      if (glGetError() != GL_NO_ERROR) {
        cout << "GL error\n" << endl;
      }
      glfwSwapBuffers();
    }
  }
}
  