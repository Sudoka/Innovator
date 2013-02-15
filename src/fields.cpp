#include <fields.h>

void
StringField::read(lua_State * L)
{
  luaL_checktype(L, -1, LUA_TTABLE);
  lua_getfield(L, -1, this->name.c_str());
  if (!lua_isnil(L, -1)) {
    this->value = luaL_checkstring(L, -1);
  }
  lua_pop(L, 1);
}