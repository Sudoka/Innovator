#include <opengl.h>
#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

using namespace glm;
using namespace std;

ArrayBuffer::Scope::Scope(ArrayBuffer & buffer) 
  : buffer(buffer) 
{ 
  this->buffer.bind(); 
}

ArrayBuffer::Scope::~Scope() 
{ 
  this->buffer.unbind(); 
}

void
ArrayBuffer::Scope::draw() 
{ 
  this->buffer.draw();
}


ArrayBuffer::ArrayBuffer() 
  : index(0),
    divisor(0)
{
}

ArrayBuffer::~ArrayBuffer() 
{
}

void
ArrayBuffer::createBuffer(const vector<vec3> & data, GLuint index, GLuint divisor)
{
  this->index = index;
  this->divisor = divisor;
  
  glGenBuffers(1, &this->buffer);
  glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * data.size(), &data[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
ArrayBuffer::deleteBuffer()
{
  glDeleteBuffers(1, &this->buffer);
}

void
ArrayBuffer::bind()
{
  glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
  glEnableVertexAttribArray(this->index);
  glVertexAttribDivisor(this->index, this->divisor);
  glVertexAttribPointer(this->index, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void
ArrayBuffer::draw()
{
  glDrawArrays(GL_TRIANGLES, 0, 3);
}


void
ArrayBuffer::unbind()
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDisableVertexAttribArray(this->index);
}

struct ShaderObject {
  ShaderObject() {}
  ShaderObject(GLenum type, const string & tag)
    : type(type), tag(tag) {}
  GLenum type;
  string tag;
  string source;
};

ShaderProgram::ShaderProgram() 
  : id(0)
{
}

void
ShaderProgram::createProgram(const string & filename) 
{
  vector<ShaderObject> objects(5);
  objects[0] = ShaderObject(GL_VERTEX_SHADER,          "//-- Vertex");
  objects[1] = ShaderObject(GL_TESS_CONTROL_SHADER,    "//-- Tess Control");
  objects[2] = ShaderObject(GL_TESS_EVALUATION_SHADER, "//-- Tess Evaluation");
  objects[3] = ShaderObject(GL_GEOMETRY_SHADER,        "//-- Geometry");
  objects[4] = ShaderObject(GL_FRAGMENT_SHADER,        "//-- Fragment");

  this->id = glCreateProgram();

  string line;
  ifstream file(filename);
  
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
    glLinkProgram(this->id);

    GLint status;
    glGetProgramiv(this->id, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
      GLint length;
      glGetShaderiv(this->id, GL_INFO_LOG_LENGTH, &length);
      GLchar * log = new GLchar[length + 1];
      glGetProgramInfoLog(this->id, length, NULL, log);
      cout << "Shader::link()" << "failed to link shader: " <<  log << endl;
      delete [] log;
    }
  } else {
    cout << "Unable to open file" << endl;
  }
}

void 
ShaderProgram::deleteProgram()
{
  glDeleteProgram(this->id);
}
