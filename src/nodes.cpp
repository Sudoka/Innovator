#include <nodes.h>

#include <actions.h>
#include <state.h>
#include <box3.h>
#include <vector>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;

// *************************************************************************************************

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

  vec3 z = glm::normalize(this->getFocalDir());
  vec3 x = glm::normalize(glm::cross(self->up, z));
  vec3 y = glm::normalize(glm::cross(z, x));

  self->orientation[0] = vec4(x, 0.0);
  self->orientation[1] = vec4(y, 0.0);
  self->orientation[2] = vec4(z, 0.0);
}

void
Camera::viewAll(std::shared_ptr<Node> root)
{
  BoundingBoxAction action;
  action.apply(root);
  box3 box = action.getBoundingBox();
  float focaldist = box.size();
  vec3 focalpoint = box.center();
  vec3 focaldir = glm::normalize(this->getFocalDir()) * focaldist;

  this->moveTo(focalpoint + focaldir);
  this->lookAt(focalpoint);
}

void
Camera::moveTo(const vec3 & position)
{
  self->pos = position;
  self->translation = glm::translate(mat4(1.0), -position);
}

void 
Camera::zoom(float dy)
{
  vec3 direction = -this->getFocalDir();
  float oldfocaldist = length(direction);
  float newfocaldist = oldfocaldist * exp(dy);
  vec3 dpos = normalize(direction) * (newfocaldist - oldfocaldist);
  this->moveTo(self->pos + dpos);
}

void
Camera::pan(const vec2 & dx)
{
  vec3 x = vec3(self->orientation * vec4(1, 0, 0, 0));
  vec3 y = vec3(self->orientation * vec4(0, 1, 0, 0));
  vec3 dpos = x * dx.x + y * dx.y;
  this->moveTo(self->pos + dpos);
  self->fpt += dpos;
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

// *************************************************************************************************

class Group::GroupP {
public:
  vector<shared_ptr<Node>> children;
};

Group::Group() : self(new GroupP) {}
Group::~Group() {}

// *************************************************************************************************

void
Group::renderGL(RenderAction * action)
{
  for (size_t i = 0; i < self->children.size(); i++) {
    self->children[i]->renderGL(action);
  }
}

void
Group::getBoundingBox(BoundingBoxAction * action)
{
  for (size_t i = 0; i < self->children.size(); i++) {
    self->children[i]->getBoundingBox(action);
  }
}

// *************************************************************************************************

void 
Group::addChild(std::shared_ptr<Node> child)
{
  self->children.push_back(child);
}

// *************************************************************************************************

Separator::Separator() {}
Separator::~Separator() {}

void
Separator::renderGL(RenderAction * action)
{
  action->state->push();
  Group::renderGL(action);
  action->state->pop();
}

void
Separator::getBoundingBox(BoundingBoxAction * action)
{
  action->state->push();
  Group::getBoundingBox(action);
  action->state->pop();
}

// *************************************************************************************************

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

// *************************************************************************************************

Transform::Transform()
  : translation(0, 0, 0),
    scaleFactor(1, 1, 1) 
{}

Transform::~Transform() {}

void
Transform::doAction(Action * action)
{
  mat4 & matrix = action->state->modelmatrixelem.matrix;
  matrix = glm::scale(matrix, this->scaleFactor);
  matrix = glm::translate(matrix, this->translation);
}

void
Transform::renderGL(RenderAction * action)
{
  this->doAction(action);
}

void
Transform::getBoundingBox(BoundingBoxAction * action)
{
  this->doAction(action);
}

// *************************************************************************************************

class Triangles::TrianglesP {
public:
  TrianglesP(Triangles * self) { 
    bufferobject.createBuffer(self->vertices, State::VertexPosition);
  }
  ~TrianglesP() { 
    bufferobject.deleteBuffers();
  }
  VertexBufferObject bufferobject;
};

Triangles::Triangles() : self(nullptr) {}
Triangles::~Triangles() {}

void
Triangles::renderGL(RenderAction * action)
{
  if (self.get() == nullptr)
    self.reset(new TrianglesP(this));

  action->state->flush();

  self->bufferobject.bind();
  glDrawArrays(GL_TRIANGLES, 0, 3);
  self->bufferobject.unbind();
}

void
Triangles::getBoundingBox(BoundingBoxAction * action)
{
  box3 bbox;
  for (size_t i = 0; i < this->vertices.size(); i++) {
    bbox.extendBy(this->vertices[i]);
  }
  bbox.transform(action->state->modelmatrixelem.matrix);
  action->extendBy(bbox);
}