#include <exception>
#include <iostream>
#include <vector>

#include "montecarlo.hpp"

int main()
{
  try
  {
  }
  catch (const std::exception &exception)
  {
    std::cerr << exception.what() << '\n';
    return 1;
  }
  return 0;
}
