#include "descriptor.hpp"
#include <cerrno>
#include <cstddef>
#include <stdexcept>
#include <system_error>
#include <utility>
#include <sys/poll.h>
#include <sys/types.h>
#include <unistd.h>

namespace
{
  const int no_descriptor = -1;

  std::system_error makeSystemError(const char* what)
  {
    return std::system_error(errno, std::generic_category(), what);
  }
}

pozdnyakov::FileDescriptor::FileDescriptor() noexcept:
  descriptor_(no_descriptor)
{}

pozdnyakov::FileDescriptor::FileDescriptor(FileDescriptor&& rhs) noexcept:
  descriptor_(rhs.descriptor_)
{
  rhs.descriptor_ = no_descriptor;
}

pozdnyakov::FileDescriptor::FileDescriptor(int descriptor) noexcept:
  descriptor_(descriptor)
{}

pozdnyakov::FileDescriptor::~FileDescriptor()
{
  if (descriptor_ != no_descriptor)
  {
    ::close(descriptor_);
  }
}

pozdnyakov::FileDescriptor& pozdnyakov::FileDescriptor::operator=(FileDescriptor&& rhs) noexcept
{
  FileDescriptor temporary(std::move(rhs));
  std::swap(descriptor_, temporary.descriptor_);
  return *this;
}

int pozdnyakov::FileDescriptor::get() const noexcept
{
  return descriptor_;
}

pozdnyakov::pipe_ends_t pozdnyakov::makePipe()
{
  const std::size_t ends_count = 2;
  int ends[ends_count] = {no_descriptor, no_descriptor};
  if (::pipe(ends) == -1)
  {
    throw makeSystemError("pipe");
  }
  return {FileDescriptor(ends[0]), FileDescriptor(ends[1])};
}

void pozdnyakov::writeAll(const FileDescriptor& descriptor, const void* data, std::size_t size)
{
  const char* position = static_cast< const char* >(data);
  while (size > 0)
  {
    const ssize_t written = ::write(descriptor.get(), position, size);
    if (written == -1)
    {
      if (errno == EINTR)
      {
        continue;
      }
      throw makeSystemError("write");
    }
    position += written;
    size -= static_cast< std::size_t >(written);
  }
}

bool pozdnyakov::readAll(const FileDescriptor& descriptor, void* data, std::size_t size)
{
  char* position = static_cast< char* >(data);
  bool started = false;
  while (size > 0)
  {
    const ssize_t received = ::read(descriptor.get(), position, size);
    if (received == -1)
    {
      if (errno == EINTR)
      {
        continue;
      }
      throw makeSystemError("read");
    }
    if (received == 0)
    {
      if (started)
      {
        throw std::runtime_error("unexpected end of pipe");
      }
      return false;
    }
    started = true;
    position += received;
    size -= static_cast< std::size_t >(received);
  }
  return true;
}

bool pozdnyakov::waitReadable(const FileDescriptor& descriptor, int milliseconds)
{
  pollfd request{descriptor.get(), POLLIN, 0};
  int ready = ::poll(&request, 1, milliseconds);
  while ((ready == -1) && (errno == EINTR))
  {
    ready = ::poll(&request, 1, milliseconds);
  }
  if (ready == -1)
  {
    throw makeSystemError("poll");
  }
  return ready > 0;
}
