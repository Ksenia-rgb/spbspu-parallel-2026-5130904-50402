#include "parse.hpp"
#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <stdexcept>
#include <string>

namespace
{
  bool isSpace(char symbol)
  {
    return std::isspace(static_cast< unsigned char >(symbol)) != 0;
  }
}

pozdnyakov::words_t pozdnyakov::splitWords(const std::string& line)
{
  words_t words;
  std::string::const_iterator begin = std::find_if_not(line.cbegin(), line.cend(), isSpace);
  while (begin != line.cend())
  {
    const std::string::const_iterator end = std::find_if(begin, line.cend(), isSpace);
    words.emplace_back(begin, end);
    begin = std::find_if_not(end, line.cend(), isSpace);
  }
  return words;
}

long long pozdnyakov::parseInteger(const std::string& text)
{
  const int base = 10;
  const char* const begin = text.c_str();
  char* end = nullptr;
  errno = 0;
  const long long value = std::strtoll(begin, &end, base);
  if (text.empty() || (*end != '\0') || (errno == ERANGE))
  {
    throw std::invalid_argument("not an integer: " + text);
  }
  return value;
}

std::size_t pozdnyakov::parseSize(const std::string& text)
{
  const long long value = parseInteger(text);
  if (value < 0)
  {
    throw std::invalid_argument("negative value: " + text);
  }
  return static_cast< std::size_t >(value);
}
