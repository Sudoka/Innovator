#include <rendercache.h>
#include <state.h>
#include <nodes.h>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

DrawCache::DrawCache(State * state)
  : state(state),
    program(state->program),
    drawcall(state->drawcall),
    material(state->material),
    transform(state->transform)
{
}

std::function<void()>
DrawCache::updateProgram() const
{
  GLint viewmatloc = this->program->getUniformLocation("ViewMatrix");
  GLint projmatloc = this->program->getUniformLocation("ProjectionMatrix");
  
  return [=](){
    this->program->bind();
    GLMatrix viewmatrix(state->viewmatrix, viewmatloc);
    GLMatrix projmatrix(state->projmatrix, projmatloc);
    viewmatrix.updateGL();
    projmatrix.updateGL();
  };
}

std::function<void()>
DrawCache::updateMaterial() const
{
  return [=](){
    this->material->updateGL();
  };
}

std::function<void()>
DrawCache::executeDrawCall() const
{
  GLMatrix matrix(this->transform, this->program->getUniformLocation("ModelMatrix"));

  return [=](){
    matrix.updateGL();
    BindScope vao(this->drawcall->vao.get());
    this->drawcall->execute();
  };
}

void
DrawCache::flush()
{
  this->updateProgram()();
  this->executeDrawCall()();
}

RenderCache::RenderCache()
{
}

RenderCache::~RenderCache()
{
}

void
RenderCache::compile()
{
  // sort by shader id
  std::sort(begin(this->drawlist), end(this->drawlist), [](DrawCache & c1, DrawCache & c2) { return c1.program->id > c2.program->id; } );
  this->glcalls.push_back(this->drawlist[0].updateProgram());
  this->glcalls.push_back(this->drawlist[0].executeDrawCall());

  for (size_t i = 1; i < this->drawlist.size(); i++) {
    if (this->drawlist[i].program->id != this->drawlist[i-1].program->id) {
      this->glcalls.push_back(this->drawlist[i].updateProgram());
    }
    this->glcalls.push_back(this->drawlist[i].executeDrawCall());
  }
}

void 
RenderCache::flush()
{
  for (size_t i = 0; i < this->glcalls.size(); i++) {
    this->glcalls[i]();
  }
}