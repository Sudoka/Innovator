
#include <iostream>
#include <innovator.h>

int main(int argc, char * argv[])
{
  Innovator innovator(640, 480);
  innovator.loop();

  std::cout << "press Return to exit" << std::endl;
  std::cin.get();
  std::exit(1);
}

