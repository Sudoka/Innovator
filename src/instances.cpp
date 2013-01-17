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
  InstancesP(Instances * self) {
    bufferobject.createBuffer(self->vertices, 0, 0);
    bufferobject.createBuffer(self->instances, 1, 1);
    //bufferobject.createBuffer(2, self->instances.size());
    program.createProgram("../../src/instances.glsl");
  }
  ~InstancesP() {
    bufferobject.deleteBuffers();
    program.deleteProgram();
  }
  ShaderProgram program;
  VertexBufferObject bufferobject;
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

  action->state->programelem.program = self->program;
  action->state->flush();

  self->bufferobject.bind();
  glDrawElementsInstanced(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, this->indices.data(), this->instances.size());
  self->bufferobject.unbind();
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