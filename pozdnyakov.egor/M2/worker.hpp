#ifndef WORKER_HPP
#define WORKER_HPP

#include <cstddef>
#include <sys/types.h>
#include "calculation.hpp"
#include "descriptor.hpp"

namespace pozdnyakov
{
  class Worker
  {
  public:
    Worker(const Worker&) = delete;
    Worker(Worker&& rhs) noexcept;
    explicit Worker(std::size_t seed);
    ~Worker();

    Worker& operator=(const Worker&) = delete;
    Worker& operator=(Worker&&) = delete;

    void send(const calculation_request_t& request) const;
    bool isReady(int milliseconds) const;
    calculation_result_t receive() const;

  private:
    pid_t pid_;
    FileDescriptor requests_;
    FileDescriptor results_;
  };
}

#endif
