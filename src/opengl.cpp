#include <opengl.h>
#include <innovator.h>
#include <string>

using namespace std;

template <typename T>
GLBufferObject * GetGLBuffer(GLenum target, GLenum usage, GLuint count, vector<double> & vec)
{
  GLsizeiptr size = count > 0 ? sizeof(T) * count : sizeof(T) * vec.size();
  GLBufferObject * glbuffer = new GLBufferObject(target, usage, size);
  if (vec.size() > 0) {
    T * data = (T *)glbuffer->map(GL_WRITE_ONLY);
    for (size_t i = 0; i < vec.size(); i++) {
      data[i] = static_cast<T>(vec[i]);
    }
    glbuffer->unmap();
  }
  return glbuffer;
}

GLBufferObject * 
GLBufferObject::create(GLenum target, GLenum usage, GLenum type, GLuint count, vector<double> & vec)
{
  switch (type) {
  case GL_FLOAT:
    return GetGLBuffer<GLfloat>(target, usage, count, vec);
    break;
  case GL_UNSIGNED_INT:
    return GetGLBuffer<GLuint>(target, usage, count, vec);
    break;
  default:
    throw std::invalid_argument("GLBufferObject::create(): Invalid buffer type");
  }
}

GLBufferObject::GLBufferObject(GLenum target, GLenum usage, GLsizeiptr size, GLvoid * data)
  : target(target) 
{
  glGenBuffers(1, &this->buffer);
  glBindBuffer(this->target, this->buffer);
  glBufferData(this->target, size, data, usage);
  glBindBuffer(this->target, 0);
}
  
GLBufferObject::~GLBufferObject() 
{
  glBindBuffer(this->target, 0);
  glDeleteBuffers(1, &this->buffer);
}

void * 
GLBufferObject::map(GLenum access) 
{
  this->bind();
  void * data = glMapBuffer(this->target, access);
  if (!data) {
    throw std::runtime_error("GLBufferObject::map(): Unable to map buffer.");
  }
  return data;
}

void 
GLBufferObject::unmap() 
{
  glUnmapBuffer(this->target);
  this->unbind();
}

void
GLBufferObject::bind() 
{
  glBindBuffer(this->target, this->buffer);
}

void
GLBufferObject::unbind() 
{
  glBindBuffer(this->target, 0);
}

// *************************************************************************************************

GLVertexArrayObject::GLVertexArrayObject()
{
  glGenVertexArrays(1, &this->vao);
}

GLVertexArrayObject::~GLVertexArrayObject()
{
  glDeleteVertexArrays(1, &this->vao);
}

void
GLVertexArrayObject::bind()
{
  glBindVertexArray(this->vao);
}

void
GLVertexArrayObject::unbind()
{
  glBindVertexArray(0);
}

// *************************************************************************************************

GLVertexAttribute::GLVertexAttribute(GLuint index, GLint size, GLenum type, GLuint divisor)
  : index(index), 
    size(size),
    type(type),
    divisor(divisor)
{
}

void
GLVertexAttribute::bind()
{
  glEnableVertexAttribArray(this->index);
  glVertexAttribPointer(this->index, this->size, this->type, GL_FALSE, 0, 0);
  glVertexAttribDivisor(this->index, this->divisor);
}

void
GLVertexAttribute::unbind()
{
  glDisableVertexAttribArray(this->index);
}

// *************************************************************************************************

GLTransformFeedback::GLTransformFeedback(GLenum mode, GLuint index, GLuint buffer)
  : mode(mode),
    index(index),
    buffer(buffer)
{
}

GLTransformFeedback::~GLTransformFeedback()
{
}

void
GLTransformFeedback::bind()
{
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, this->index, this->buffer);
  glEnable(GL_RASTERIZER_DISCARD);
  glBeginTransformFeedback(this->mode);
}

void
GLTransformFeedback::unbind()
{
  glEndTransformFeedback();
  glDisable(GL_RASTERIZER_DISCARD);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, this->index, 0);
}

// *************************************************************************************************

GLQueryObject::GLQueryObject(GLenum target)
  : target(target)
{
  glGenQueries(1, &this->query);
}

GLQueryObject::~GLQueryObject()
{
  glDeleteQueries(1, &this->query);
}

GLuint 
GLQueryObject::getResult()
{
  GLuint result = 0;
  glGetQueryObjectuiv(this->query, GL_QUERY_RESULT, &result);
  return result;
}

void
GLQueryObject::bind()
{
  glBeginQuery(this->target, this->query);
}
 
void
GLQueryObject::unbind()
{
  glEndQuery(this->target);
}

// *************************************************************************************************

GLProgram::GLProgram() 
  : id(glCreateProgram())
{
}

GLProgram::~GLProgram()
{
  glDeleteProgram(this->id);
}

void 
GLProgram::attach(const char * source, GLenum type)
{
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);
  
  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    GLint length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    GLchar * log = new GLchar[length + 1];
    glGetShaderInfoLog(shader, length, NULL, log);
    Innovator::postError("Shader::attach: failed to compile shader: " + string(log));
    delete [] log;
  } else {
    glAttachShader(this->id, shader);
    glDeleteShader(shader);
  }
}

void
GLProgram::link()
{
  glLinkProgram(this->id);
  
  GLint status;
  glGetProgramiv(this->id, GL_LINK_STATUS, &status);
  if (status != GL_TRUE) {
    GLint length;
    glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &length);
    GLchar * log = new GLchar[length + 1];
    glGetProgramInfoLog(this->id, length, NULL, log);
    Innovator::postError("Shader::link(): failed to link program: " + string(log));
    delete [] log;
  }
}

void
GLProgram::bind()
{
  glUseProgram(this->id);
}

void
GLProgram::unbind()
{
  glUseProgram(0);
}
