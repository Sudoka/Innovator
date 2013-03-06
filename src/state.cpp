#include <state.h>
#include <vector>
#include <iostream>
#include <nodes.h>
#include <opengl.h>

using namespace std;

class State::StateP {
public:
  StateP() {}
  ~StateP() {}

  vector<GLProgram*> programstack;
  vector<VertexElement> vertexstack;
  vector<MatrixElement> modelmatrixstack;
  vector<ViewportElement> viewportstack;
};

State::State()
  : self(new StateP), program(nullptr)
{
  this->viewmatrixelem.name = "ViewMatrix";
  this->modelmatrixelem.name = "ModelMatrix";
  this->projmatrixelem.name = "ProjectionMatrix";
  this->program = nullptr;
}

State::~State() {}

void
State::push()
{
  self->programstack.push_back(this->program);
  self->vertexstack.push_back(this->vertexelem);
  self->viewportstack.push_back(this->viewportelem);
  self->modelmatrixstack.push_back(this->modelmatrixelem);
}

void
State::pop()
{
  this->program = self->programstack.back();
  this->vertexelem = self->vertexstack.back();
  this->viewportelem = self->viewportstack.back();
  this->modelmatrixelem = self->modelmatrixstack.back();

  self->programstack.pop_back();
  self->viewportstack.pop_back();
  self->vertexstack.pop_back();
  self->modelmatrixstack.pop_back();
}

void
State::flush(Shape * shape)
{
  BindScope program(this->program);
  BindScope vertices(&this->vertexelem);

  this->viewportelem.updateGL(this);
  this->viewmatrixelem.updateGL(this);
  this->projmatrixelem.updateGL(this);
  this->modelmatrixelem.updateGL(this);
  
  shape->draw(this);
}
