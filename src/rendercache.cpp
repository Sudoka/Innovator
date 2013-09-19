#include <rendercache.h>
#include <state.h>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class RenderCache::Pimpl {
public:
  Pimpl() : shapecount(0) {}
  vector<Shape*> shapes;
  vector<Program*> programs;

  std::unique_ptr<GLUniformBuffer> glcamera;
  std::unique_ptr<GLUniformBuffer> gltransform;

  vector<GLProgram*> glprograms;
  vector<GLBufferObject*> indexbuffers;
  vector<GLBufferObject*> vertexbuffers;
  vector<GLVertexAttribute*> vertexattribs;
  vector<GLVertexArrayObject*> vertexarrayobjects;

  vector<ShapeInfo> shapeinfovec;
  int shapecount;
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
  if (std::find(begin(self->shapes), end(self->shapes), state->shape) == end(self->shapes)) {
    self->shapes.push_back(state->shape);
  }
  if (std::find(begin(self->programs), end(self->programs), state->program) == end(self->programs)) {
    self->programs.push_back(state->program);
  }

  ShapeInfo shapeinfo;
  shapeinfo.transform = state->transform;
  shapeinfo.shapeindex = self->shapes.size() - 1;
  shapeinfo.programindex = self->programs.size() - 1;

  self->shapeinfovec.push_back(shapeinfo);
  self->shapecount++;
}

void
RenderCache::compile()
{
  for each (Shape * shape in self->shapes) {
    GLBufferObject * indexbuffer = GLBufferObject::create(GL_ELEMENT_ARRAY_BUFFER,
                                                          GL_STATIC_DRAW,
                                                          GL_UNSIGNED_INT,
                                                          0,
                                                          shape->indices.vec);
    GLBufferObject * vertexbuffer = GLBufferObject::create(GL_ARRAY_BUFFER,
                                                           GL_STATIC_DRAW,
                                                           GL_FLOAT,
                                                           0,
                                                           shape->vertices.vec);
    GLVertexAttribute * vertexattrib = new GLVertexAttribute(0, 3, GL_FLOAT, 0);

    GLVertexArrayObject * vertexarrayobject = new GLVertexArrayObject;

    vertexarrayobject->bind();
    BindScope indexbufferscope(indexbuffer);
    BindScope vertexbufferscope(vertexbuffer);
    BindScope vertexattribscope(vertexattrib);
    vertexarrayobject->unbind();

    self->indexbuffers.push_back(indexbuffer);
    self->vertexbuffers.push_back(vertexbuffer);
    self->vertexattribs.push_back(vertexattrib);
    self->vertexarrayobjects.push_back(vertexarrayobject);
  }

  for each (Program * program in self->programs) {
    GLProgram * glprogram = new GLProgram(program->shaders.values);
    self->glprograms.push_back(glprogram);

    GLuint cameraindex = glGetUniformBlockIndex(glprogram->id, "Camera");
    if (cameraindex != GL_INVALID_VALUE) {
      glUniformBlockBinding(glprogram->id, cameraindex, 0);
    }
    GLuint transformindex = glGetUniformBlockIndex(glprogram->id, "Transform");
    if (transformindex != GL_INVALID_VALUE) {
      glUniformBlockBinding(glprogram->id, transformindex, 1);
    }
  }

  self->glcamera.reset(new GLUniformBuffer(0, 2));
  self->gltransform.reset(new GLUniformBuffer(1));
}

void 
RenderCache::flush(State * state)
{
  self->glcamera->updateGL(glm::value_ptr(state->viewmatrix), sizeof(mat4), 0);
  self->glcamera->updateGL(glm::value_ptr(state->projmatrix), sizeof(mat4), 1);

  Shape * shape = self->shapes[0];
  GLProgram * program = self->glprograms[0];
  GLVertexArrayObject * vao = self->vertexarrayobjects[0];
  
  BindScope programscope(program);
  self->glcamera->bindBuffer();
  self->gltransform->bindBuffer();
  BindScope vaoscope(vao);

  for (int i = 0; i < self->shapecount; i++) {
    ShapeInfo shapeinfo = self->shapeinfovec[i];
    self->gltransform->updateGL(glm::value_ptr(shapeinfo.transform), sizeof(mat4));
    glDrawElements(GL_TRIANGLES, shape->indices.vec.size(), GL_UNSIGNED_INT, nullptr);
  }
}