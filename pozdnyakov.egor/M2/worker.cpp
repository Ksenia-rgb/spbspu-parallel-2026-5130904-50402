#include "worker.hpp"
#include <cerrno>
#include <chrono>
#include <cstddef>
#include <exception>
#include <iostream>
#include <random>
#include <ratio>
#include <stdexcept>
#include <system_error>
#include <utility>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "calculation.hpp"
#include "descriptor.hpp"
#include "monte-carlo.hpp"
#include "shapes.hpp"

namespace
{
  const pid_t no_process = -1;

  struct request_header_t
  {
    std::size_t threads;
    std::size_t tries;
    std::size_t count;
  };

  using engine_t = std::mt19937_64;
  using milliseconds_t = std::chrono::duration< double, std::milli >;
  using steady_t = std::chrono::steady_clock;

  double computeArea(const pozdnyakov::calculation_request_t& request, std::size_t seed)
  {
    return pozdnyakov::computeCoverage(request.ellipses, request.threads, request.tries, seed);
  }

  bool readRequest(const pozdnyakov::FileDescriptor& requests, pozdnyakov::calculation_request_t& request)
  {
    request_header_t header{0, 0, 0};
    if (!pozdnyakov::readAll(requests, &header, sizeof(header)))
    {
      return false;
    }
    pozdnyakov::ellipses_t ellipses(header.count, pozdnyakov::ellipse_t{{0.0, 0.0}, 0.0, 0.0});
    if (!pozdnyakov::readAll(requests, ellipses.data(), ellipses.size() * sizeof(pozdnyakov::ellipse_t)))
    {
      throw std::runtime_error("request without shapes");
    }
    request = {header.threads, header.tries, std::move(ellipses)};
    return true;
  }

  void serveRequests(const pozdnyakov::FileDescriptor& in, const pozdnyakov::FileDescriptor& out, std::size_t seed)
  {
    engine_t engine(seed);
    pozdnyakov::calculation_request_t request{0, 0, {}};
    while (readRequest(in, request))
    {
      const std::size_t calculation_seed = engine();
      const steady_t::time_point start = steady_t::now();
      const double area = computeArea(request, calculation_seed);
      const milliseconds_t duration = steady_t::now() - start;
      const pozdnyakov::calculation_result_t result{duration.count(), area};
      pozdnyakov::writeAll(out, &result, sizeof(result));
    }
  }

  void runChild(const pozdnyakov::FileDescriptor& requests, const pozdnyakov::FileDescriptor& results, std::size_t seed)
  {
    const int failure_code = 2;
    try
    {
      serveRequests(requests, results, seed);
    }
    catch (const std::exception& error)
    {
      std::cerr << error.what() << '\n';
      ::_exit(failure_code);
    }
    ::_exit(0);
  }
}

pozdnyakov::Worker::Worker(Worker&& rhs) noexcept:
  pid_(rhs.pid_),
  requests_(std::move(rhs.requests_)),
  results_(std::move(rhs.results_))
{
  rhs.pid_ = no_process;
}

pozdnyakov::Worker::Worker(std::size_t seed):
  pid_(no_process),
  requests_(),
  results_()
{
  pipe_ends_t requests = makePipe();
  pipe_ends_t results = makePipe();
  std::cout.flush();
  const pid_t pid = ::fork();
  if (pid == no_process)
  {
    throw std::system_error(errno, std::generic_category(), "fork");
  }
  if (pid == 0)
  {
    requests.second = FileDescriptor();
    results.first = FileDescriptor();
    runChild(requests.first, results.second, seed);
  }
  pid_ = pid;
  requests_ = std::move(requests.second);
  results_ = std::move(results.first);
}

pozdnyakov::Worker::~Worker()
{
  if (pid_ == no_process)
  {
    return;
  }
  ::kill(pid_, SIGKILL);
  pid_t finished = ::waitpid(pid_, nullptr, 0);
  while ((finished == no_process) && (errno == EINTR))
  {
    finished = ::waitpid(pid_, nullptr, 0);
  }
}

void pozdnyakov::Worker::send(const calculation_request_t& request) const
{
  const request_header_t header{request.threads, request.tries, request.ellipses.size()};
  writeAll(requests_, &header, sizeof(header));
  writeAll(requests_, request.ellipses.data(), request.ellipses.size() * sizeof(ellipse_t));
}

bool pozdnyakov::Worker::isReady(int milliseconds) const
{
  return waitReadable(results_, milliseconds);
}

pozdnyakov::calculation_result_t pozdnyakov::Worker::receive() const
{
  calculation_result_t result{0.0, 0.0};
  if (!readAll(results_, &result, sizeof(result)))
  {
    throw std::runtime_error("calculating process has terminated");
  }
  return result;
}
