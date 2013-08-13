#include <state.h>
#include <vector>
#include <iostream>
#include <nodes.h>
#include <opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class State::StateP {
public:
  vector<DrawElement> drawstack;
  vector<VertexElement> vertexstack;
  vector<ProgramElement> programstack;
  vector<ViewMatrixElement> viewmatstack;
  vector<TransformElement> transformstack;
  vector<ProjectionMatrixElement> projmatstack;
};

State::State()
  : self(new StateP)
{
}

State::~State() 
{
}

void
State::push()
{
  self->drawstack.push_back(this->drawelem);
  self->vertexstack.push_back(this->vertexelem);
  self->viewmatstack.push_back(this->viewmatelem);
  self->programstack.push_back(this->programelem);
  self->projmatstack.push_back(this->projmatelem);
  self->transformstack.push_back(this->transformelem);
}

void
State::pop()
{
  this->drawelem = self->drawstack.back();
  this->vertexelem = self->vertexstack.back();
  this->programelem = self->programstack.back();
  this->viewmatelem = self->viewmatstack.back();
  this->projmatelem = self->projmatstack.back();
  this->transformelem = self->transformstack.back();

  self->drawstack.pop_back();
  self->vertexstack.pop_back();
  self->programstack.pop_back();
  self->viewmatstack.pop_back();
  self->projmatstack.pop_back();
  self->transformstack.pop_back();
}

function<void()> 
State::capture()
{
  DrawCall * draw = this->drawelem.drawcall;
  GLint viewmatloc = glGetUniformLocation(this->programelem.program, "ViewMatrix");
  GLint modelmatloc = glGetUniformLocation(this->programelem.program, "ModelMatrix");
  GLint projmatloc = glGetUniformLocation(this->programelem.program, "ProjectionMatrix");

  return [=]() {
    glUseProgram(this->programelem.program);
    glUniformMatrix4fv(viewmatloc, 1, GL_FALSE, glm::value_ptr(this->viewmatelem.matrix));
    glUniformMatrix4fv(projmatloc, 1, GL_FALSE, glm::value_ptr(this->projmatelem.matrix));
    glUniformMatrix4fv(modelmatloc, 1, GL_FALSE, glm::value_ptr(this->transformelem.matrix));
    draw->execute();
  };
}
