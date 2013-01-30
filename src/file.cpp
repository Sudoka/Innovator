#include <file.h>
#include <lua.hpp>
#include <luawrapper.h>
#include <nodes.h>
#include <iostream>

using namespace std;

class File::FileP {
public:
  FileP() 
  {
    Lua::dofile("../../src/file.lua");
    Lua::registerNodes();
  }
  ~FileP() 
  {
  }
};

File::File()
  : self(new FileP)
{
}

File::~File()
{
}

shared_ptr<Separator> 
File::readAll(const string & filename)
{
  return Lua::doivfile(filename);
}