#include "child.hpp"
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

namespace
{
  double calculate(int seed)
  {
    return seed * 2.0;
  }

  bool write_all(int fd, const void* data, size_t size)
  {
    const char* ptr = static_cast<const char*>(data);
    while (size > 0)
    {
      ssize_t written = write(fd, ptr, size);
      if (written <= 0)
      {
        return false;
      }
      ptr += written;
      size -= written;
    }
    return true;
  }
}

int run_child(int read_fd, int write_fd)
{
  int seed = 0;

  while (true)
  {
    ssize_t bytes = read(read_fd, &seed, sizeof(seed));
  
    if (bytes <= 0)
    {
      break; 
    }

    double result = calculate(seed);
    if (!write_all(write_fd, &result, sizeof(result)))
    {
      break;
    }
  }

  close(read_fd);
  close(write_fd);

  return 0;
}
