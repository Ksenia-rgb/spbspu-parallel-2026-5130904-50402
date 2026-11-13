#ifndef CHILD_HPP
#define CHILD_HPP

#include <cstddef>

namespace chernikov {
  // TODO: child.cpp — цикл receiveRequest -> makeShape -> coverArea (с замером времени) -> sendResult до EOF
  void runChild(int request_fd, int result_fd, size_t seed);
}
#endif