
#include <innovator.h>

int main(int argc, char * argv[])
{
  Innovator::init();
  Innovator::CreateWindow(640, 480, "Innovator");
  Innovator::CreateWindow(640, 480, "Innovator");
  Innovator::loop();
  Innovator::exit();
  exit(EXIT_SUCCESS);
}

