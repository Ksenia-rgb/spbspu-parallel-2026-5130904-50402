#include "commands.hpp"
#include <cstddef>
#include <stdexcept>
#include <string>

// [M1] из анонимного namespace main.cpp; теперь принимает std::string
long long chernikov::parseInteger(const std::string &str)
{
  size_t pos = 0;
  const long long value = std::stoll(str, &pos);
  if (pos != str.size()) {
    throw std::invalid_argument("not an integer");
  }
  return value;
}