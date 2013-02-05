#pragma once

#include <memory>
#include <string>

class Separator;

class File {
public:
  static void init();
  static void exit();
  static std::shared_ptr<Separator> readAll(const std::string & filename);
};