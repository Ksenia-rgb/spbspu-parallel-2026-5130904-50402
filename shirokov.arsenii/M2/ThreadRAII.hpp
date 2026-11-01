#ifndef THREAD_RAII_HPP
#define THREAD_RAII_HPP

#include <thread>

namespace shirokov
{
  struct ThreadRAII
  {
    std::thread th;

    ThreadRAII() = delete;
    ThreadRAII(std::thread);
    ThreadRAII(const ThreadRAII&) = delete;
    ThreadRAII(ThreadRAII&&);

    ThreadRAII& operator=(const ThreadRAII&) = delete;
    ThreadRAII& operator=(ThreadRAII&&);

    ~ThreadRAII();
  };
}

#endif
