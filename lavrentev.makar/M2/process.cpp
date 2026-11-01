#include "process.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <poll.h>
#include <stdexcept>
#include <utility>
#include <string>
#include <vector>
#include <memory>
#include "child.hpp"

namespace
{
  bool write_all(int fd, const void* data, size_t size)
  {
    const char* ptr = static_cast<const char*>(data);
    while (size > 0)
    {
      ssize_t written = write(fd, ptr, size);
      if (written <= 0)
      {
        return false;
      }
      ptr += written;
      size -= written;
    }
    return true;
  }

  bool read_all(int fd, void* data, size_t size)
  {
    char* ptr = static_cast<char*>(data);
    while (size > 0)
    {
      ssize_t res = read(fd, ptr, size);
      if (res <= 0)
      {
        return false;
      }
      ptr += res;
      size -= res;
    }
    return true;
  }

  struct CircleData
  {
    int r;
    int x;
    int y;
  };
}

lavrentev::Process::Process(std::string process_name):
  process_name_(std::move(process_name)),
  pending_calcs_(),
  seed_(0),
  calc_results_(),
  pid_(-1),
  in_fd_(-1),
  out_fd_(-1)
{}

lavrentev::Process::Process(Process&& other) noexcept:
  process_name_(std::move(other.process_name_)),
  pending_calcs_(std::move(other.pending_calcs_)),
  seed_(other.seed_),
  calc_results_(std::move(other.calc_results_)),
  pid_(std::exchange(other.pid_, -1)),
  in_fd_(std::exchange(other.in_fd_, -1)),
  out_fd_(std::exchange(other.out_fd_, -1))
{}

lavrentev::Process& lavrentev::Process::operator=(Process&& other) noexcept
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

double lavrentev::Process::get_result()
{
  if (out_fd_ == -1 || pending_calcs_.empty())
  {
    return 0.0;
  }

  double val = 0.0;
  double time = 0.0;

  std::string finished_calc = pending_calcs_.front();
  pending_calcs_.pop();

  if (read_all(out_fd_, &val, sizeof(val)) && read_all(out_fd_, &time, sizeof(time)))
  {
    calc_results_[finished_calc] = CalcRes{process_name_, val, time};
  }

  return val;
}

void lavrentev::Process::wait_for(const std::string& calc_name, calc_map& global_cm)
{
  while (!pending_calcs_.empty() && calc_results_[calc_name].time == 0.0)
  {
    std::string next_calc = pending_calcs_.front();
    get_result();
    global_cm[next_calc] = calc_results_[next_calc];
    if (next_calc == calc_name)
    {
      break;
    }
  }
}

void lavrentev::Process::poll_results(calc_map& global_cm)
{
  if (out_fd_ == -1)
  {
    return;
  }

  struct pollfd pfd{};
  pfd.fd = out_fd_;
  pfd.events = POLLIN;

  while (!pending_calcs_.empty() && poll(&pfd, 1, 0) > 0 && (pfd.revents & POLLIN))
  {
    std::string next_calc = pending_calcs_.front();
    get_result();
    global_cm[next_calc] = calc_results_[next_calc];
  }
}

void lavrentev::Process::set_seed(int seed)
{
  seed_ = seed;
}

bool lavrentev::Process::is_alive() const
{
  return pid_ > 0 && in_fd_ != -1;
}

bool lavrentev::Process::at(const std::string& calc_name) const
{
  return calc_results_.find(calc_name) != calc_results_.end();
}

const lavrentev::CalcRes& lavrentev::Process::get_calc(const std::string& calc_name) const
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

void lavrentev::Process::start(std::vector<std::shared_ptr<Circle>> &circles, std::string calc_name, size_t threads, size_t tries)
{
  pending_calcs_.push(calc_name);
  calc_results_[calc_name] = CalcRes{process_name_, 0.0, 0.0};

  if (!is_alive())
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

      for (int fd = 3; fd < 1024; ++fd)
      {
        if (fd != p_in[0] && fd != p_out[1])
        {
          close(fd);
        }
      }

      int exit_code = run_child(p_in[0], p_out[1]);
      _exit(exit_code);
    }

    close(p_in[0]);
    close(p_out[1]);
    in_fd_ = p_in[1];
    out_fd_ = p_out[0];
  }

  size_t str_len = calc_name.size();
  write_all(in_fd_, &str_len, sizeof(str_len));
  if (str_len > 0)
  {
    write_all(in_fd_, calc_name.data(), str_len);
  }

  write_all(in_fd_, &threads, sizeof(threads));
  write_all(in_fd_, &tries, sizeof(tries));
  write_all(in_fd_, &seed_, sizeof(seed_));

  size_t circles_count = circles.size();
  write_all(in_fd_, &circles_count, sizeof(circles_count));

  if (circles_count > 0)
  {
    std::vector<CircleData> raw_data;
    raw_data.reserve(circles_count);
    for (const auto& c : circles)
    {
      if (c)
      {
        raw_data.push_back({c->getRadius(), c->getX(), c->getY()});
      }
    }
    write_all(in_fd_, raw_data.data(), raw_data.size() * sizeof(CircleData));
  }
}
