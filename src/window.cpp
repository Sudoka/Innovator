#include <window.h>
#include <GLFW/glfw3.h>

class Window::Pimpl {
public:
  GLFWwindow * window;
};

Window::Window(int width, int height, const std::string & title)
  : self(new Pimpl)
{
/*
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
*/
  self->window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (self->window == nullptr) {
    throw std::runtime_error("failed to open GLFW window.");
  }
}

Window::~Window()
{
  glfwDestroyWindow(self->window);
}

void
Window::render()
{
  glfwMakeContextCurrent(self->window);
  this->renderScene();
  glfwSwapBuffers(self->window);
}
