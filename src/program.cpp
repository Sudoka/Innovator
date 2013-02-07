#include <nodes.h>
#include <opengl.h>
#include <actions.h>
#include <state.h>
#include <memory>

using namespace std;

void
VertexShader::attach(ShaderProgram * program)
{
  program->attach(this->source.c_str(), GL_VERTEX_SHADER);
}

void
GeometryShader::attach(ShaderProgram * program)
{
  program->attach(this->source.c_str(), GL_GEOMETRY_SHADER);
}

void
FragmentShader::attach(ShaderProgram * program)
{
  program->attach(this->source.c_str(), GL_FRAGMENT_SHADER);
}

class Program::ProgramP {
public:
  ProgramP(Program * self) 
    : program(new ShaderProgram()) 
  {
    for (unsigned int i = 0; i < self->shaders.size(); i++) {
      self->shaders[i]->attach(this->program.get());
    }
    program->link();
  }
  ~ProgramP() {}
  unique_ptr<ShaderProgram> program;
};

Program::Program() : self(nullptr) {}
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