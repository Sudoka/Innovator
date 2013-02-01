#include <nodes.h>
#include <opengl.h>
#include <actions.h>
#include <state.h>
#include <lua.hpp>
#include <memory>

using namespace std;

class Program::ProgramP {
public:
  ProgramP(Program * self) 
    : program(new ShaderProgram()) 
  {
    string fullname = "../../src/" + self->fileName;

    lua_State * L = luaL_newstate();
    luaL_openlibs(L);
    luaL_dofile(L, fullname.c_str());

    lua_getglobal(L, "VertexShader");
    if (lua_isstring(L, -1)) {
      program->attach(lua_tostring(L, -1), GL_VERTEX_SHADER);
    }
    lua_getglobal(L, "GeometryShader");
    if (lua_isstring(L, -1)) {
      program->attach(lua_tostring(L, -1), GL_GEOMETRY_SHADER);
    }
    lua_getglobal(L, "FragmentShader");
    if (lua_isstring(L, -1)) {
      program->attach(lua_tostring(L, -1), GL_FRAGMENT_SHADER);
    }
    lua_close(L);

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