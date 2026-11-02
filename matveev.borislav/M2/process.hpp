#ifndef MATVEEV_PROCESS_HPP
#define MATVEEV_PROCESS_HPP

#include <vector>
#include <cstdint>
#include <cstddef>
#include <sys/types.h>
#include "descriptor.hpp"
#include "protocol.hpp"

namespace matveev
{
  class Process final
  {
  public:
    Process(const Process &) = delete;
    Process(std::uint64_t seed, const std::vector< int > &inherited);
    ~Process();
    Process &operator=(const Process &) = delete;
    int getDescriptor() const noexcept;
    bool hasOutput() const noexcept;
    void start(std::vector< char > &bytes) noexcept;
    void sendPending();
    bool receive(response_t &response);
    void stop(bool force) noexcept;

  private:
    Descriptor channel_{};
    pid_t pid_ = 0;
    std::vector< char > outgoing_ = {};
    std::size_t sent_ = 0;
    response_t response_ = {};
    std::size_t received_ = 0;
  };
}

#endif
