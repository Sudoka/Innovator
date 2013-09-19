#include <state.h>
#include <vector>
#include <nodes.h>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class State::StateP {
public:
  vector<mat4> transformstack;
  vector<Program*> programstack;
  vector<Material*> materialstack;
};

State::State()
  : self(new StateP),
    shape(nullptr),
    program(nullptr),
    material(nullptr),
    rendercache(nullptr)
{
}

State::~State() 
{
}

void
State::push()
{
  self->programstack.push_back(this->program);
  self->materialstack.push_back(this->material);
  self->transformstack.push_back(this->transform);
}

void
State::pop()
{
  this->program = self->programstack.back();
  this->material = self->materialstack.back();
  this->transform = self->transformstack.back();

  self->programstack.pop_back();
  self->materialstack.pop_back();
  self->transformstack.pop_back();
}