#include <actions.h>
#include <state.h>
#include <GL/glew.h>
#include <opengl.h>
#include <iostream>

using namespace std;

Action::Action()
  : state(nullptr) {}
Action::~Action() {}

class RenderAction::RenderActionP {
};

RenderAction::RenderAction()
  : Action(), self(new RenderActionP)
{}

RenderAction::~RenderAction() {}

void
RenderAction::apply(Node::ptr node)
{
  glEnable(GL_DEPTH_TEST);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  state.reset(new State);
  node->traverse(this);
  glFinish();
}

BoundingBoxAction::BoundingBoxAction()
  : Action() {}

BoundingBoxAction::~BoundingBoxAction() {}

void
BoundingBoxAction::extendBy(const box3 & box)
{
  this->box.extendBy(box.min);
  this->box.extendBy(box.max);
}

void
BoundingBoxAction::apply(Node::ptr node)
{
  state.reset(new State);
  this->box.makeEmpty();
  node->traverse(this);
}

const box3 & 
BoundingBoxAction::getBoundingBox() const
{
  return this->box;
}
