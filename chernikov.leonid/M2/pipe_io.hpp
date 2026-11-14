#ifndef PIPE_IO_HPP
#define PIPE_IO_HPP

#include <cstddef>

namespace chernikov {
  bool writeAll(int fd, const void *buf, size_t size);
  bool readAll(int fd, void *buf, size_t size);
}
#endif
