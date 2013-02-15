#include <fields.h>

void
SFString::read(lua_State * L)
{
  luaL_checktype(L, -1, LUA_TTABLE);
  lua_getfield(L, -1, this->name.c_str());
  if (!lua_isnil(L, -1)) {
    this->value = luaL_checkstring(L, -1);
  }
  lua_pop(L, 1);
}

void
SFVec3f::read(lua_State * L)
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
