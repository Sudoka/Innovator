#include <innovator.h>
#include <viewer.h>
#include <luawrapper.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <map>
#include <string>
#include <iostream>

using namespace std;

static map<GLFWwindow *, shared_ptr<Viewer>> windows;

static void ErrorCallback(int error, const char * description)
{
  cout << string(description) << endl;
}

static void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
}

static void CursorPosCallback(GLFWwindow * window, double x, double y)
{
  windows[window]->mouseMoved((int)x, (int)y);
}

static void MouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
  windows[window]->mouseButton(button, action);
}

void
Innovator::init()
{
  if (!glfwInit()) {
    throw std::runtime_error("failed to initialize GLFW.");
  }
  glfwSetErrorCallback(ErrorCallback);
}

void
Innovator::CreateWindow(int width, int height, const std::string & title)
{
  GLFWwindow * shared = nullptr;
  GLFWmonitor * monitor = nullptr;

  GLFWwindow * window = glfwCreateWindow(width, height, title.c_str(), monitor, shared);
  if (window == nullptr) {
    throw std::runtime_error("failed to open GLFW window.");
  }

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, KeyCallback);
  glfwSetCursorPosCallback(window, CursorPosCallback);
  glfwSetMouseButtonCallback(window, MouseButtonCallback);

  // TODO: multi-context glew
  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("failed to initialize GLEW.");
  }

  Lua lua;
  lua.dofile("test2.lua");
  shared_ptr<Separator> root(static_cast<Separator*>(lua.getglobaluserdata("SceneRoot")));
  
  windows[window] = make_shared<Viewer>(width, height);
  windows[window]->setSceneGraph(root);
}

void
Innovator::loop()
{
  while (true) {

    if (windows.empty()) return;
    glfwWaitEvents();

    // remove closed windows
    for (auto it = windows.begin(); it != windows.end();) {
      glfwMakeContextCurrent(it->first);

      if (glfwWindowShouldClose(it->first)) {
        glfwDestroyWindow(it->first);
        windows.erase(it++);
      } else {
        it->second->render();
        glfwSwapBuffers(it->first);
        glfwPollEvents();
        it++;
      }
    }
  }
}

void
Innovator::exit()
{
  glfwTerminate();
}