#pragma once

class Innovator {
public:
  Innovator(int width, int height);
  ~Innovator();

  void loop();

private:
  bool initialized;
};