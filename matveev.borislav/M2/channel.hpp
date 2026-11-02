#ifndef MATVEEV_CHANNEL_HPP
#define MATVEEV_CHANNEL_HPP

#include <cstddef>

namespace matveev
{
  bool readExact(int descriptor, void *data, std::size_t size);
  void writeExact(int descriptor, const void *data, std::size_t size);
}

#endif
