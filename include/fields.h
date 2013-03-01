#pragma once

#include <string>
#include <vector>
#include <map>
#include <lua.hpp>
#include <glm/glm.hpp>
#include <node.h>

class Buffer;

class Field {
public:
  virtual void read(lua_State * L) = 0;
  std::string name;
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

template <typename T>
class SFNumber : public Field {
public:
  T value;
};

class SFFloat : public SFNumber<float> {
public:
  virtual void read(lua_State * L);
};

class SFUint32 : public SFNumber<unsigned int> {
public:
  virtual void read(lua_State * L);
};

template <typename T>
class MFVec3 : public Field {
public:
  std::vector<glm::detail::tvec3<T>> vec;
};

class MFVec3f : public MFVec3<float> {
public:
  virtual void read(lua_State * L);
};

class MFVec3i : public MFVec3<int> {
public:
  virtual void read(lua_State * L);
};

class SFBuffer : public Field {
public:
  virtual void read(lua_State * L);
  std::shared_ptr<Buffer> value;
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

class SFEnum : public SFNumber<int> {
public:
  virtual void read(lua_State * L);
  typedef std::map<std::string, int> Enums;
  Enums enums;
};