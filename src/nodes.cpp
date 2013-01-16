#include <nodes.h>

#include <actions.h>
#include <state.h>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;

class Camera::CameraP {
public:
  vec3 up, pos, fpt;
  mat4 viewmatrix;
  mat4 projmatrix;
  mat4 orientation;
  mat4 translation;
};

Camera::Camera()
  : self(new CameraP)
{
  self->up  = vec3(0, 1, 0);
  self->pos = vec3(0, 0, 1);
  self->fpt = vec3(0, 0, 0);
}

Camera::~Camera()
{
  
}

void 
Camera::renderGL(RenderAction * action)
{
  self->viewmatrix = glm::transpose(self->orientation) * self->translation;
  action->state->viewmatrixelem.matrix = self->viewmatrix;
  action->state->projmatrixelem.matrix = self->projmatrix;
}

vec3
Camera::getFocalDir() const
{
  return self->pos - self->fpt;
}

void
Camera::lookAt(const vec3 & focalpoint)
{
  self->fpt = focalpoint;
  self->orientation = glm::lookAt(self->pos, self->fpt, self->up);
}

void
Camera::moveTo(const vec3 & position)
{
  self->pos = position;
  self->translation = glm::translate(mat4(1.0), -position);
}

void 
Camera::zoom(float dz)
{

}

void
Camera::pan(const vec2 & dx)
{

}

void
Camera::orbit(const vec2 & dx)
{
  self->orientation = glm::rotate(self->orientation, dx.y, vec3(1, 0, 0));
  self->orientation = glm::rotate(self->orientation, dx.x, vec3(0, 1, 0));
  float z = glm::length(this->getFocalDir());
  vec3 newdir = vec3(self->orientation * vec4(0, 0, z, 0));
  this->moveTo(self->fpt + newdir);
}

void 
Camera::perspective(float fovy, float aspect, float near, float far)
{
  self->projmatrix = glm::perspective(fovy, aspect, near, far);
}

class Group::GroupP {
public:
  vector<shared_ptr<Node>> children;
};

Group::Group() : self(new GroupP) {}
Group::~Group() {}

void
Group::renderGL(RenderAction * action)
{
  for (size_t i = 0; i < self->children.size(); i++) {
    self->children[i]->renderGL(action);
  }
}

void 
Group::addChild(std::shared_ptr<Node> child)
{
  self->children.push_back(child);
}

void
Separator::renderGL(RenderAction * action)
{
  action->state->push();
  Group::renderGL(action);
  action->state->pop();
}

class Shader::ShaderP {
public:
  ShaderP(const string & filename) { this->program.createProgram(filename); }
  ~ShaderP() { this->program.deleteProgram(); }
  ShaderProgram program;
};

Shader::Shader() : self(nullptr) {}
Shader::~Shader() {}

void
Shader::renderGL(RenderAction * action)
{
  if (self.get() == nullptr) {
    self.reset(new ShaderP(this->fileName));
  }
  action->state->programelem.program = self->program;
}

Transform::Transform()
  : translation(0, 0, 0),
    scaleFactor(1, 1, 1) 
{}

Transform::~Transform() {}

void
Transform::renderGL(RenderAction * action)
{
  mat4 & matrix = action->state->modelmatrixelem.matrix;
  matrix = glm::scale(matrix, this->scaleFactor);
  matrix = glm::translate(matrix, this->translation);
}

class Triangles::TrianglesP {
public:
  TrianglesP(Triangles * self) { vertexbuffer.createBuffer(self->vertices, State::VertexPosition); }
  ~TrianglesP() { vertexbuffer.deleteBuffer(); }
  ArrayBuffer vertexbuffer;
};

Triangles::Triangles() : self(nullptr) {}
Triangles::~Triangles() {}

void
Triangles::renderGL(RenderAction * action)
{
  if (self.get() == nullptr)
    self.reset(new TrianglesP(this));

  action->state->buffer = self->vertexbuffer;
  action->state->flush();
}