#pragma once

#include <memory>
#include <string>

class Separator;

class File {
public:
  File();
  ~File();

  std::shared_ptr<Separator> readAll(const std::string & filename);

private:
  class FileP;
  std::unique_ptr<FileP> self;
};