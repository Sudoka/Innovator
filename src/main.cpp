
#include <iostream>
#include <innovator.h>

int main(int argc, char * argv[])
{
  {
    Innovator app(640, 480);
    app.loop();
  }

  std::cout << "press Return to exit" << std::endl;
  std::cin.get();
  std::exit(1);
}

