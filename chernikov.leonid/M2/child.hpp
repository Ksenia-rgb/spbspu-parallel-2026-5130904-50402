#ifndef CHILD_HPP
#define CHILD_HPP

#include <cstddef>

namespace chernikov {
  void runChild(int request_fd, int result_fd, size_t seed);
}
#endif
