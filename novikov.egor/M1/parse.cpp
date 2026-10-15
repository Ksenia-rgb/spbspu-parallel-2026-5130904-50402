#include "parse.hpp"

#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <limits>
#include <stdexcept>

std::size_t novikov::stringToSizeT(const char *str)
{
  if (!str)
  {
    throw std::invalid_argument("Null pointer");
  }

  while (std::isspace(static_cast< unsigned char >(*str)))
  {
    ++str;
  }

  if (*str == '-')
  {
    throw std::invalid_argument("Negative number");
  }

  char *end = nullptr;
  errno = 0;
  const unsigned long long res = std::strtoull(str, &end, 10);

  if (end == str)
  {
    throw std::invalid_argument("Not a number");
  }

  while (std::isspace(static_cast< unsigned char >(*end)))
  {
    ++end;
  }

  if (*end != '\0')
  {
    throw std::invalid_argument("Not a number");
  }

  if (errno == ERANGE || res > std::numeric_limits< std::size_t >::max())
  {
    throw std::invalid_argument("Overflow");
  }

  return static_cast< std::size_t >(res);
}
