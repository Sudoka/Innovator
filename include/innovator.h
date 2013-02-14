#pragma once
#include <memory>
#include <string>

class Innovator {
public:
  Innovator(int width, int height);
  ~Innovator();

  void loop();

  static void postError(const std::string & msg);

private:
  class InnovatorP;
  std::unique_ptr<InnovatorP> self;
};