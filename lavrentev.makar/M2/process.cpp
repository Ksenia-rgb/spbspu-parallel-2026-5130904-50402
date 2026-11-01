#include "process.hpp"

#include <poll.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "child.hpp"

namespace
{
  const int FIRST_USER_FD = 3;
  const int MAX_FD_TO_CLOSE = 1024;

  struct CircleData
  {
    int r;
    int x;
    int y;
  };

  bool writeAll(int fd, const void *data, std::size_t size)
  {
    const char *ptr = static_cast< const char * >(data);
    while (size > 0)
    {
      const ssize_t written = write(fd, ptr, size);
      if (written <= 0)
      {
        return false;
      }
      ptr += written;
      size -= static_cast< std::size_t >(written);
    }
    return true;
  }

  bool readAll(int fd, void *data, std::size_t size)
  {
    char *ptr = static_cast< char * >(data);
    while (size > 0)
    {
      const ssize_t res = read(fd, ptr, size);
      if (res <= 0)
      {
        return false;
      }
      ptr += res;
      size -= static_cast< std::size_t >(res);
    }
    return true;
  }
}

lavrentev::Process::Process(const std::string &process_name):
  process_name_(process_name),
  pending_calcs_(),
  seed_(0),
  calc_results_(),
  pid_(-1),
  in_fd_(-1),
  out_fd_(-1)
{
}

lavrentev::Process::Process(Process &&other) noexcept:
  process_name_(std::move(other.process_name_)),
  pending_calcs_(std::move(other.pending_calcs_)),
  seed_(other.seed_),
  calc_results_(std::move(other.calc_results_)),
  pid_(std::exchange(other.pid_, -1)),
  in_fd_(std::exchange(other.in_fd_, -1)),
  out_fd_(std::exchange(other.out_fd_, -1))
{
}

lavrentev::Process &lavrentev::Process::operator=(Process &&other) noexcept
{
  if (this != &other)
  {
    cleanup();
    process_name_ = std::move(other.process_name_);
    pending_calcs_ = std::move(other.pending_calcs_);
    seed_ = other.seed_;
    calc_results_ = std::move(other.calc_results_);
    pid_ = std::exchange(other.pid_, -1);
    in_fd_ = std::exchange(other.in_fd_, -1);
    out_fd_ = std::exchange(other.out_fd_, -1);
  }
  return *this;
}

lavrentev::Process::~Process()
{
  cleanup();
}

double lavrentev::Process::getResult()
{
  if ((out_fd_ == -1) || pending_calcs_.empty())
  {
    return 0.0;
  }

  double val = 0.0;
  double time = 0.0;

  const std::string finished_calc = pending_calcs_.front();
  pending_calcs_.pop();

  if (readAll(out_fd_, &val, sizeof(val)) && readAll(out_fd_, &time, sizeof(time)))
  {
    calc_results_[finished_calc] = CalcRes{process_name_, val, time};
  }

  return val;
}

void lavrentev::Process::waitFor(const std::string &calc_name, calc_map &global_cm)
{
  while (!pending_calcs_.empty() && (calc_results_[calc_name].time == 0.0))
  {
    const std::string next_calc = pending_calcs_.front();
    getResult();
    global_cm[next_calc] = calc_results_[next_calc];
    if (next_calc == calc_name)
    {
      break;
    }
  }
}

void lavrentev::Process::pollResults(calc_map &global_cm)
{
  if (out_fd_ == -1)
  {
    return;
  }

  pollfd pfd{};
  pfd.fd = out_fd_;
  pfd.events = POLLIN;

  const int timeout_ms = 0;
  while (!pending_calcs_.empty() && (poll(&pfd, 1, timeout_ms) > 0) && ((pfd.revents & POLLIN) != 0))
  {
    const std::string next_calc = pending_calcs_.front();
    getResult();
    global_cm[next_calc] = calc_results_[next_calc];
  }
}

void lavrentev::Process::setSeed(int seed)
{
  seed_ = seed;
}

bool lavrentev::Process::isAlive() const
{
  return (pid_ > 0) && (in_fd_ != -1);
}

bool lavrentev::Process::at(const std::string &calc_name) const
{
  return calc_results_.find(calc_name) != calc_results_.end();
}

const lavrentev::CalcRes &lavrentev::Process::getCalc(const std::string &calc_name) const
{
  return calc_results_.at(calc_name);
}

void lavrentev::Process::cleanup()
{
  if (in_fd_ != -1)
  {
    close(in_fd_);
    in_fd_ = -1;
  }

  if (out_fd_ != -1)
  {
    close(out_fd_);
    out_fd_ = -1;
  }

  if (pid_ > 0)
  {
    waitpid(pid_, nullptr, 0);
    pid_ = -1;
  }
}

void lavrentev::Process::start(
  std::vector< std::shared_ptr< Circle > > &circles,
  const std::string &calc_name,
  std::size_t threads,
  std::size_t tries
)
{
  pending_calcs_.push(calc_name);
  calc_results_[calc_name] = CalcRes{process_name_, 0.0, 0.0};

  if (!isAlive())
  {
    int p_in[2] = {};
    int p_out[2] = {};

    if (pipe(p_in) == -1)
    {
      throw std::runtime_error("Parent: failed to create in-pipe");
    }
    if (pipe(p_out) == -1)
    {
      close(p_in[0]);
      close(p_in[1]);
      throw std::runtime_error("Parent: failed to create out-pipe");
    }

    pid_ = fork();
    if (pid_ == -1)
    {
      close(p_in[0]);
      close(p_in[1]);
      close(p_out[0]);
      close(p_out[1]);
      throw std::runtime_error("Fork error");
    }

    if (pid_ == 0)
    {
      close(p_in[1]);
      close(p_out[0]);

      for (int fd = FIRST_USER_FD; fd < MAX_FD_TO_CLOSE; ++fd)
      {
        if ((fd != p_in[0]) && (fd != p_out[1]))
        {
          close(fd);
        }
      }

      const int exit_code = runChild(p_in[0], p_out[1]);
      _exit(exit_code);
    }

    close(p_in[0]);
    close(p_out[1]);
    in_fd_ = p_in[1];
    out_fd_ = p_out[0];
  }

  const std::size_t str_len = calc_name.size();
  writeAll(in_fd_, &str_len, sizeof(str_len));
  if (str_len > 0)
  {
    writeAll(in_fd_, calc_name.data(), str_len);
  }

  writeAll(in_fd_, &threads, sizeof(threads));
  writeAll(in_fd_, &tries, sizeof(tries));
  writeAll(in_fd_, &seed_, sizeof(seed_));

  const std::size_t circles_count = circles.size();
  writeAll(in_fd_, &circles_count, sizeof(circles_count));

  if (circles_count > 0)
  {
    std::vector< CircleData > raw_data;
    raw_data.reserve(circles_count);
    for (const auto &c : circles)
    {
      if (c)
      {
        raw_data.push_back({c->getRadius(), c->getX(), c->getY()});
      }
    }
    writeAll(in_fd_, raw_data.data(), raw_data.size() * sizeof(CircleData));
  }
}
