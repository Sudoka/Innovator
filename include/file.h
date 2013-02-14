#pragma once

#include <memory>
#include <string>
#include <map>
#include <nodes.h>

class File {
public:
  static void init();
  static void exit();
  static Separator::ptr readAll(const std::string & filename);

  typedef std::map<std::string, Draw::Mode> DrawModeMap;
  static DrawModeMap drawmodes;
};