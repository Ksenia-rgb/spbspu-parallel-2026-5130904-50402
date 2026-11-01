#include "ThreadRAII.hpp"
#include <memory>

shirokov::ThreadRAII::ThreadRAII(std::thread th):
  th(std::move(th))
{}

shirokov::ThreadRAII::ThreadRAII(ThreadRAII&& other):
  th(std::move(other.th))
{}

shirokov::ThreadRAII& shirokov::ThreadRAII::operator=(ThreadRAII&& other)
{
  if (this != std::addressof(other))
  {
    std::swap(this->th, other.th);
  }
  return *this;
}

shirokov::ThreadRAII::~ThreadRAII()
{
  if (th.joinable())
  {
    th.join();
  }
}
