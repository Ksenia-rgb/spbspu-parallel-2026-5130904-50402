#include "process.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <stdexcept>
#include <utility>

lavrentev::Process::Process(std::string process_name):
  process_name_(process_name),
  calc_name_(""),
  seed_(0),
  result_(-1.0),
  pid_(-1),
  in_fd_(-1),
  out_fd_(-1)
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

  pid_t pid = fork();
  if (pid == -1)
  {
    perror("Parent: fork error");
    close(p_in[0]);
    close(p_in[1]);
    close(p_out[0]);
    close(p_out[1]);
    throw std::runtime_error("Parent: fork error");
  }

  if (pid_ == 0)
  {
    close(p_in[1]);
    close(p_out[0]);

    char in_arg[16];
    char out_arg[16];
    snprintf(in_arg, sizeof(in_arg), "%d", p_in[0]);
    snprintf(out_arg, sizeof(out_arg), "%d", p_out[1]);
    execl(process_name_.c_str(), process_name_.c_str(), in_arg, out_arg, nullptr);

    close(p_in[0]);
    close(p_out[1]);
    _exit(127);
  }

  close(p_in[0]);
  close(p_out[1]);
  out_fd_ = p_in[1];
  in_fd_ = p_out[0];
}

lavrentev::Process::Process(Process&& other) noexcept:
  process_name_(std::move(other.process_name_)),
  calc_name_(std::move(other.calc_name_)),
  seed_(other.seed_),
  result_(other.result_),
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
    calc_name_ = std::move(other.calc_name_);
    seed_ = other.seed_;
    result_ = other.result_;

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
  if (out_fd_ == -1)
  {
    return result_;
  }

  double val = 0.0;
  ssize_t bytes_read = read(out_fd_, &val, sizeof(val));
  if (bytes_read == sizeof(val))
  {
    result_ = val;
  }
  return result_;
}

void lavrentev::Process::set_seed(int seed)
{
  seed_ = seed;
  if (in_fd_ != -1)
  {
    write(in_fd_, &seed_, sizeof(seed_));
  }
}

bool lavrentev::Process::is_alive() const
{
  return pid_ > 0 && in_fd_ != -1;
}

void lavrentev::Process::cleanup()
{
  if (in_fd_ != -1)
  {
    close(in_fd_);
    in_fd_ = -1;
  }

  if (out_fd_ != -1) {
    close(out_fd_);
    out_fd_ = -1;
  }

  if (pid_ > 0)
  {
    waitpid(pid_, nullptr, 0);
    pid_ = -1;
  }
}
