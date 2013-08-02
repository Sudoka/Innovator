#include <actions.h>
#include <state.h>
#include <GL/glew.h>
#include <opengl.h>
#include <iostream>

using namespace std;

Action::Action()
  : state(new State) 
{
}

Action::~Action() 
{
}

RenderAction::RenderAction()
  : Action()
{
}

RenderAction::~RenderAction() 
{
}

void
RenderAction::apply(Node * node)
{
  glEnable(GL_DEPTH_TEST);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  node->traverse(this);
  glFinish();
}

BoundingBoxAction::BoundingBoxAction()
  : Action() 
{
}

BoundingBoxAction::~BoundingBoxAction() 
{
}

void
BoundingBoxAction::extendBy(const box3 & box)
{
  this->box.extendBy(box.min);
  this->box.extendBy(box.max);
}

void
BoundingBoxAction::apply(Node * node)
{
  this->box.makeEmpty();
  node->traverse(this);
}

const box3 & 
BoundingBoxAction::getBoundingBox() const
{
  return this->box;
}
