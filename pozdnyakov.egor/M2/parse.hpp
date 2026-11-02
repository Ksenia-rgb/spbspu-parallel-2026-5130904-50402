#ifndef PARSE_HPP
#define PARSE_HPP

#include <cstddef>
#include <string>
#include <vector>

namespace pozdnyakov
{
  using words_t = std::vector< std::string >;

  words_t splitWords(const std::string& line);
  long long parseInteger(const std::string& text);
  std::size_t parseSize(const std::string& text);
}

#endif
