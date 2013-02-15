#pragma once

#include <lua.hpp>

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
} \

#define LUA_NODE_ADD_FIELD(_field_, _name_) \
  static_cast<Field*>(_field_)->name = _name_; \
  fields.push_back(_field_); \

