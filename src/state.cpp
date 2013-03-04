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

  vector<ShaderProgram*> programstack;
  vector<MatrixElement> modelmatrixstack;
  vector<ViewportElement> viewportstack;
  vector<AttributeElement> attributestack;
};

State::State()
  : self(new StateP)
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
  self->attributestack.push_back(this->attribelem);
  self->viewportstack.push_back(this->viewportelem);
  self->modelmatrixstack.push_back(this->modelmatrixelem);
}

void
State::pop()
{
  this->program = self->programstack.back();
  this->attribelem = self->attributestack.back();
  this->viewportelem = self->viewportstack.back();
  this->modelmatrixelem = self->modelmatrixstack.back();

  self->programstack.pop_back();
  self->viewportstack.pop_back();
  self->attributestack.pop_back();
  self->modelmatrixstack.pop_back();
}

void
State::flush(Draw * draw)
{
  BindScope program(this->program);
  BindScope attributes(&this->attribelem);

  this->viewportelem.updateGL(this);
  this->viewmatrixelem.updateGL(this);
  this->projmatrixelem.updateGL(this);
  this->modelmatrixelem.updateGL(this);

  draw->execute(this);
}
