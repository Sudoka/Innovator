#include <nodes.h>
#include <opengl.h>
#include <state.h>
#include <actions.h>
#include <elements.h>
#include <iostream>

using namespace glm;
using namespace std;

class Instances::InstancesP {
public:
  InstancesP(Instances * self) 
    : vertexbuffer(new VertexBuffer(self->vertices, 0, 0)),
      instancebuffer(new VertexBuffer(self->instances, 1, 1)),
      program(new ShaderProgram("../../src/instances.glsl"))
  {
  }

  ~InstancesP() {}
  unique_ptr<ShaderProgram> program;
  unique_ptr<VertexBuffer> vertexbuffer;
  unique_ptr<VertexBuffer> instancebuffer;
};

Instances::Instances()
  : self(nullptr)
{

}

Instances::~Instances()
{

}

void
Instances::renderGL(RenderAction * action)
{
  if (self.get() == nullptr)
    self.reset(new InstancesP(this));

  action->state->programelem.program = *self->program;
  action->state->flush();

  {
    VertexBufferScope bufferscope(self->vertexbuffer.get());
    VertexBufferScope instancescope(self->instancebuffer.get());
    glDrawElementsInstanced(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, this->indices.data(), this->instances.size());
  }
}

void
Instances::getBoundingBox(BoundingBoxAction * action)
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