#include "pipe_io.hpp"
#include <cerrno>
#include <cstddef>
#include <sys/types.h>
#include <unistd.h>

bool chernikov::writeAll(int fd, const void *buf, size_t size)
{
  const char *const data = static_cast< const char * >(buf);
  size_t done = 0;
  while (done < size) {
    const ssize_t n = ::write(fd, data + done, size - done);
    if (n < 0) {
      if (errno == EINTR) {
        continue;
      }
      return false;
    }
    done += static_cast< size_t >(n);
  }
  return true;
}

bool chernikov::readAll(int fd, void *buf, size_t size)
{
  char *const data = static_cast< char * >(buf);
  size_t done = 0;
  while (done < size) {
    const ssize_t n = ::read(fd, data + done, size - done);
    if (n < 0) {
      if (errno == EINTR) {
        continue;
      }
      return false;
    }
    if (n == 0) {
      return false;
    }
    done += static_cast< size_t >(n);
  }
  return true;
}
