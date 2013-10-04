#pragma once
#include <memory>
#include <string>

class Innovator {
public:
  Innovator();
  ~Innovator();

  static void CreateWindow(int width, int height, const std::string & title);

  static void init();
  static void loop();
  static void exit();

};