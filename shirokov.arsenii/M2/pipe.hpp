#ifndef PIPE_HPP
#define PIPE_HPP

namespace shirokov
{
  struct UniqueFd
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
  private:
    int fd_ = -1;
  };

  struct Pipe
  {
    UniqueFd read;
    UniqueFd write;

    Pipe();
  };
}

#endif
