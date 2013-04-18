#include <state.h>
#include <vector>
#include <iostream>
#include <nodes.h>
#include <opengl.h>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class State::StateP {
public:
  StateP() {}
  ~StateP() {}
  vector<Program*> programstack;
  vector<Viewport*> viewportstack;
  vector<VertexElement> vertexstack;
  vector<UniformElement> uniformstack;
  vector<FeedbackBuffer*> feedbackstack;
  vector<TransformElement> transformstack;
};

State::State()
  : self(new StateP), 
    camera(nullptr),
    program(nullptr),
    viewport(nullptr),
    feedbackbuffer(nullptr)
{
}

State::~State() 
{
}

void
State::push()
{
  self->programstack.push_back(this->program);
  self->viewportstack.push_back(this->viewport);
  self->vertexstack.push_back(this->vertexelem);
  self->uniformstack.push_back(this->uniformelem);
  self->feedbackstack.push_back(this->feedbackbuffer);
  self->transformstack.push_back(this->transformelem);
}

void
State::pop()
{
  this->program = self->programstack.back();
  this->viewport = self->viewportstack.back();
  this->vertexelem = self->vertexstack.back();
  this->uniformelem = self->uniformstack.back();
  this->transformelem = self->transformstack.back();
  this->feedbackbuffer = self->feedbackstack.back();

  self->vertexstack.pop_back();
  self->programstack.pop_back();
  self->uniformstack.pop_back();
  self->viewportstack.pop_back();
  self->feedbackstack.pop_back();
  self->transformstack.pop_back();
}

void
State::flush(Draw * draw)
{
  this->program->flush(this);
  this->camera->flush(this);
  this->viewport->flush(this);
  this->uniformelem.flush(this);
  this->transformelem.flush(this);
  FeedbackBuffer::Scope feedback_scope(this->feedbackbuffer);
  draw->execute(this);
}
