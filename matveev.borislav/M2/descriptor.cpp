#include "descriptor.hpp"
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>

matveev::Descriptor::Descriptor(const int value) noexcept:
  value_(value)
{}

matveev::Descriptor::~Descriptor()
{
  reset();
}

int matveev::Descriptor::get() const noexcept
{
  return value_;
}

int matveev::Descriptor::release() noexcept
{
  const int result = value_;
  value_ = invalidDescriptor;
  return result;
}

void matveev::Descriptor::reset(const int value) noexcept
{
  if (value_ >= 0)
  {
    ::close(value_);
  }
  value_ = value;
}

void matveev::makeNonblocking(const int descriptor)
{
  const int flags = ::fcntl(descriptor, F_GETFL);
  if ((flags < 0) || (::fcntl(descriptor, F_SETFL, flags | O_NONBLOCK) < 0))
  {
    throw std::runtime_error("Cannot configure descriptor");
  }
}
