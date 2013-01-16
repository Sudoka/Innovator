#include <state.h>
#include <vector>

using namespace std;

class State::StateP {
public:
  StateP() {}
  ~StateP() {}

  vector<ProgramElement> programstack;
  vector<MatrixElement> modelmatrixstack;
};

State::State()
  : self(new StateP)
{
  this->viewmatrixelem.name = "ViewMatrix";
  this->modelmatrixelem.name = "ModelMatrix";
  this->projmatrixelem.name = "ProjectionMatrix";
}

State::~State() {}

void
State::push()
{
  self->programstack.push_back(this->programelem);
  self->modelmatrixstack.push_back(this->modelmatrixelem);
}

void
State::pop()
{
  this->programelem = self->programstack.back();
  this->modelmatrixelem = self->modelmatrixstack.back();
  self->programstack.pop_back();
  self->modelmatrixstack.pop_back();
}

void
State::flush()
{
  this->programelem.updateGL(this);
  this->viewmatrixelem.updateGL(this);
  this->projmatrixelem.updateGL(this);
  this->modelmatrixelem.updateGL(this);

  ArrayBuffer::Scope bufferscope(this->buffer);
  bufferscope.draw();
}
