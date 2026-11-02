#ifndef MATVEEV_PARSE_HPP
#define MATVEEV_PARSE_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>

namespace matveev
{
  using Tokens = std::vector< std::string >;
  Tokens splitLine(const std::string &line);
  long long parseInteger(const std::string &text);
  std::uint64_t parseUnsigned(const std::string &text);
  double parseReal(const std::string &text);
  void requireName(const std::string &name);
  void requireCount(const Tokens &tokens, std::size_t count);
}

#endif
