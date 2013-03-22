#pragma once

#include <GL/glew.h>
#include <map>
#include <string>
#include <vector>
#include <lua.hpp>
#include <glm/glm.hpp>
#include <innovator.h>

class Node;
class Buffer;

class Field {
public:
  virtual void read(lua_State * L) = 0;
  std::string name;
};

template <typename T>
class FieldValue {
public:
  T readValue(lua_State * L);
};

template <typename T>
class Value : public FieldValue<T> {
public:
  Value(lua_State * L, T & value)
  {
    luaL_checktype(L, -1, LUA_TTABLE);
    int n = luaL_len(L, -1);
    assert(n <= 1);
    lua_rawgeti(L, -1, 1);
    if (!lua_isnil(L, -1)) {
      value = this->readValue(L);
    }
    lua_pop(L, 1);
  }
  Value(lua_State * L, const std::string & name, T & value)
  {
    luaL_checktype(L, -1, LUA_TTABLE);
    lua_getfield(L, -1, name.c_str());
    if (!lua_isnil(L, -1)) {
      value = this->readValue(L);
    }
    lua_pop(L, 1);
  }
};

template <typename T>
class Array : public FieldValue<T> {
public:
  Array(lua_State * L, std::vector<T> & vec)
  {
    luaL_checktype(L, -1, LUA_TTABLE);
    this->read(L, vec);
  }
  Array(lua_State * L, const std::string & name, std::vector<T> & vec)
  {
    luaL_checktype(L, -1, LUA_TTABLE);
    lua_getfield(L, -1, name.c_str());
    if (!lua_isnil(L, -1)) {
      this->read(L, vec);
    }
    lua_pop(L, 1);
  }
  void read(lua_State * L, std::vector<T> & vec) {
    if (!lua_isnil(L, -1)) {
      int n = luaL_len(L, -1);
      for (int i = 1; i <= n; i++) {
        lua_rawgeti(L, -1, i);
        vec.push_back(this->readValue(L));
        lua_pop(L, 1);
      }
    }
  }
};

class SFNumber : public Field {
public:
  virtual void read(lua_State * L)
  {
    Value<double> value(L, this->name, this->value);
  }
  double value;
};

class SFInt : public Field {
public:
  virtual void read(lua_State * L)
  {
    Value<GLint> value(L, this->name, this->value);
  }
  GLint value;
};

class SFUint : public Field {
public:
  virtual void read(lua_State * L)
  {
    Value<GLuint> value(L, this->name, this->value);
  }
  GLuint value;
};

class SFString : public Field {
public:
  virtual void read(lua_State * L)
  {
    Value<std::string> value(L, this->name, this->value);
  }
  std::string value;
};

class MFInt : public Field {
public:
  virtual void read(lua_State * L) 
  {
    Array<int> value(L, this->name, this->vec);
  }
  std::vector<int> vec;
};

class MFNumber : public Field {
public:
  virtual void read(lua_State * L) 
  {
    Array<double> value(L, this->name, this->vec);
  }
  std::vector<double> vec;
};

class SFBuffer : public Field {
public:
  virtual void read(lua_State * L)
  {
    Value<std::shared_ptr<Buffer>> value(L, this->value);
  }
  std::shared_ptr<Buffer> value;
};

class MFNode : public Field {
public:
  virtual void read(lua_State * L)
  {
    Array<std::shared_ptr<Node>> value(L, this->values);
  }
  std::vector<std::shared_ptr<Node>> values;
};

class SFVec3f : public Field {
public:
  virtual void read(lua_State * L)
  {
    std::vector<double> tmp;
    Array<double> value(L, this->name, tmp);
    if (tmp.size() == 3) {
      this->value = glm::vec3(tmp[0], tmp[1], tmp[2]);
    }
  }
  glm::vec3 value;
};

class SFEnum : public Field {
public:
  virtual void read(lua_State * L)
  {
    std::string name;
    Value<std::string> value(L, this->name, name);
    if (name.empty()) return;
    
    if (this->enums.find(name) == this->enums.end()) {
      Innovator::postError("Invalid Enum: " + name);
      return;
    }
    this->value = this->enums[name];
  }
  int value;
  std::map<std::string, int> enums;
};
