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
  vector<TextureElement> texturestack;
  vector<TransformElement> transformstack;
  vector<TransformFeedbackElement> feedbackstack;
};

State::State()
  : self(new StateP), 
    camera(nullptr),
    program(nullptr),
    viewport(nullptr)
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
  self->texturestack.push_back(this->textureelem);
  self->feedbackstack.push_back(this->feedbackelem);
  self->transformstack.push_back(this->transformelem);
}

void
State::pop()
{
  this->program = self->programstack.back();
  this->viewport = self->viewportstack.back();
  this->vertexelem = self->vertexstack.back();
  this->textureelem = self->texturestack.back();
  this->feedbackelem = self->feedbackstack.back();
  this->transformelem = self->transformstack.back();

  self->vertexstack.pop_back();
  self->programstack.pop_back();
  self->texturestack.pop_back();
  self->viewportstack.pop_back();
  self->feedbackstack.pop_back();
  self->transformstack.pop_back();
}

void
State::flush(Shape * shape)
{
  this->program->flush(this);
  this->camera->flush(this);
  this->viewport->flush(this);
  this->textureelem.flush(this);
  this->transformelem.flush(this);

  this->feedbackelem.begin();
  shape->render(this);
  this->feedbackelem.end();
}
