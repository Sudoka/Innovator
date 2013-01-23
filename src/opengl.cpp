#include <opengl.h>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

using namespace glm;
using namespace std;

BufferObject::BufferObject(GLenum target)
  : target(target)
{
  glGenBuffers(1, &this->buffer);
}

BufferObject::BufferObject(GLenum target, GLenum usage, GLsizeiptr size, GLvoid * data)
  : target(target)
{
  this->construct(target, usage, size, data);
}

BufferObject::BufferObject(const std::vector<glm::vec3> & data, GLenum target, GLenum usage)
  : target(target)
{
  this->construct(target, usage, sizeof(vec3) * data.size(), data.data());
}

BufferObject::BufferObject(const std::vector<ivec3> & data, GLenum target, GLenum usage)
  : target(target)
{
  this->construct(target, usage, sizeof(ivec3) * data.size(), data.data());
}

void
BufferObject::construct(GLenum target, GLenum usage, GLsizeiptr size, const GLvoid * data)
{
  glGenBuffers(1, &this->buffer);
  this->setValues(usage, size, data);
}

BufferObject::~BufferObject()
{
  glBindBuffer(this->target, this->buffer);
  glDeleteBuffers(1, &this->buffer);
}

void
BufferObject::setValues(GLenum usage, GLsizeiptr size, const GLvoid * data)
{
  this->bind();
  glBufferData(this->target, size, data, usage);
  this->unbind();
}

void
BufferObject::set1Value(int index, GLuint value)
{
  this->bind();
  glBufferSubData(this->target, sizeof(GLuint) * index, sizeof(GLuint), &value);
  this->unbind();
}

void
BufferObject::bind()
{
  glBindBuffer(this->target, this->buffer);
}

void
BufferObject::unbind()
{
  glBindBuffer(this->target, 0);
}

VertexBuffer::VertexBuffer(const std::vector<vec3> & data, GLuint index, GLuint divisor)
  : index(index),
    divisor(divisor),
    BufferObject(data, GL_ARRAY_BUFFER, GL_STATIC_DRAW)
{
}

VertexBuffer::~VertexBuffer()
{
}

void
VertexBuffer::bind()
{
  BufferObject::bind();
  glEnableVertexAttribArray(this->index);
  glVertexAttribPointer(this->index, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glVertexAttribDivisor(this->index, this->divisor);
}

void
VertexBuffer::unbind()
{
  BufferObject::unbind();
  glDisableVertexAttribArray(this->index);
}

TransformFeedback::TransformFeedback(GLuint buffer, GLenum mode)
  : mode(mode)
{
  glGenTransformFeedbacks(1, &this->id);
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, this->id);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buffer);
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

}

TransformFeedback::~TransformFeedback()
{
  glDeleteTransformFeedbacks(1, &this->id);
}

void
TransformFeedback::bind()
{
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, this->id);
  glEnable(GL_RASTERIZER_DISCARD);
  glBeginTransformFeedback(this->mode);
}

void
TransformFeedback::unbind()
{
  glEndTransformFeedback();
  glDisable(GL_RASTERIZER_DISCARD);
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

// *************************************************************************************************

struct ShaderObject {
  ShaderObject() {}
  ShaderObject(GLenum type, const string & tag)
    : type(type), tag(tag) {}
  GLenum type;
  string tag;
  string source;
};

ShaderProgram::ShaderProgram() 
  : id(0) {}

ShaderProgram::ShaderProgram(const string & filename, vector<const char *> transformFeedbackVaryings) 
  : id(0)
{
  vector<ShaderObject> objects(5);
  objects[0] = ShaderObject(GL_VERTEX_SHADER,          "//-- Vertex");
  objects[1] = ShaderObject(GL_TESS_CONTROL_SHADER,    "//-- Tess Control");
  objects[2] = ShaderObject(GL_TESS_EVALUATION_SHADER, "//-- Tess Evaluation");
  objects[3] = ShaderObject(GL_GEOMETRY_SHADER,        "//-- Geometry");
  objects[4] = ShaderObject(GL_FRAGMENT_SHADER,        "//-- Fragment");

  this->id = glCreateProgram();

  string line;
  ifstream file("../../src/" + filename);
  
  if (file.is_open()) {
    
    string version("#version 400 core");
    int index = -1;
    
    while (file.good()) {
      getline(file, line);
      line += "\n";
      if (line.find("#version") != string::npos) {
        version = line;
        continue;
      }
      for (size_t i = 0; i < objects.size(); i++) {
        if (line.find(objects[i].tag) != string::npos) {
          objects[i].source += version;
          index = i;
          continue;
        }
      }
      if (index < 0) continue;
      objects[index].source += line;
    }
    file.close();
    
    for (auto i = objects.begin(); i != objects.end(); i++) {
      if (!i->source.empty()) {
        GLuint shader = glCreateShader(i->type);
        const char * s = i->source.c_str();
        glShaderSource(shader, 1, &s, NULL);
        glCompileShader(shader);

        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE) {
          GLint length;
          glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
          GLchar * log = new GLchar[length + 1];
          glGetShaderInfoLog(shader, length, NULL, log);
          cout << "Shader::attach" << "failed to compile shader: " << log << endl;
          delete [] log;
        } else {
          glAttachShader(this->id, shader);
          glDeleteShader(shader);
        }
      }
    }
    GLsizei count = transformFeedbackVaryings.size();
    if (count > 0) {
      const char ** varyings = transformFeedbackVaryings.data();
      glTransformFeedbackVaryings(this->id, count, varyings, GL_SEPARATE_ATTRIBS);
    }
    this->link();
  } else {
    cout << "Unable to open file" << endl;
  }
}

ShaderProgram::~ShaderProgram()
{
  glDeleteProgram(this->id);
}

void
ShaderProgram::link()
{
  glLinkProgram(this->id);
  
  GLint status;
  glGetProgramiv(this->id, GL_LINK_STATUS, &status);
  if (status != GL_TRUE) {
    GLint length;
    glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &length);
    GLchar * log = new GLchar[length + 1];
    glGetProgramInfoLog(this->id, length, NULL, log);
    cout << "Shader::link()" << "failed to link shader: " <<  log << endl;
    delete [] log;
  }
}
