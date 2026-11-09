#include <iostream>
#include <string>

#include "commands.hpp"

int main()
{
  std::string line;
  while (std::getline(std::cin, line))
  {
    karpenkov::execute(line);
  }
  return 0;
}