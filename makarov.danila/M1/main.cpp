#include <iostream>
#include <stdexcept>
#include "args.hpp"

int main(int argc, char ** argv)
{
  try {
    makarov::parseParameters(argc, argv);
  } catch (const std::invalid_argument & e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }
}
