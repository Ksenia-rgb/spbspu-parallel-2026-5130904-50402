#include <iostream>

int main(int argc, char* argv[])
{
  if (argc < 3 || argc > 4)
  {
    std::cerr << "Invalid number of arguments\n";
    return 1;
  }
}