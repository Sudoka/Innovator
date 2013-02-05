#include <nodes.h>
#include <opengl.h>
#include <actions.h>
#include <state.h>
#include <luawrapper.h>
#include <memory>

using namespace std;

class Program::ProgramP {
public:
  ProgramP(Program * self) 
    : program(new ShaderProgram()) 
  {
    Lua::dofile("../../src/" + self->fileName);

    program->attach(Lua::getstring("VertexShader"), GL_VERTEX_SHADER);
    program->attach(Lua::getstring("GeometryShader"), GL_GEOMETRY_SHADER);
    program->attach(Lua::getstring("FragmentShader"), GL_FRAGMENT_SHADER);

    if (self->transformFeedbackVaryings.size() > 0) {
      glTransformFeedbackVaryings(program->id, 
                                  self->transformFeedbackVaryings.size(), 
                                  self->transformFeedbackVaryings.data(), 
                                  GL_SEPARATE_ATTRIBS);
    }
    program->link();
  }
  ~ProgramP() {}
  unique_ptr<ShaderProgram> program;
};

Program::Program() 
  : self(nullptr) {}
Program::~Program() {}

void
Program::traverse(RenderAction * action)
{
  if (self.get() == nullptr) {
    self.reset(new ProgramP(this));
  }
  action->state->program = this;
}

unsigned int
Program::getProgramId() const
{
  return self->program->id;
}

void
Program::bind()
{
  glUseProgram(self->program->id);
}

void
Program::unbind()
{
  glUseProgram(0);
}