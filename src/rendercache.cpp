#include <rendercache.h>
#include <state.h>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class RenderCache::Pimpl {
public:
  Pimpl() : shapecount(0), draw_instanced(false) {}
  vector<Shape*> shapes;
  vector<Program*> programs;
  vector<Material*> materials;

  std::unique_ptr<GLUniformBuffer> glmaterial;
  std::unique_ptr<GLUniformBuffer> gltransform;

  vector<GLProgram*> glprograms;
  vector<GLBufferObject*> indexbuffers;
  vector<GLBufferObject*> vertexbuffers;
  vector<GLVertexAttribute*> vertexattribs;
  vector<GLVertexArrayObject*> vertexarrayobjects;

  vector<ShapeInfo> shapeinfovec;
  int shapecount;
  bool draw_instanced;
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
  //if (std::find(begin(self->materials), end(self->materials), state->material) == end(self->materials)) {
    self->materials.push_back(state->material);
    // }

  ShapeInfo shapeinfo;
  shapeinfo.transform = state->transform;
  shapeinfo.shapeindex = self->shapes.size() - 1;
  shapeinfo.programindex = self->programs.size() - 1;
  shapeinfo.materialindex = self->materials.size() - 1;

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

    GLuint transformindex = glGetUniformBlockIndex(glprogram->id, "Transform");
    if (transformindex != GL_INVALID_VALUE) {
      glUniformBlockBinding(glprogram->id, transformindex, 0);
    }
    GLuint materialindex = glGetUniformBlockIndex(glprogram->id, "Material");
    if (materialindex != GL_INVALID_VALUE) {
      glUniformBlockBinding(glprogram->id, materialindex, 1);
    }
  }

  self->draw_instanced = (self->shapes.size() == 1 && self->programs.size() == 1);

  if (self->draw_instanced) {
    self->gltransform.reset(new GLUniformBuffer(0, sizeof(mat4) * (self->shapecount + 2)));
    for (int i = 0; i < self->shapecount; i++) {
      ShapeInfo shapeinfo = self->shapeinfovec[i];
      self->gltransform->updateGL(glm::value_ptr(shapeinfo.transform), sizeof(mat4), i + 2);
    }

    self->glmaterial.reset(new GLUniformBuffer(1, sizeof(GLMaterial) * 8));

    for (size_t i = 0; i < self->materials.size(); i++) {
      ShapeInfo shapeinfo = self->shapeinfovec[i];
      Material * mat = self->materials[shapeinfo.materialindex];
      GLMaterial glmat;
      glmat.ambient = vec4(mat->ambient.value, 1.0);
      glmat.diffuse = vec4(mat->diffuse.value, mat->transparency.value);
      glmat.specular = vec4(mat->specular.value, 1.0);
      
      self->glmaterial->updateGL(&glmat, sizeof(GLMaterial), i);
    }
  } else {
    self->gltransform.reset(new GLUniformBuffer(0, sizeof(mat4) * 3));
    self->glmaterial.reset(new GLUniformBuffer(1, sizeof(GLMaterial)));
  }
}

void 
RenderCache::flush(State * state)
{
  self->gltransform->updateGL(glm::value_ptr(state->viewmatrix), sizeof(mat4), 0);
  self->gltransform->updateGL(glm::value_ptr(state->projmatrix), sizeof(mat4), 1);

  if (self->draw_instanced) {

    BindScope programscope(self->glprograms[0]);
    self->glmaterial->bindBuffer();
    self->gltransform->bindBuffer();
    BindScope vaoscope(self->vertexarrayobjects[0]);
    
    glDrawElementsInstanced(GL_TRIANGLES, self->shapes[0]->indices.vec.size(), GL_UNSIGNED_INT, nullptr, self->shapecount);
  } else {

    for (int i = 0; i < self->shapecount; i++) {
      ShapeInfo shapeinfo = self->shapeinfovec[i];

      Shape * shape = self->shapes[shapeinfo.shapeindex];
      GLProgram * program = self->glprograms[shapeinfo.programindex];
      GLVertexArrayObject * vao = self->vertexarrayobjects[shapeinfo.shapeindex];
      
      BindScope programscope(program);
      self->glmaterial->bindBuffer();
      self->gltransform->bindBuffer();
      BindScope vaoscope(vao);

      Material * mat = self->materials[shapeinfo.materialindex];
      GLMaterial glmat;
      glmat.ambient = vec4(mat->ambient.value, 1.0);
      glmat.diffuse = vec4(mat->diffuse.value, mat->transparency.value);
      glmat.specular = vec4(mat->specular.value, 1.0);
      
      self->glmaterial->updateGL(&glmat, sizeof(GLMaterial), 0);
      self->gltransform->updateGL(glm::value_ptr(shapeinfo.transform), sizeof(mat4), 2);
      glDrawElements(GL_TRIANGLES, shape->indices.vec.size(), GL_UNSIGNED_INT, nullptr);
    }
  }
}