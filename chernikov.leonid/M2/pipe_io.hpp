#ifndef PIPE_IO_HPP
#define PIPE_IO_HPP

#include <cstddef>

namespace chernikov {
  // TODO: перенести из задачи с пайпами (циклы write/read до конца)
  bool writeAll(int fd, const void *buf, size_t size);
  bool readAll(int fd, void *buf, size_t size); // false при EOF или ошибке
}
#endif