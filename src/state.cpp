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
  vector<Uniform3fElement> uniform3fstack;
};

State::State()
  : self(new StateP), 
    query(nullptr),
    program(nullptr),
    feedback(nullptr)
{
  this->viewmatrixelem.name = "ViewMatrix";
  this->modelmatrixelem.name = "ModelMatrix";
  this->projmatrixelem.name = "ProjectionMatrix";
}

State::~State() {}

void
State::push()
{
  self->programstack.push_back(this->program);
  self->vertexstack.push_back(this->vertexelem);
  self->viewportstack.push_back(this->viewportelem);
  self->uniform3fstack.push_back(this->uniform3felem);
  self->modelmatrixstack.push_back(this->modelmatrixelem);
}

void
State::pop()
{
  this->program = self->programstack.back();
  this->vertexelem = self->vertexstack.back();
  this->viewportelem = self->viewportstack.back();
  this->uniform3felem = self->uniform3fstack.back();
  this->modelmatrixelem = self->modelmatrixstack.back();

  self->vertexstack.pop_back();
  self->programstack.pop_back();
  self->viewportstack.pop_back();
  self->uniform3fstack.pop_back();
  self->modelmatrixstack.pop_back();
}

void
State::flush(Draw * draw)
{
  BindScope program(this->program);
  this->viewportelem.updateGL(this);
  this->uniform3felem.updateGL(this);
  this->viewmatrixelem.updateGL(this);
  this->projmatrixelem.updateGL(this);
  this->modelmatrixelem.updateGL(this);
  BindScope feedback(this->feedback);
  BindScope query(this->query);
  draw->execute(this);
}
