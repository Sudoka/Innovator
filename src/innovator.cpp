#include <innovator.h>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <viewer.h>
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

Innovator::Innovator(int width, int height, const std::string & filename)
{
  if (self.get() != nullptr) {
    throw std::runtime_error("Innovator already created.");
  }
  self.reset(new InnovatorP);
  self->viewer.reset(new Viewer(width, height));

  self->lua.reset(new Lua);
  self->glfw.reset(new Glfw);

  glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 4);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);

  if (glfwOpenWindow(width, height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW) != GL_TRUE) {
    throw std::runtime_error("failed to open GLFW window.");
  }
  glewExperimental = true; // necessary for core profile
  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("failed to initialize GLEW.");
  }
  if (!GLEW_VERSION_4_3) {
    throw std::runtime_error("OpenGL 4.3 not supported.");
  }
  //glfwDisable(GLFW_AUTO_POLL_EVENTS);
  glfwSetWindowSizeCallback(Innovator::resizeCB);
  glfwSetMousePosCallback(Innovator::mouseMovedCB);
  glfwSetMouseButtonCallback(Innovator::mouseButtonCB);

  Group::initClass();
  Buffer::initClass();
  Program::initClass();
  Separator::initClass();
  Uniform3f::initClass();
  Transform::initClass();
  BoundingBox::initClass();
  VertexShader::initClass();
  GeometryShader::initClass();
  FragmentShader::initClass();
  FeedbackBuffer::initClass();
  VertexAttribute::initClass();
  DrawArrays::initClass();
  DrawElements::initClass();
  DrawArraysInstanced::initClass();
  DrawElementsInstanced::initClass();

  self->viewer->setSceneGraph(self->lua->readAll(filename));
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
    //glfwWaitEvents();
    if (glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED))
      break;

    if (self->viewer->needRedraw()) {
      self->viewer->renderGL();
      self->viewer->scheduleRedraw();
      if (glGetError() != GL_NO_ERROR) {
        cout << "GL error" << endl;
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
