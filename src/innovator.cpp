#include <innovator.h>
#include <viewer.h>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <luawrapper.h>
#include <iostream>
#include <nodes.h>

using namespace std;

class Glfw {
public:
  Glfw() {
    if (!glfwInit()) {
      throw std::runtime_error("failed to initialize GLFW.");
    }
  }
  ~Glfw() {
    cout << "glfwTerminate()" << endl;
    glfwTerminate();
  }
};

class Innovator::InnovatorP {
public:
  unique_ptr<Lua> lua;
  unique_ptr<Glfw> glfw;
  unique_ptr<Viewer> viewer;
};

std::unique_ptr<Innovator::InnovatorP> Innovator::self = nullptr;

Innovator::Innovator(int width, int height)
{
  if (self.get() != nullptr) {
    throw std::runtime_error("Innovator already created.");
  }
  self.reset(new InnovatorP);
  self->viewer.reset(new Viewer(width, height));

  self->lua.reset(new Lua);
  self->glfw.reset(new Glfw);

  if (!glfwOpenWindow(width, height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)) {
    throw std::runtime_error("failed to open GLFW window.");
  }
  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("failed to initialize GLEW.");
  }
  if (!GLEW_VERSION_3_3) {
    throw std::runtime_error("OpenGL 3.3 not supported.");
  }
  glfwDisable(GLFW_AUTO_POLL_EVENTS);
  glfwSetWindowSizeCallback(Innovator::resizeCB);
  glfwSetMousePosCallback(Innovator::mouseMovedCB);
  glfwSetMouseButtonCallback(Innovator::mouseButtonCB);

  IntBuffer::initClass();
  Vec3Buffer::initClass();
  DrawArrays::initClass();
  DrawArraysInstanced::initClass();
  DrawElements::initClass();
  DrawElementsInstanced::initClass();
  Program::initClass();
  Group::initClass();
  Separator::initClass();
  Transform::initClass();
  VertexAttribute::initClass();
  VertexShader::initClass();
  GeometryShader::initClass();
  FragmentShader::initClass();

  self->viewer->setSceneGraph(self->lua->readAll("../../src/scene.lua"));
}

Innovator::~Innovator()
{
}

void 
Innovator::resizeCB(int width, int height)
{
  self->viewer->resize(width, height);
  self->viewer->scheduleRedraw();
}

void
Innovator::mouseMovedCB(int x, int y)
{
  self->viewer->mouseMoved(x, y);
}

void
Innovator::mouseButtonCB(int button, int action)
{
  self->viewer->mouseButton(button, action);
}

void 
Innovator::loop()
{
  while (true) {
    glfwWaitEvents();
    if (glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED))
      break;

    if (self->viewer->needRedraw()) {
      self->viewer->renderGL();
      if (glGetError() != GL_NO_ERROR) {
        cout << "GL error\n" << endl;
      }
      glfwSwapBuffers();
    }
  }
}

void
Innovator::postError(const string & msg)
{
  cout << "Innovator error: " << msg << endl;
}

Lua *
Innovator::lua()
{
  return self->lua.get();
}  
