#include <rendercache.h>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

DrawCache::DrawCache(GLProgram * program,
                     GLMaterial * material,
                     GLMatrix * viewmatrix,
                     GLMatrix * projmatrix,
                     GLMatrix * transform,
                     GLVertexArrayObject * vao,
                     GLDrawCall * drawcall)
  : program(program),
    material(material),
    transform(transform),
    viewmatrix(viewmatrix),
    projmatrix(projmatrix),
    vao(vao),
    drawcall(drawcall)
{
}

void 
DrawCache::flush()
{
  BindScope program(this->program);
  //this->viewmatrix->updateGL(this->program);
  //this->projmatrix->updateGL(this->program);
  this->material->updateGL(this->program);
  this->transform->updateGL(this->program);
  BindScope vao(this->vao);
  this->drawcall->execute();
}

RenderCache::RenderCache()
{
}

RenderCache::~RenderCache()
{
}

void
RenderCache::compile()
{/*
  std::sort(begin(this->drawlist), end(this->drawlist), [](DrawCache & c1, DrawCache & c2) { return c1.program->id > c2.program->id; } );
  

  this->glcalls.push_back(this->drawlist[0].updateProgram());
  this->glcalls.push_back(this->drawlist[0].executeDrawCall());

  for (size_t i = 1; i < this->drawlist.size(); i++) {
    if (this->drawlist[i].program->id != this->drawlist[i-1].program->id) {
      this->glcalls.push_back(this->drawlist[i].updateProgram());
    }
    this->glcalls.push_back(this->drawlist[i].executeDrawCall());
  }
  */
}

void 
RenderCache::flush()
{
  for (size_t i = 0; i < this->drawlist.size(); i++) {
    this->drawlist[i].flush();
  }
}