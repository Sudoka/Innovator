#pragma once

#include <memory>
#include <string>
#include <nodes.h>

class File {
public:
  static void init();
  static void exit();
  static Separator::ptr readAll(const std::string & filename);
};