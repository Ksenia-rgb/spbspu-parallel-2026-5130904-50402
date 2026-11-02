#include "parse.hpp"
#include <stdexcept>
#include <cmath>

matveev::Tokens matveev::splitLine(const std::string &line)
{
  Tokens tokens = {};
  std::size_t begin = line.find_first_not_of(" \t\r\n\v\f");
  while (begin != std::string::npos)
  {
    const std::size_t end = line.find_first_of(" \t\r\n\v\f", begin);
    tokens.push_back(line.substr(begin, end - begin));
    begin = line.find_first_not_of(" \t\r\n\v\f", end);
  }
  return tokens;
}

long long matveev::parseInteger(const std::string &text)
{
  std::size_t position = 0;
  const long long value = std::stoll(text, &position);
  if (position != text.size())
  {
    throw std::invalid_argument("Invalid integer");
  }
  return value;
}

std::uint64_t matveev::parseUnsigned(const std::string &text)
{
  if (text.empty() || (text.front() == '-'))
  {
    throw std::invalid_argument("Expected a nonnegative integer");
  }
  std::size_t position = 0;
  const unsigned long long value = std::stoull(text, &position);
  if (position != text.size())
  {
    throw std::invalid_argument("Invalid integer");
  }
  return value;
}

double matveev::parseReal(const std::string &text)
{
  std::size_t position = 0;
  const double value = std::stod(text, &position);
  if ((position != text.size()) || !std::isfinite(value))
  {
    throw std::invalid_argument("Invalid real number");
  }
  return value;
}

void matveev::requireName(const std::string &name)
{
  if (name.empty())
  {
    throw std::invalid_argument("Empty name");
  }
  const char first = name.front();
  const bool lower = (first >= 'a') && (first <= 'z');
  const bool upper = (first >= 'A') && (first <= 'Z');
  if (!lower && !upper)
  {
    throw std::invalid_argument("Invalid name");
  }
}

void matveev::requireCount(const Tokens &tokens, const std::size_t count)
{
  if (tokens.size() != count)
  {
    throw std::invalid_argument("Invalid argument count");
  }
}
