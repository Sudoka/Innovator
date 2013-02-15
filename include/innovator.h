#pragma once
#include <memory>
#include <string>

class Lua;

class Innovator {
public:
  Innovator(int width, int height);
  ~Innovator();

  static void resizeCB(int width, int height);
  static void mouseMovedCB(int x, int y);
  static void mouseButtonCB(int button, int action);

  void loop();

  static void postError(const std::string & msg);

  static Lua * lua();

  class InnovatorP;
private:
  static std::unique_ptr<InnovatorP> self;
};