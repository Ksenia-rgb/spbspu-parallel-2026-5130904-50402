#ifndef PIPE_HPP
#define PIPE_HPP

#include <sys/types.h>

namespace shirokov
{
  class UniqueFd
  {
  public:
    constexpr UniqueFd() noexcept = default;
    explicit UniqueFd(int fd) noexcept;

    UniqueFd(const UniqueFd&) = delete;
    UniqueFd(UniqueFd&&) noexcept;

    UniqueFd& operator=(const UniqueFd&) = delete;
    UniqueFd& operator=(UniqueFd&&) noexcept;

    ~UniqueFd();

    int get() const noexcept;
    void reset() noexcept;
  private:
    int fd_ = -1;
  };

  struct Pipe
  {
    UniqueFd read;
    UniqueFd write;

    Pipe();
  };

  class DuplexChannel
  {
  public:
    DuplexChannel(UniqueFd read, UniqueFd write) noexcept;
    ssize_t write(const void* b, ssize_t k);
    ssize_t read(void* b, ssize_t k);
    int getReadFd() const noexcept;

  private:
    UniqueFd read_fd_, write_fd_;
  };

  class PipeBridge
  {
  public:
    PipeBridge();
    DuplexChannel asParent() &&;
    DuplexChannel asChild() &&;
  private:
    UniqueFd p2c_read_;
    UniqueFd p2c_write_;
    UniqueFd c2p_read_;
    UniqueFd c2p_write_;
  };
}

#endif
