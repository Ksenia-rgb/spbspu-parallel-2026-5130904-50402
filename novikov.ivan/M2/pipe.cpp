#include "pipe.hpp"
#include <memory>
#include <unistd.h>
#include <utility>
#include <system_error>

novikov::UniqueFd::UniqueFd(int fd) noexcept:
  fd_(fd)
{}

novikov::UniqueFd::UniqueFd(UniqueFd&& other) noexcept:
  fd_(other.fd_)
{
  other.fd_ = -1;
}

novikov::UniqueFd& novikov::UniqueFd::operator=(UniqueFd&& other) noexcept
{
  if (this != std::addressof(other))
  {
    std::swap(fd_, other.fd_);
  }
  return *this;
}

novikov::UniqueFd::~UniqueFd()
{
  if (fd_ != -1)
  {
    ::close(fd_);
  }
}

int novikov::UniqueFd::get() const noexcept
{
  return fd_;
}

void novikov::UniqueFd::reset() noexcept
{
  if (fd_ != -1)
  {
    ::close(fd_);
  }
  fd_ = -1;
}

novikov::Pipe::Pipe()
{
  int pps[2] = {}, err = ::pipe(pps);

  if (err)
  {
    throw std::system_error(errno, std::generic_category(), "pipe error");
  }

  read = UniqueFd(pps[0]);
  write = UniqueFd(pps[1]);
}

novikov::DuplexChannel::DuplexChannel(UniqueFd read, UniqueFd write) noexcept:
  read_fd_(std::move(read)),
  write_fd_(std::move(write))
{}

ssize_t novikov::DuplexChannel::write(const void* b, ssize_t k)
{
  ssize_t r = 0;
  const char* const bytes = static_cast< const char* >(b);
  while (r < k)
  {
    const ssize_t err = ::write(write_fd_.get(), bytes + r, static_cast< size_t >(k - r));
    if (err <= 0)
    {
      throw std::system_error(errno, std::generic_category(), "pip write error");
    }
    r += err;
  }
  return r;
}

ssize_t novikov::DuplexChannel::read(void* b, ssize_t k)
{
  ssize_t r = 0;
  char* const bytes = static_cast< char* >(b);
  while (r < k)
  {
    const ssize_t err = ::read(read_fd_.get(), bytes + r, static_cast< size_t >(k - r));
    if (err <= 0)
    {
      throw std::system_error(errno, std::generic_category(), "pip read error");
    }
    r += err;
  }
  return r;
}

int novikov::DuplexChannel::getReadFd() const noexcept
{
  return read_fd_.get();
}

novikov::PipeBridge::PipeBridge()
{
  Pipe p2c{}, c2p{};

  p2c_read_ = std::move(p2c.read);
  p2c_write_ = std::move(p2c.write);
  c2p_read_ = std::move(c2p.read);
  c2p_write_ = std::move(c2p.write);
}

novikov::DuplexChannel novikov::PipeBridge::asParent() &&
{
  p2c_read_.reset();
  c2p_write_.reset();
  return DuplexChannel(std::move(c2p_read_), std::move(p2c_write_));
}

novikov::DuplexChannel novikov::PipeBridge::asChild() &&
{
  p2c_write_.reset();
  c2p_read_.reset();
  return DuplexChannel(std::move(p2c_read_), std::move(c2p_write_));
}
