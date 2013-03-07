#pragma once

#include <GL/glew.h>
#include <map>
#include <string>
#include <vector>
#include <lua.hpp>
#include <glm/glm.hpp>

class Node;
class ArrayBuffer;

class Field {
public:
  virtual void read(lua_State * L) = 0;
  std::string name;
};

template <typename T>
class SField : public Field {
public:
  T value;
};

template <typename T>
class MField : public Field {
public:
  std::vector<T> vec;
  GLvoid * data() { return vec.data(); }
  GLsizeiptr size() { return sizeof(T) * vec.size(); }
};

class SFString : public Field {
public:
  virtual void read(lua_State * L);
  std::string value;
};

class SFVec3f : public Field {
public:
  virtual void read(lua_State * L);
  glm::vec3 value;
};

class SFFloat : public SField<float> {
public:
  virtual void read(lua_State * L);
};

class SFInt : public SField<int> {
public:
  virtual void read(lua_State * L);
};

class MFInt : public MField<int> {
public:
  virtual void read(lua_State * L);
};

class MFFloat : public MField<float> {
public:
  virtual void read(lua_State * L);
};

class MFVec3f : public MField<glm::vec3> {
public:
  virtual void read(lua_State * L);
};

class MFVec3i : public MField<glm::ivec3> {
public:
  virtual void read(lua_State * L);
};

template <class NodeType>
class SFNode : public Field {
public:
  std::shared_ptr<NodeType> value;
};

class SFArrayBuffer : public SFNode<ArrayBuffer> {
public:
  virtual void read(lua_State * L);
};

template <typename NodeType>
class MFChild : public Field {
public:
  std::vector<std::shared_ptr<NodeType>> values;
};

class MFNode : public MFChild<Node> {
public:
  virtual void read(lua_State * L);
};

class SFEnum : public SField<int> {
public:
  virtual void read(lua_State * L);
  typedef std::map<std::string, int> Enums;
  Enums enums;
};
