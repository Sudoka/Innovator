#include <opengl.h>
#include <state.h>
#include <nodes.h>
#include <innovator.h>
#include <string>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

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
GLBufferObject::bindBufferBase(GLuint bindingPoint)
{
  if (this->target != GL_UNIFORM_BUFFER && this->target != GL_TRANSFORM_FEEDBACK_BUFFER) {
    throw std::runtime_error("GLBufferObject::bindBufferBase(): invalid target.");
  }
  glBindBufferBase(this->target, bindingPoint, this->buffer);
}

void
GLBufferObject::bufferSubData(GLintptr offset, GLsizeiptr size, const GLvoid * data)
{
  this->bind();
  glBufferSubData(this->target, offset, size, data);
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

GLDrawCall::GLDrawCall()
{
}

GLDrawCall::~GLDrawCall()
{
}

// *************************************************************************************************

GLDrawElements::GLDrawElements(GLenum mode, GLsizei count, GLenum type)
  : mode(mode),
    count(count),
    type(type)
{
}

GLDrawElements::~GLDrawElements()
{
}

void
GLDrawElements::execute()
{
  if (this->mode == GL_PATCHES) {
    glPatchParameteri(GL_PATCH_VERTICES, 3);
  }
  glDrawElements(this->mode, this->count, this->type, nullptr);
}

// *************************************************************************************************

GLUniformBufferFeedback::GLUniformBufferFeedback(GLenum mode, GLuint index, GLuint buffer)
  : mode(mode),
    index(index),
    buffer(buffer)
{
}

GLUniformBufferFeedback::~GLUniformBufferFeedback()
{
}

void
GLUniformBufferFeedback::bind()
{
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, this->index, this->buffer);
  glEnable(GL_RASTERIZER_DISCARD);
  glBeginTransformFeedback(this->mode);
}

void
GLUniformBufferFeedback::unbind()
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

GLUniformBuffer::GLUniformBuffer(GLuint blockbinding, GLsizeiptr count)
  : blockbinding(blockbinding),
    buffer(new GLBufferObject(GL_UNIFORM_BUFFER, GL_STREAM_DRAW, count * sizeof(mat4)))
{
}

GLUniformBuffer::~GLUniformBuffer()
{
}

void
GLUniformBuffer::updateGL(GLvoid * element, GLsizeiptr size, GLuint index)
{
  this->buffer->bufferSubData(size * index, size, element);
}

void
GLUniformBuffer::bindBuffer()
{
  this->buffer->bindBufferBase(this->blockbinding);
}

// *************************************************************************************************

GLProgram::GLProgram(const vector<shared_ptr<ShaderObject>> & shaderobjects)
  : id(glCreateProgram())
{
  for each (const shared_ptr<ShaderObject> & shader in shaderobjects) {
    this->attach(shader->source.value.c_str(), shader->type.value);
  }
  this->link();
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

GLint
GLProgram::getUniformLocation(const std::string & name)
{
  if (this->uniformLocations.find(name) == this->uniformLocations.end()) {
    this->uniformLocations[name] = glGetUniformLocation(this->id, name.c_str());
  }
  return this->uniformLocations[name];
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

// *************************************************************************************************

GLTextureUnit::GLTextureUnit(GLuint unit)
  : unit(unit)
{
}

GLTextureUnit::~GLTextureUnit()
{
}

void
GLTextureUnit::bind()
{
  glActiveTexture(GL_TEXTURE0 + this->unit);
}

void
GLTextureUnit::unbind()
{
  glActiveTexture(GL_TEXTURE0);
}

// *************************************************************************************************

GLTextureObject::GLTextureObject(GLenum target, 
                                 GLint level,
                                 GLint internalFormat,
                                 GLsizei width,
                                 GLsizei height,
                                 GLint border,
                                 GLenum format,
                                 GLenum type,
                                 const GLvoid * data)
  : target(target)
{
  glGenTextures(1, &this->id);
  glBindTexture(this->target, this->id);
  glTexImage2D(this->target, level, internalFormat, width, height, border, format, type, data);
  glBindTexture(this->target, 0);
}

GLTextureObject::~GLTextureObject()
{
  glDeleteTextures(1, &this->id);
}

void
GLTextureObject::bind()
{
  glBindTexture(this->target, this->id);
}

void
GLTextureObject::unbind()
{
  glBindTexture(this->target, 0);
}

// *************************************************************************************************

GLTextureSampler::GLTextureSampler(GLint wraps, 
                                   GLint wrapt,
                                   GLint wrapr,
                                   GLint min_filter,
                                   GLint mag_filter)
  : unit(0)
{
  glGenSamplers(1, &this->id);
  glSamplerParameteri(this->id, GL_TEXTURE_WRAP_S, wraps);
  glSamplerParameteri(this->id, GL_TEXTURE_WRAP_T, wrapt);
  glSamplerParameteri(this->id, GL_TEXTURE_WRAP_R, wrapr);
  glSamplerParameteri(this->id, GL_TEXTURE_MIN_FILTER, min_filter);
  glSamplerParameteri(this->id, GL_TEXTURE_MAG_FILTER, mag_filter);
}

GLTextureSampler::~GLTextureSampler()
{
  glDeleteSamplers(1, &this->id);
}

void
GLTextureSampler::setUnit(GLuint unit)
{
  this->unit = unit;
}

void
GLTextureSampler::bind()
{
  glBindSampler(this->unit, this->id);
}

void
GLTextureSampler::unbind()
{
  glBindSampler(this->unit, 0);
}

// *************************************************************************************************

GLFramebufferObject::GLFramebufferObject()
{
  glGenFramebuffers(1, &this->id);
}

GLFramebufferObject::~GLFramebufferObject()
{
  glDeleteFramebuffers(1, &this->id);
}

void
GLFramebufferObject::attach(GLenum attachment, GLuint texture)
{
  glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, 0);
}

void
GLFramebufferObject::checkStatus()
{
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("framebuffer incomplete");
  }
}

void
GLFramebufferObject::bind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, this->id);
}

void
GLFramebufferObject::unbind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// *************************************************************************************************