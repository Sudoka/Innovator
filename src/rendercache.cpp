#include <rendercache.h>
#include <state.h>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

CachedShape::CachedShape(State * state)
{
  this->shape = state->shape;
  this->program = state->program;
  this->material = state->material;
  this->transform = state->transform;

  this->indexbuffer.reset(GLBufferObject::create(GL_ELEMENT_ARRAY_BUFFER,
                                                 GL_STATIC_DRAW,
                                                 GL_UNSIGNED_INT,
                                                 0,
                                                 this->shape->indices.vec));
  this->vertexbuffer.reset(GLBufferObject::create(GL_ARRAY_BUFFER,
                                                  GL_STATIC_DRAW,
                                                  GL_FLOAT,
                                                  0,
                                                  this->shape->vertices.vec));

  this->glprogram.reset(new GLProgram(this->program->shaders.values));
  this->vertexattrib.reset(new GLVertexAttribute(0, 3, GL_FLOAT, 0));
  this->vertexarrayobject.reset(new GLVertexArrayObject);
  this->gltransform.reset(new GLUniformBuffer(1));
  this->glcamera.reset(new GLUniformBuffer(0, 2));
  
  this->vertexarrayobject->bind();
  BindScope indexbuffer(this->indexbuffer.get());
  BindScope vertexbuffer(this->vertexbuffer.get());
  BindScope vertexattrib(this->vertexattrib.get());
  this->vertexarrayobject->unbind();
}

void 
CachedShape::flush(State * state)
{
  this->gltransform->updateGL(glm::value_ptr(this->transform), sizeof(mat4));
  this->glcamera->updateGL(glm::value_ptr(state->viewmatrix), sizeof(mat4), 0);
  this->glcamera->updateGL(glm::value_ptr(state->projmatrix), sizeof(mat4), 1);
  
  BindScope program(this->glprogram.get());
  this->glcamera->bindBuffer();
  this->gltransform->bindBuffer();
  BindScope vao(this->vertexarrayobject.get());
  glDrawElements(GL_TRIANGLES, this->shape->indices.vec.size(), GL_UNSIGNED_INT, nullptr);
}

class RenderCache::Pimpl {
public:
  vector<CachedShape*> shapes;
};

RenderCache::RenderCache()
  : self(new Pimpl)
{
}

RenderCache::~RenderCache()
{
}

void 
RenderCache::capture(State * state)
{
  self->shapes.push_back(new CachedShape(state));
}

void
RenderCache::compile()
{
}

void 
RenderCache::flush(State * state)
{
  for each (CachedShape * shape in self->shapes) {
    shape->flush(state);
  }
}