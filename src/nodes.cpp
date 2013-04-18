#include <nodes.h>
#include <actions.h>
#include <opengl.h>
#include <state.h>
#include <box3.h>
#include <math.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

// *************************************************************************************************

class Camera::CameraP {
public:
  vec3 up, pos, fpt;

  mat4 orientation;
  mat4 translation;

  mat4 viewmatrix;
  mat4 projmatrix;
};

Camera::Camera()
  : self(new CameraP)
{
  self->viewmatrix = mat4(1.0);
  self->projmatrix = mat4(1.0);

  self->up  = vec3(0, 1, 0);
  self->pos = vec3(0, 0, 1);
  self->fpt = vec3(0, 0, 0);
}

Camera::~Camera()
{
}

void 
Camera::traverse(RenderAction * action)
{
  action->state->camera = this;
}

void
Camera::flush(State * state)
{
  self->viewmatrix = glm::transpose(self->orientation) * self->translation;
  GLMatrix4f glviewmatrix("ViewMatrix", self->viewmatrix);
  GLMatrix4f glprojmatrix("ProjectionMatrix", self->projmatrix);
  glviewmatrix.bind();
  glprojmatrix.bind();
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
Camera::viewAll(Separator::ptr root)
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

LUA_NODE_SOURCE(Viewport);

void
Viewport::initClass()
{
  LUA_NODE_INIT_CLASS(Viewport, "Viewport");
}

Viewport::Viewport()
{
  LUA_NODE_ADD_FIELD_3(this->size, "size", ivec2(-1));
  LUA_NODE_ADD_FIELD_3(this->origin, "origin", ivec2(-1));
}

Viewport::~Viewport()
{
}

void
Viewport::traverse(RenderAction * action)
{
  action->state->viewport = this;
}

void
Viewport::flush(State * state)
{
  if (this->size.value == ivec2(-1) || this->origin.value == ivec2(-1)) {
    throw std::invalid_argument("invalid viewport");
  }
  glViewport(this->origin.value.x, this->origin.value.y, this->size.value.x, this->size.value.y);
}

// *************************************************************************************************

LUA_NODE_SOURCE(Group);

void
Group::initClass()
{
  LUA_NODE_INIT_CLASS(Group, "Group");
}

Group::Group() 
{
  LUA_NODE_ADD_FIELD_1(this->children);
}
Group::~Group() {}

// *************************************************************************************************

void
Group::traverse(RenderAction * action)
{
  for each (shared_ptr<Node> node in this->children.values) {
    node->traverse(action);
  }
}

void
Group::traverse(BoundingBoxAction * action)
{
  for each (shared_ptr<Node> node in this->children.values) {
    node->traverse(action);
  }
}

// *************************************************************************************************

LUA_NODE_SOURCE(Separator);

void
Separator::initClass()
{
  LUA_NODE_INIT_CLASS(Separator, "Separator");
}

void
Separator::traverse(RenderAction * action)
{
  State::Scope scope(action->state);
  Group::traverse(action);
}

void
Separator::traverse(BoundingBoxAction * action)
{
  State::Scope scope(action->state);
  Group::traverse(action);
}

// *************************************************************************************************

LUA_NODE_SOURCE(Uniform3f);

void
Uniform3f::initClass()
{
  LUA_NODE_INIT_CLASS(Uniform3f, "Uniform3f");
}

Uniform3f::Uniform3f()
{
  LUA_NODE_ADD_FIELD_3(this->name, "name", "");
  LUA_NODE_ADD_FIELD_3(this->value, "value", vec3(0));
}

Uniform3f::~Uniform3f()
{
}

void
Uniform3f::traverse(RenderAction * action)
{
  action->state->uniformelem.add(this);
}

void
Uniform3f::flush(State * state)
{
  GLVector3f glvector(this->name.value, this->value.value);
  glvector.bind();
}

// *************************************************************************************************

LUA_NODE_SOURCE(UniformMatrix4f);

void
UniformMatrix4f::initClass()
{
  LUA_NODE_INIT_CLASS(UniformMatrix4f, "UniformMatrix4f");
}

UniformMatrix4f::UniformMatrix4f()
{
  LUA_NODE_ADD_FIELD_2(this->name, "name");
  LUA_NODE_ADD_FIELD_3(this->value, "value", mat4(1));
}

UniformMatrix4f::~UniformMatrix4f()
{
}

void
UniformMatrix4f::traverse(RenderAction * action)
{
  action->state->uniformelem.add(this);
}

void
UniformMatrix4f::flush(State * state)
{
  GLMatrix4f matrix(this->name.value, this->value.value);
  matrix.bind();  
}

// *************************************************************************************************

LUA_NODE_SOURCE(ShaderObject);

void
ShaderObject::initClass()
{
  LUA_NODE_INIT_CLASS(ShaderObject, "ShaderObject");
}

ShaderObject::ShaderObject()
{
  LUA_NODE_ADD_FIELD_3(this->type, "type", GL_INVALID_VALUE);
  LUA_ENUM_DEFINE_VALUE(this->type, "VERTEX_SHADER", GL_VERTEX_SHADER);
  LUA_ENUM_DEFINE_VALUE(this->type, "GEOMETRY_SHADER", GL_GEOMETRY_SHADER);
  LUA_ENUM_DEFINE_VALUE(this->type, "FRAGMENT_SHADER", GL_FRAGMENT_SHADER);

  LUA_NODE_ADD_FIELD_2(this->source, "source");
}

ShaderObject::~ShaderObject()
{
}

// *************************************************************************************************

LUA_NODE_SOURCE(Program);

void
Program::initClass()
{
  LUA_NODE_INIT_CLASS(Program, "Program");
}

Program::Program() 
  : glprogram(nullptr)
{
  LUA_NODE_ADD_FIELD_1(this->shaders);
  LUA_NODE_ADD_FIELD_3(this->feedbackVarying, "feedbackVarying", "");
}

Program::~Program() {}

void
Program::traverse(RenderAction * action)
{
  if (this->glprogram.get() == nullptr) {
    this->glprogram.reset(new GLProgram);
    for each (const shared_ptr<ShaderObject> shader in this->shaders.values) {
      this->glprogram->attach(shader->source.value.c_str(), shader->type.value);
    }
    if (!this->feedbackVarying.value.empty()) {
      const char * varyings[] = { this->feedbackVarying.value.c_str() };
      glTransformFeedbackVaryings(this->glprogram->id, 1, varyings, GL_SEPARATE_ATTRIBS);
    }
    this->glprogram->link();
  }
  action->state->program = this;
}

void
Program::flush(State * state)
{
  this->glprogram->bind();
}

// *************************************************************************************************

LUA_NODE_SOURCE(Transform);

void
Transform::initClass()
{
  LUA_NODE_INIT_CLASS(Transform, "Transform");
}

Transform::Transform()
{
  LUA_NODE_ADD_FIELD_3(this->translation, "translation", vec3(0));
  LUA_NODE_ADD_FIELD_3(this->scaleFactor, "scaleFactor", vec3(1));
}

Transform::~Transform() {}

void
Transform::doAction(Action * action)
{
  mat4 matrix(1.0);
  matrix = glm::scale(matrix, this->scaleFactor.value);
  matrix = glm::translate(matrix, this->translation.value);
  action->state->transformelem.mult(matrix);
}

void
Transform::traverse(RenderAction * action)
{
  this->doAction(action);
}

void
Transform::traverse(BoundingBoxAction * action)
{
  this->doAction(action);
}

// *************************************************************************************************

LUA_NODE_SOURCE(Buffer);

void
Buffer::initClass()
{
  LUA_NODE_INIT_CLASS(Buffer, "Buffer");
}

Buffer::Buffer()
  : buffer(nullptr)
{
  LUA_NODE_ADD_FIELD_3(this->type, "type", GL_FLOAT);
  LUA_ENUM_DEFINE_VALUE(this->type, "FLOAT", GL_FLOAT);
  LUA_ENUM_DEFINE_VALUE(this->type, "UNSIGNED_INT", GL_UNSIGNED_INT);

  LUA_NODE_ADD_FIELD_3(this->usage, "usage", GL_STATIC_DRAW);
  LUA_ENUM_DEFINE_VALUE(this->usage, "STATIC_DRAW", GL_STATIC_DRAW);
  LUA_ENUM_DEFINE_VALUE(this->usage, "DYNAMIC_DRAW", GL_DYNAMIC_DRAW);

  LUA_NODE_ADD_FIELD_3(this->target, "target", GL_ARRAY_BUFFER);
  LUA_ENUM_DEFINE_VALUE(this->target, "ARRAY", GL_ARRAY_BUFFER);
  LUA_ENUM_DEFINE_VALUE(this->target, "ELEMENT_ARRAY", GL_ELEMENT_ARRAY_BUFFER);

  LUA_NODE_ADD_FIELD_3(this->count, "count", 0);
  LUA_NODE_ADD_FIELD_2(this->values, "values");
}

Buffer::~Buffer()
{
}

void
Buffer::traverse(RenderAction * action)
{
  if (!this->buffer.get()) {
    this->buffer.reset(GLBufferObject::create(this->target.value, 
                                              this->usage.value, 
                                              this->type.value, 
                                              this->count.value,
                                              this->values.vec));
  }
  action->state->vertexelem.set(this);
}

void
Buffer::traverse(BoundingBoxAction * action)
{
  action->state->vertexelem.set(this);
}

GLuint
Buffer::getCount() const
{
  return (this->count.value > 0) ? this->count.value : this->values.vec.size() / 3;
}

// *************************************************************************************************

class FeedbackBuffer::FeedbackBufferP {
public:
  FeedbackBufferP() 
    : glquery(nullptr), glfeedback(nullptr) {}
  std::unique_ptr<GLQueryObject> glquery;
  std::unique_ptr<GLTransformFeedback> glfeedback;
};

LUA_NODE_SOURCE(FeedbackBuffer);

void
FeedbackBuffer::initClass()
{
  LUA_NODE_INIT_CLASS(FeedbackBuffer, "FeedbackBuffer");
}

FeedbackBuffer::FeedbackBuffer()
  : self(new FeedbackBufferP)
{
  LUA_NODE_ADD_FIELD_3(this->scene, "scene", nullptr);
}

FeedbackBuffer::~FeedbackBuffer()
{
}

void
FeedbackBuffer::traverse(RenderAction * action)
{
  Buffer::traverse(action);

  if (!Innovator::isLodEnabled()) return;

  if (!self->glfeedback.get()) {
    self->glfeedback.reset(new GLTransformFeedback(GL_POINTS, 0, this->buffer->buffer));
  }
  if (!self->glquery.get()) {
    self->glquery.reset(new GLQueryObject(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN));
  }
  State::Scope scope(action->state);
  action->state->feedbackbuffer = this;
  this->scene.value->traverse(action);
}

void
FeedbackBuffer::beginTransformFeedback()
{
  self->glquery->bind();
  self->glfeedback->bind();
}

void
FeedbackBuffer::endTransformFeedback()
{
  self->glquery->unbind();
  self->glfeedback->unbind();
}

GLuint
FeedbackBuffer::getCount() const
{
  return self->glquery->getResult();
}

// *************************************************************************************************

LUA_NODE_SOURCE(VertexAttribute);

void
VertexAttribute::initClass()
{
  LUA_NODE_INIT_CLASS(VertexAttribute, "VertexAttribute");
}

VertexAttribute::VertexAttribute()
  : glattrib(nullptr)
{
  LUA_NODE_ADD_FIELD_3(this->type, "type", GL_FLOAT);
  LUA_NODE_ADD_FIELD_3(this->size, "size", 3);
  LUA_NODE_ADD_FIELD_3(this->index, "location", 0);
  LUA_NODE_ADD_FIELD_3(this->divisor, "divisor", 0);
  LUA_NODE_ADD_FIELD_3(this->buffer, "buffer", nullptr);
}

VertexAttribute::~VertexAttribute() {}

void
VertexAttribute::traverse(RenderAction * action)
{
  if (!this->glattrib.get()) {
    this->glattrib.reset(new GLVertexAttribute(this->index.value, 
                                               this->size.value,
                                               this->type.value,
                                               this->divisor.value));
  }
  if (this->buffer.value.get()) {
    this->buffer.value->traverse(action);
  }
  action->state->vertexelem.set(this);
}

void
VertexAttribute::traverse(BoundingBoxAction * action)
{
  if (this->buffer.value.get()) {
    this->buffer.value->traverse(action);
  }
  action->state->vertexelem.set(this);
}

// *************************************************************************************************

LUA_NODE_SOURCE(TextureUnit);

void
TextureUnit::initClass()
{
  LUA_NODE_INIT_CLASS(TextureUnit, "TextureUnit");
}

TextureUnit::TextureUnit()
{
  LUA_NODE_ADD_FIELD_3(this->unit, "unit", 0);
}

TextureUnit::~TextureUnit()
{

}

void
TextureUnit::traverse(RenderAction * action)
{

}


// *************************************************************************************************

class Texture2D::Texture2DP {
public:
  unique_ptr<GLTextureObject> gltexture;
  unique_ptr<GLTextureSampler> glsampler;
};

LUA_NODE_SOURCE(Texture2D);

void
Texture2D::initClass()
{
  LUA_NODE_INIT_CLASS(Texture2D, "Texture2D");
}

Texture2D::Texture2D()
  : self(nullptr)
{
  LUA_NODE_ADD_FIELD_3(this->width, "width", 0);
  LUA_NODE_ADD_FIELD_3(this->height, "height", 0);

  LUA_NODE_ADD_FIELD_3(this->format, "format", GL_RGBA);
  LUA_ENUM_DEFINE_VALUE(this->format, "RGBA", GL_RGBA);
  LUA_ENUM_DEFINE_VALUE(this->format, "DEPTH_COMPONENT", GL_DEPTH_COMPONENT);

  LUA_NODE_ADD_FIELD_3(this->internalFormat, "internalFormat", GL_RGBA8);
  LUA_ENUM_DEFINE_VALUE(this->internalFormat, "RGBA8", GL_RGBA8);
  LUA_ENUM_DEFINE_VALUE(this->internalFormat, "DEPTH_COMPONENT32", GL_DEPTH_COMPONENT32);
}

Texture2D::~Texture2D()
{

}

void
Texture2D::traverse(RenderAction * action)
{
  if (self.get() == nullptr) {
    self.reset(new Texture2DP);
  }
}

// *************************************************************************************************

LUA_NODE_SOURCE(BoundingBox);

void
BoundingBox::initClass()
{
  LUA_NODE_INIT_CLASS(BoundingBox, "BoundingBox");
}

BoundingBox::BoundingBox()
{
  LUA_NODE_ADD_FIELD_3(this->min, "min", vec3(-1));
  LUA_NODE_ADD_FIELD_3(this->max, "max", vec3( 1));
}

void
BoundingBox::traverse(BoundingBoxAction * action) 
{
  box3 bbox(this->min.value, this->max.value);
  bbox.transform(action->state->transformelem.matrix);
  action->extendBy(bbox);
}

// *************************************************************************************************

Draw::Draw() 
  : vao(nullptr) 
{
  LUA_NODE_ADD_FIELD_3(this->mode, "mode", GL_POINTS);
  LUA_ENUM_DEFINE_VALUE(this->mode, "POINTS", GL_POINTS);
  LUA_ENUM_DEFINE_VALUE(this->mode, "TRIANGLES", GL_TRIANGLES);
  LUA_ENUM_DEFINE_VALUE(this->mode, "TRIANGLE_STRIP", GL_TRIANGLE_STRIP);
}

void
Draw::traverse(RenderAction * action)
{
  if (this->vao.get() == nullptr) {
    this->vao.reset(action->state->vertexelem.createVAO());
  }
  action->state->flush(this);
}

// *************************************************************************************************

LUA_NODE_SOURCE(DrawArrays);

void
DrawArrays::initClass()
{
  LUA_NODE_INIT_CLASS(DrawArrays, "DrawArrays"); 
}

void
DrawArrays::execute(State * state)
{
  if (!state->vertexelem.vertexbuffer) {
    throw std::runtime_error("DrawArrays::execute(): invalid state");
  }
  BindScope vao(this->vao.get());
  GLenum mode = this->mode.value;
  unsigned int count = state->vertexelem.vertexbuffer->getCount();
  glDrawArrays(mode, 0, count);
}

// *************************************************************************************************

LUA_NODE_SOURCE(DrawElements);

void
DrawElements::initClass()
{
  LUA_NODE_INIT_CLASS(DrawElements, "DrawElements"); 
}

void
DrawElements::execute(State * state)
{
  if (!state->vertexelem.elementbuffer) throw std::runtime_error("DrawElements::execute(): invalid state");

  BindScope vao(this->vao.get());
  GLenum mode = this->mode.value;

  unsigned int count = state->vertexelem.elementbuffer->values.vec.size();
  glDrawElements(mode, count, GL_UNSIGNED_INT, nullptr);
}

// *************************************************************************************************

LUA_NODE_SOURCE(DrawArraysInstanced);

void
DrawArraysInstanced::initClass()
{
  LUA_NODE_INIT_CLASS(DrawArraysInstanced, "DrawArraysInstanced"); 
}

void
DrawArraysInstanced::execute(State * state)
{
  Buffer * vertexbuffer = state->vertexelem.vertexbuffer;
  Buffer * instancebuffer = state->vertexelem.instancebuffer;
  if (!instancebuffer || !vertexbuffer) throw std::runtime_error("DrawArraysInstanced::execute(): invalid state");

  BindScope vao(this->vao.get());
  GLenum mode = this->mode.value;

  unsigned int count = vertexbuffer->values.vec.size() / 3;
  unsigned int instancecount = instancebuffer->values.vec.size() / 3; // FIXME
  if (instancecount > 0) {
    glDrawArraysInstanced(mode, 0, count, instancecount);
  }
}

// *************************************************************************************************

LUA_NODE_SOURCE(DrawElementsInstanced);

void
DrawElementsInstanced::initClass()
{
  LUA_NODE_INIT_CLASS(DrawElementsInstanced, "DrawElementsInstanced"); 
}

void
DrawElementsInstanced::execute(State * state)
{
  Buffer * elementbuffer = state->vertexelem.elementbuffer;
  Buffer * instancebuffer = state->vertexelem.instancebuffer;
  if (!elementbuffer || !instancebuffer) throw std::runtime_error("DrawElementsInstanced::execute(): invalid state");

  BindScope vao(this->vao.get());
  GLenum mode = this->mode.value;

  unsigned int elemcount = elementbuffer->values.vec.size();
  unsigned int instancecount = instancebuffer->getCount();
  if (instancecount > 0) {
    glDrawElementsInstanced(mode, elemcount, GL_UNSIGNED_INT, 0, instancecount);
  }
}
