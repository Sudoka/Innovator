#include <viewer.h>
#include <GL/glfw.h>

#include <nodes.h>
#include <actions.h>

#include <glm/glm.hpp>
#include <iostream>

using namespace glm;
using namespace std;

class PostProcess {
public:
  PostProcess(GLint width, GLint height)
  {
    this->program.reset(new GLProgram);
    this->program->attach("#version 330\n" 
                          "layout(location = 0) in vec2 Position;\n"
                          "out vec2 TexCoord;\n"
                          "void main() {\n"
                          "  TexCoord = Position * 0.5 + 0.5;\n"
                          "  gl_Position = vec4(Position.x, Position.y, 0.5, 1.0);\n" 
                          "}\n",
                          GL_VERTEX_SHADER);

    this->program->attach("#version 330\n"
                          "layout(location = 0) out vec4 FragColor;\n"
                          "in vec2 TexCoord;\n"
                          "uniform sampler2D FrameBuffer;\n"
                          "void main() {\n"
                          "  FragColor = texture(FrameBuffer, TexCoord);\n"
                          "}\n", GL_FRAGMENT_SHADER);

    this->program->link();

    this->color_texture.reset(new GLTextureObject(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
    this->depth_texture.reset(new GLTextureObject(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));

    this->color_sampler.reset(new GLTextureSampler(0));
    this->color_sampler->parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    this->color_sampler->parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    this->color_sampler->parameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    this->color_sampler->parameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    this->framebuffer.reset(new GLFramebufferObject);
    BindScope framebuffer(this->framebuffer.get());
    this->framebuffer->attach(GL_COLOR_ATTACHMENT0, this->color_texture->id);
    this->framebuffer->attach(GL_DEPTH_ATTACHMENT, this->depth_texture->id);
    this->framebuffer->checkStatus();
  }

  void bindFramebuffer()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer->id);
  }

  void unbindFramebuffer()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void execute()
  {
    static GLfloat vertices[] = { -1, -1, -1, 1, 1, -1, 1, 1 };
    
    glDisable(GL_DEPTH_TEST);
    BindScope program(this->program.get());
    BindScope sampler(this->color_sampler.get());
    glActiveTexture(GL_TEXTURE0);
    BindScope texture(this->color_texture.get());
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }

  unique_ptr<GLProgram> program;
  unique_ptr<GLTextureObject> color_texture;
  unique_ptr<GLTextureObject> depth_texture;
  unique_ptr<GLTextureSampler> color_sampler;
  unique_ptr<GLFramebufferObject> framebuffer;
};

class Viewer::ViewerP {
public:
  ViewerP(int width, int height) 
    : prev_x(0),
      prev_y(0),
      button(0),
      redraw(true),
      mousedown(false),
      camera(new Camera),
      viewport(new Viewport),
      renderaction(new RenderAction),
      postprocess(nullptr)
  {}
  ~ViewerP() {}

  float prev_x;
  float prev_y;
  int button;
  bool redraw;
  bool mousedown;
  Separator::ptr root;
  Camera::ptr camera;
  Viewport::ptr viewport;
  unique_ptr<RenderAction> renderaction;
  unique_ptr<PostProcess> postprocess;
};

Viewer::Viewer(int width, int height)
  : self(new ViewerP(width, height))
{
  this->resize(width, height);
}

Viewer::~Viewer()
{
}

void
Viewer::scheduleRedraw()
{
  self->redraw = true;
}

bool
Viewer::needRedraw() const
{
  return self->redraw;
}

void
Viewer::resize(int width, int height)
{
  self->camera->perspective(45, float(width) / float(height), 0.1f, 10000);
  self->viewport->origin.value = ivec2(0);
  self->viewport->size.value = ivec2(width, height);
}

void
Viewer::renderGL()
{
  if (!self->postprocess.get()) {
    self->postprocess.reset(new PostProcess(640, 480));
  }
  self->postprocess->bindFramebuffer();
  self->renderaction->apply(self->root);
  self->postprocess->unbindFramebuffer();
  self->postprocess->execute();
  self->redraw = false;
}

void
Viewer::setSceneGraph(Separator::ptr root)
{
  self->root.reset(new Separator);
  self->root->children.values.push_back(self->camera);
  self->root->children.values.push_back(self->viewport);
  self->root->children.values.push_back(root);
  self->camera->viewAll(root);
}

void 
Viewer::mouseMoved(int x, int y)
{
  if (self->mousedown) {
    float dx = (self->prev_x - x) / 100.0f;
    float dy = (self->prev_y - y) / 100.0f;

    switch (self->button) {
    case GLFW_MOUSE_BUTTON_LEFT:
      self->camera->orbit(vec2(dx * 10.0, dy * 10.0));
      break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
      self->camera->pan(vec2(dx, -dy));
      break;
    case GLFW_MOUSE_BUTTON_RIGHT:
      self->camera->zoom(-dy);
      break;
    }
    self->redraw = true;
  }
  self->prev_x = (float)x;
  self->prev_y = (float)y;
}

void
Viewer::mouseButton(int button, int action)
{
  self->button = button;
  self->mousedown = action == GLFW_PRESS;
}

void
Viewer::keyboard(int button)
{

}
