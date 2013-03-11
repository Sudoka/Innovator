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

class Value {
public:
  Value(lua_State * L, const std::string & name) 
    : L(L) {
    luaL_checktype(L, -1, LUA_TTABLE);
    lua_getfield(L, -1, name.c_str());
  }
  ~Value() {
    lua_pop(L, 1);
  }
  lua_State * L;
};

class Number : public Value {
public:
  Number(lua_State * L, const std::string & name)
    : Value(L, name), value(0) {
    if (!lua_isnil(L, -1)) {
      this->value = luaL_checknumber(L, -1);
    }
  }
  double value;
};

class Array : public Value {
public:
  Array(lua_State * L, const std::string & name, std::vector<double> & vec)
    : Value(L, name) {
    if (!lua_isnil(L, -1)) {
      int n = luaL_len(L, -1);
      for (int i = 1; i <= n; i++) {
        lua_rawgeti(L, -1, i);
        vec.push_back(luaL_checknumber(L, -1));
        lua_pop(L, 1);
      }
    }
  }
};

template <typename FieldType>
class MField : public Field {
public:
  virtual void read(lua_State * L) 
  {
    FieldType value(L, this->name, this->vec);
  }
  std::vector<double> vec;
};

template <typename FieldType>
class SField : public Field {
public:
  virtual void read(lua_State * L)
  {
    FieldType type(L, this->name);
    this->value = type.value;
  }
  double value;
};

class String {
public:
  static void read(lua_State * L, std::string & name, std::string & value)
  {
    Value field(L, name);
    if (!lua_isnil(L, -1)) {
      value = luaL_checkstring(L, -1);
    }
  }
};

class SFVec3f : public Field {
public:
  virtual void read(lua_State * L)
  {
    luaL_checktype(L, -1, LUA_TTABLE);
    lua_getfield(L, -1, this->name.c_str());
    if (!lua_isnil(L, -1)) {
      luaL_checktype(L, -1, LUA_TTABLE);
      int n = luaL_len(L, -1);
      assert(n == 3);
      for (int i = 0; i < n; i++) {
        lua_rawgeti(L, -1, i + 1);
        this->value[i] = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
      }
    }
    lua_pop(L, 1);
  }
  glm::vec3 value;
};

template <class NodeType>
class SFNode : public Field {
public:
  virtual void read(lua_State * L)
  {
    luaL_checktype(L, -1, LUA_TTABLE);
    int n = luaL_len(L, -1);
    assert(n <= 1);
    lua_rawgeti(L, -1, 1);
    if (!lua_isnil(L, -1)) {
      luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
      NodeType * node = static_cast<NodeType *>(lua_touserdata(L, -1));
      value.reset(node);
    }
    lua_pop(L, 1);
  }
  std::shared_ptr<NodeType> value;
};

class SFBuffer : public SFNode<Buffer> {
};

template <typename NodeType>
class MFChild : public Field {
public:
  virtual void read(lua_State * L)
  {
    luaL_checktype(L, -1, LUA_TTABLE);
    int n = luaL_len(L, -1);
    
    for (int i = 1; i <= n; i++) {
      lua_rawgeti(L, -1, i);
      luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
      NodeType * child = static_cast<NodeType *>(lua_touserdata(L, -1));
      this->values.push_back(shared_ptr<NodeType>(child));
      lua_pop(L, 1);
    }
  }
  std::vector<std::shared_ptr<NodeType>> values;
};

class SFString : public Field {
public:
  virtual void read(lua_State * L)
  {
    String::read(L, this->name, this->value);
  }
  std::string value;
};

class SFEnum : public Field {
public:
  virtual void read(lua_State * L)
  {
    std::string name;
    String::read(L, this->name, name);
    if (name.empty()) return;
    
    if (this->enums.find(name) == this->enums.end()) {
      Innovator::postError("Invalid Enum: " + name);
      return;
    }
    this->value = this->enums[name];
  }
  typedef std::map<std::string, int> Enums;
  Enums enums;
  int value;
};

typedef MFChild<Node> MFNode;
