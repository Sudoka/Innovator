#include <nodes.h>
#include <mesh.h>
#include <opengl.h>
#include <state.h>
#include <actions.h>
#include <elements.h>
#include <iostream>

using namespace glm;
using namespace std;

#undef DEBUG_CULLING

class InstancedTriangleSet::InstancedTriangleSetP {
public:
  InstancedTriangleSetP(InstancedTriangleSet * self) 
    : self(self),
      drawprogram(new Shader),
      cullprogram(new Shader),
      normalprogram(new Shader),
      drawinstances(new VertexBuffer(self->instances, 1, 0)),
      cullinstances(new VertexBuffer(self->instances, 2, 1)),
      indirectbuffer(new BufferObject(GL_DRAW_INDIRECT_BUFFER)),
      transformfeedback(new TransformFeedback(cullinstances->buffer))
  {
    drawprogram->fileName = "drawprogram.glsl";
    normalprogram->fileName = "normalprogram.glsl";
    cullprogram->fileName = "cullprogram.glsl";
    cullprogram->transformFeedbackVaryings.push_back("PositionOut");

    Mesh mesh(self);
    mesh.subdivide(1);
    mesh.normalize();
    mesh.generateNormals();
    
    this->elements.reset(new BufferObject(self->indices));
    this->vertices.reset(new VertexBuffer(self->vertices, 0, 0));
    this->normals.reset(new VertexBuffer(self->normals, 1, 0));

    DrawElementsIndirectBuffer buffer(self->indices.size() * 3);
    this->indirectbuffer->setValues(GL_DYNAMIC_DRAW, sizeof(buffer), &buffer);
  }

  ~InstancedTriangleSetP() 
  {
  }

  void cull(RenderAction * action) 
  {
    this->cullprogram->renderGL(action);
    action->state->flush();

    // clear instance count
    this->indirectbuffer->set1Value(1, 0);
    // bind instance count to atomic counter in cullprogram
    BindBufferRange bufferrange(GL_ATOMIC_COUNTER_BUFFER, 0, this->indirectbuffer->buffer, sizeof(GLuint), sizeof(GLuint));
    BindScope instances(this->drawinstances.get());
    BindScope transformfeedback(this->transformfeedback.get());

    glDrawArrays(GL_POINTS, 0, self->instances.size());
  }

  void draw(RenderAction * action) 
  {
    this->drawprogram->renderGL(action);
    action->state->flush();

    BindScope vertices(this->vertices.get());
    BindScope normals(this->normals.get());
    BindScope elements(this->elements.get());
    BindScope indirect(this->indirectbuffer.get());
    BindScope instances(this->cullinstances.get());

    glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr);

#ifdef DEBUG_CULLING
    GLuint instancecount;
    glGetBufferSubData(GL_DRAW_INDIRECT_BUFFER, 4, sizeof(GLuint), &instancecount);
    cout << "instancecount: " << instancecount << endl;
#endif
  }

  void drawNormals(RenderAction * action)
  {
    this->normalprogram->renderGL(action);
    action->state->flush();

    BindScope vertices(this->vertices.get());
    BindScope normals(this->normals.get());
    
    glDrawArrays(GL_POINTS, 0, self->vertices.size());
  }

  unique_ptr<Shader> drawprogram;
  unique_ptr<Shader> cullprogram;
  unique_ptr<Shader> normalprogram;

  unique_ptr<BufferObject> elements;
  unique_ptr<VertexBuffer> vertices;
  unique_ptr<VertexBuffer> normals;

  unique_ptr<VertexBuffer> drawinstances;
  unique_ptr<VertexBuffer> cullinstances;
  unique_ptr<BufferObject> indirectbuffer;

  unique_ptr<TransformFeedback> transformfeedback;
  InstancedTriangleSet * self;
};

InstancedTriangleSet::InstancedTriangleSet()
  : self(nullptr)
{

}

InstancedTriangleSet::~InstancedTriangleSet()
{

}

void
InstancedTriangleSet::renderGL(RenderAction * action)
{
  if (self.get() == nullptr) {
    self.reset(new InstancedTriangleSetP(this));
  }
  self->cull(action);
  self->draw(action);
  self->drawNormals(action);
}

void
InstancedTriangleSet::getBoundingBox(BoundingBoxAction * action)
{
  box3 bbox;
  for (size_t i = 0; i < this->vertices.size(); i++) {
    bbox.extendBy(this->vertices[i]);
  }

  if (this->instances.size() > 0) {
    vec3 min = bbox.min;
    vec3 max = bbox.max;
    bbox.makeEmpty();
    for (size_t i = 0; i < this->instances.size(); i++) {
      bbox.extendBy(this->instances[i] + max);
      bbox.extendBy(this->instances[i] + min);
    }
  }

  bbox.transform(action->state->modelmatrixelem.matrix);
  action->extendBy(bbox);
}