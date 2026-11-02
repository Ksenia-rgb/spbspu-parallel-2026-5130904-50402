#include "process.hpp"
#include <stdexcept>
#include <cerrno>
#include <csignal>
#include <algorithm>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>

matveev::Process::Process(const std::uint64_t seed, const std::vector< int > &inherited)
{
  constexpr std::size_t channelEnds = 2;
  constexpr std::size_t childEnd = 1;
  int sockets[channelEnds] = {invalidDescriptor, invalidDescriptor};
  if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0)
  {
    throw std::runtime_error("Cannot create channel");
  }
  Descriptor parent(sockets[0]);
  Descriptor child(sockets[childEnd]);
  makeNonblocking(parent.get());
  const pid_t pid = ::fork();
  if (pid < 0)
  {
    throw std::runtime_error("Cannot create process");
  }
  if (pid == 0)
  {
    parent.reset();
    for (const int descriptor: inherited)
    {
      if (descriptor >= 0)
      {
        ::close(descriptor);
      }
    }
    ::close(STDIN_FILENO);
    ::close(STDOUT_FILENO);
    const int status = runWorker(child.get(), seed);
    child.reset();
    ::_exit(status);
  }
  pid_ = pid;
  channel_.reset(parent.release());
}

matveev::Process::~Process()
{
  stop(true);
}

int matveev::Process::getDescriptor() const noexcept
{
  return channel_.get();
}

void matveev::Process::stop(const bool force) noexcept
{
  channel_.reset();
  if (pid_ > 0)
  {
    if (force)
    {
      ::kill(pid_, SIGKILL);
    }
    while ((::waitpid(pid_, nullptr, 0) < 0) && (errno == EINTR))
    {}
    pid_ = 0;
  }
}

bool matveev::Process::hasOutput() const noexcept
{
  return sent_ < outgoing_.size();
}

void matveev::Process::start(std::vector< char > &bytes) noexcept
{
  outgoing_.swap(bytes);
  sent_ = 0;
  response_ = {};
  received_ = 0;
}

void matveev::Process::sendPending()
{
  constexpr std::size_t blockSize = 65536;
  while (hasOutput())
  {
    const std::size_t count = std::min(blockSize, outgoing_.size() - sent_);
    const ssize_t sent = ::send(channel_.get(), outgoing_.data() + sent_, count, MSG_NOSIGNAL);
    if ((sent < 0) && (errno == EINTR))
    {
      continue;
    }
    if ((sent < 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
    {
      return;
    }
    if (sent <= 0)
    {
      throw std::runtime_error("Cannot send calculation");
    }
    sent_ += static_cast< std::size_t >(sent);
  }
}

bool matveev::Process::receive(response_t &response)
{
  char *const bytes = reinterpret_cast< char * >(&response_);
  while (received_ < sizeof(response_))
  {
    const ssize_t count = ::recv(channel_.get(), bytes + received_, sizeof(response_) - received_, 0);
    if ((count < 0) && (errno == EINTR))
    {
      continue;
    }
    if ((count < 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
    {
      return false;
    }
    if (count <= 0)
    {
      throw std::runtime_error("Worker disconnected");
    }
    received_ += static_cast< std::size_t >(count);
  }
  response = response_;
  received_ = 0;
  return true;
}
