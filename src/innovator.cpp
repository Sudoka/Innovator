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

Innovator::Innovator(int width, int height)
  : initialized(false)
{
  if (!glfwInit()) {
    cout << "failed to initialize GLFW." << endl;
    return;
  }
  if (!glfwOpenWindow(width, height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)) {
    cout << "failed to open window." << endl;
    return;
  }
  glfwDisable(GLFW_AUTO_POLL_EVENTS);
  glfwSetWindowSizeCallback(resizeCB);
  glfwSetMousePosCallback(mouseMovedCB);
  glfwSetMouseButtonCallback(mouseButtonCB);

  if (glewInit() != GLEW_OK) {
    cout << "failed to initialize GLEW." << endl;
    return;
  }
  if (!GLEW_VERSION_4_2) {
    cout << "OpenGL 4.2 not supported.\n" << endl;
    return;
  }

  Lua::init();
  File::init();

  viewer.reset(new Viewer(width, height));
  viewer->setSceneGraph(File::readAll("../../src/scene.lua"));

  this->initialized = true;
}

Innovator::~Innovator()
{
  viewer.reset(nullptr);
  glfwTerminate();
  Lua::exit();
  File::exit();
}

void 
Innovator::loop()
{
  if (!this->initialized) return;

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
  