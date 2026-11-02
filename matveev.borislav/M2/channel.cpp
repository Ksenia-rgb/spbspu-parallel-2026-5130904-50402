#include "channel.hpp"
#include <cerrno>
#include <stdexcept>
#include <algorithm>
#include <sys/socket.h>

bool matveev::readExact(const int descriptor, void *data, const std::size_t size)
{
  char *bytes = static_cast< char * >(data);
  std::size_t position = 0;
  constexpr std::size_t blockSize = 65536;
  while (position < size)
  {
    const std::size_t count = std::min(blockSize, size - position);
    const ssize_t received = ::recv(descriptor, bytes + position, count, 0);
    if (received < 0)
    {
      if (errno == EINTR)
      {
        continue;
      }
      throw std::runtime_error("Channel read failed");
    }
    if (received == 0)
    {
      if (position != 0)
      {
        throw std::runtime_error("Incomplete message");
      }
      return false;
    }
    position += static_cast< std::size_t >(received);
  }
  return true;
}

void matveev::writeExact(const int descriptor, const void *data, const std::size_t size)
{
  const char *bytes = static_cast< const char * >(data);
  std::size_t position = 0;
  constexpr std::size_t blockSize = 65536;
  while (position < size)
  {
    const std::size_t count = std::min(blockSize, size - position);
    const ssize_t sent = ::send(descriptor, bytes + position, count, MSG_NOSIGNAL);
    if ((sent < 0) && (errno == EINTR))
    {
      continue;
    }
    if (sent <= 0)
    {
      throw std::runtime_error("Channel write failed");
    }
    position += static_cast< std::size_t >(sent);
  }
}
