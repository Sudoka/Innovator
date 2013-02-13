
#include <iostream>
#include <exception>
#include <innovator.h>

using namespace std;

int main(int argc, char * argv[])
{
  try {
    Innovator app(640, 480);
    app.loop();
  } catch (const exception & e) {
    cout << e.what() << endl;
    cout << "press Return to exit" << std::endl;
    cin.get();
  }
  exit(1);
}

