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
  Lua::dofile(filename);
  Separator * scene = static_cast<Separator *>(Lua::getuserdata("root"));
  assert(scene);
  return std::shared_ptr<Separator>(scene);
}