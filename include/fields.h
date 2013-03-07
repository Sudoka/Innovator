#pragma once

#include <GL/glew.h>
#include <map>
#include <string>
#include <vector>
#include <lua.hpp>
#include <glm/glm.hpp>
#include <innovator.h>

class Node;
template <typename T>
class Buffer;

class Field {
public:
  virtual void read(lua_State * L) = 0;
  std::string name;
};

template <typename T>
class SField : public Field {
public:
  virtual void read(lua_State * L)
  {
    luaL_checktype(L, -1, LUA_TTABLE);
    lua_getfield(L, -1, this->name.c_str());
    if (!lua_isnil(L, -1)) {
      this->value = static_cast<T>(luaL_checknumber(L, -1));
    }
    lua_pop(L, 1);
  }
  T value;
};

template <typename T>
class MField : public Field {
public:
  virtual void read(lua_State * L) 
  {
    luaL_checktype(L, -1, LUA_TTABLE);
    lua_getfield(L, -1, this->name.c_str());
    int n = luaL_len(L, -1);
    
    this->vec.resize(n);
    T * dataptr = (T *) this->vec.data();
    
    for (int i = 1; i <= n; i++) {
      lua_rawgeti(L, -1, i);
      dataptr[i-1] = (T)luaL_checknumber(L, -1);
      lua_pop(L, 1);
    }
    lua_pop(L, 1);
  }
  std::vector<T> vec;
};

static void ReadString(lua_State * L, std::string & value, const char * name)
{
  luaL_checktype(L, -1, LUA_TTABLE);
  lua_getfield(L, -1, name);
  if (!lua_isnil(L, -1)) {
    value = luaL_checkstring(L, -1);
  }
  lua_pop(L, 1);
}

class SFVec3f : public Field {
public:
  virtual void read(lua_State * L)
  {
    luaL_checktype(L, -1, LUA_TTABLE);
    lua_getfield(L, -1, this->name.c_str());
    
    luaL_checktype(L, -1, LUA_TTABLE);
    if (!lua_isnil(L, -1)) {
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

template <typename T>
class SFBuffer : public SFNode<Buffer<T>> {
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
    ReadString(L, this->value, this->name.c_str());
  }
  std::string value;
};

class SFEnum : public SField<int> {
public:
  virtual void read(lua_State * L)
  {
    std::string name;
    ReadString(L, name, this->name.c_str());
    if (name.empty()) return;
    
    if (this->enums.find(name) == this->enums.end()) {
      Innovator::postError("Invalid Enum: " + name);
      return;
    }
    this->value = this->enums[name];
  }
  typedef std::map<std::string, int> Enums;
  Enums enums;
};

typedef SField<int> SFInt;
typedef SField<float> SFFloat;
typedef MField<int> MFInt;
typedef MField<float> MFFloat;
typedef MFChild<Node> MFNode;
