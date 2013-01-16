#include <elements.h>
#include <state.h>
#include <glm/gtc/type_ptr.hpp>

void
ProgramElement::updateGL(State * state)
{
  glUseProgram(this->program.id);
}

void
MatrixElement::updateGL(State * state)
{
  GLuint loc = glGetUniformLocation(state->programelem.program.id, this->name.c_str());
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(this->matrix));
}