#pragma once

#include <lua.hpp>
#include <innovator.h>
#include <luawrapper.h>

template <typename NodeType>
static int LuaNode(lua_State * L) {
  lua_pushlightuserdata(L, NodeType::createInstance(L));
  return 1;
}

template <typename T>
static void RegisterClass(const std::string & name)
{
  Lua * lua = Innovator::lua();
  lua->registerFunction(name, LuaNode<T>);
}

#define LUA_NODE_HEADER(_class_) \
public: \
  static _class_ * createInstance(lua_State * L); \

#define LUA_NODE_SOURCE(_class_) \
_class_ * _class_::createInstance(lua_State * L) \
{ \
  _class_ * self = new _class_; \
  for (auto it = self->fields.begin(); it != self->fields.end(); it++) { \
    (*it)->read(L); \
  } \
  return self; \
}

#define LUA_NODE_ADD_FIELD_1(_field_) \
  fields.push_back(&_field_); \

#define LUA_NODE_ADD_FIELD_2(_field_, _name_) \
  _field_.name = _name_; \
  fields.push_back(&_field_); \

#define LUA_NODE_ADD_FIELD_3(_field_, _name_, _value_) \
  _field_.name = _name_; \
  _field_.value = _value_; \
  fields.push_back(&_field_); \


#define LUA_NODE_INIT_CLASS(_class_, _name_) \
  RegisterClass<_class_>(_name_); \

