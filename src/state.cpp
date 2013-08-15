#include <state.h>
#include <vector>
#include <nodes.h>

using namespace std;
using namespace glm;

class State::StateP {
public:
  vector<mat4> transformstack;
  vector<GLProgram*> programstack;
  vector<GLMaterial*> materialstack;
  vector<VertexElement> vertexstack;
};

State::State()
  : self(new StateP),
    program(nullptr),
    material(nullptr)
{
}

State::~State() 
{
}

void
State::push()
{
  self->programstack.push_back(this->program);
  self->vertexstack.push_back(this->vertexelem);
  self->materialstack.push_back(this->material);
  self->transformstack.push_back(this->transform);
}

void
State::pop()
{
  this->program = self->programstack.back();
  this->material = self->materialstack.back();
  this->vertexelem = self->vertexstack.back();
  this->transform = self->transformstack.back();

  self->vertexstack.pop_back();
  self->programstack.pop_back();
  self->materialstack.pop_back();
  self->transformstack.pop_back();
}