
#include <nodes.h>
#include <opengl.h>
#include <actions.h>
#include <state.h>
#include <memory>

using namespace std;

ShaderObject::ShaderObject(unsigned int type) : type(type) 
{
  LUA_NODE_ADD_FIELD_2(this->source, "source");
}

LUA_NODE_SOURCE(VertexShader);

void
VertexShader::initClass()
{
  LUA_NODE_INIT_CLASS(VertexShader, "VertexShader");
}

VertexShader::VertexShader()
  : ShaderObject(GL_VERTEX_SHADER) {}

LUA_NODE_SOURCE(GeometryShader);

void
GeometryShader::initClass()
{
  LUA_NODE_INIT_CLASS(GeometryShader, "GeometryShader");
}

GeometryShader::GeometryShader()
  : ShaderObject(GL_GEOMETRY_SHADER) {}

LUA_NODE_SOURCE(FragmentShader);

void
FragmentShader::initClass()
{
  LUA_NODE_INIT_CLASS(FragmentShader, "FragmentShader");
}

FragmentShader::FragmentShader()
  : ShaderObject(GL_FRAGMENT_SHADER) {}

class Program::ProgramP {
public:
  ProgramP(Program * self) 
    : program(new ShaderProgram()) 
  {
    for (unsigned int i = 0; i < self->shaders.values.size(); i++) {
      ShaderObject * shader = static_cast<ShaderObject*>(self->shaders.values[i].get());
      this->program->attach(shader->source.value.c_str(), shader->type);
    }
    program->link();
  }
  ~ProgramP() {}
  unique_ptr<ShaderProgram> program;
};

LUA_NODE_SOURCE(Program);

void
Program::initClass()
{
  LUA_NODE_INIT_CLASS(Program, "Program");
}

Program::Program() : self(nullptr) 
{
  LUA_NODE_ADD_FIELD_1(this->shaders);
}

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