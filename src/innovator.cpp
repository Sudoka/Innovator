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
  bool lod_enabled;
};

std::unique_ptr<Innovator::InnovatorP> Innovator::self = nullptr;

Innovator::Innovator(int width, int height, const std::string & filename)
{
  if (self.get() != nullptr) {
    throw std::runtime_error("Innovator already created.");
  }
  self.reset(new InnovatorP);
  self->lod_enabled = true;
  self->viewer.reset(new Viewer(width, height));

  self->lua.reset(new Lua);
  self->glfw.reset(new Glfw);

  //glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
  //glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);

  if (glfwOpenWindow(width, height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW) != GL_TRUE) {
    throw std::runtime_error("failed to open GLFW window.");
  }
  glewExperimental = true; // necessary for core profile
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

  self->lua->registerFunction("Group", Node::CreateInstance<Group>);
  self->lua->registerFunction("Buffer", Node::CreateInstance<Buffer>);
  self->lua->registerFunction("Texture", Node::CreateInstance<Texture>);
  self->lua->registerFunction("Program", Node::CreateInstance<Program>);
  self->lua->registerFunction("Material", Node::CreateInstance<Material>);
  self->lua->registerFunction("Uniform3f", Node::CreateInstance<Uniform3f>);
  self->lua->registerFunction("Separator", Node::CreateInstance<Separator>);
  self->lua->registerFunction("Transform", Node::CreateInstance<Transform>);
  self->lua->registerFunction("DrawArrays", Node::CreateInstance<DrawArrays>);
  self->lua->registerFunction("TextureUnit", Node::CreateInstance<TextureUnit>);
  self->lua->registerFunction("BoundingBox", Node::CreateInstance<BoundingBox>);
  self->lua->registerFunction("DrawElements", Node::CreateInstance<DrawElements>);
  self->lua->registerFunction("ShaderObject", Node::CreateInstance<ShaderObject>);
  self->lua->registerFunction("TextureSampler", Node::CreateInstance<TextureSampler>);
  self->lua->registerFunction("VertexAttribute", Node::CreateInstance<VertexAttribute>);
  self->lua->registerFunction("UniformMatrix4f", Node::CreateInstance<UniformMatrix4f>);

  self->lua->dofile(filename);
  shared_ptr<Separator> root(static_cast<Separator*>(self->lua->getglobaluserdata("SceneRoot")));
  self->viewer->setSceneGraph(root);
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

    if (glfwGetKey(GLFW_KEY_SPACE)) {
      self->lod_enabled = !self->lod_enabled;
      self->viewer->scheduleRedraw();
    }

    if (self->viewer->needRedraw()) {
      self->viewer->renderGL();
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

bool 
Innovator::isLodEnabled()
{
  return self->lod_enabled;
}