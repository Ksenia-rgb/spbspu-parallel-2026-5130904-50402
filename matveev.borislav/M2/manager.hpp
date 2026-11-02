#ifndef MATVEEV_MANAGER_HPP
#define MATVEEV_MANAGER_HPP

#include <map>
#include <memory>
#include <deque>
#include <string>
#include <iosfwd>
#include "process.hpp"

namespace matveev
{
  constexpr int waitIndefinitely = -1;

  class Manager final
  {
  public:
    void spawn(const std::string &name, std::uint64_t seed);
    void submit(const std::string &process, const std::string &name,
        const std::vector< shape_t > &shapes, std::uint64_t threads, std::uint64_t tries);
    bool pump(int timeout, int input = invalidDescriptor);
    void printStatus(const std::string &name, std::ostream &out) const;
    void wait(const std::string &name);
    void waitFor(const std::string &name, std::uint64_t milliseconds);
    void kill(const std::string &name);

  private:
    enum class State
    {
      pending,
      completed,
      canceled
    };
    struct calculation_t
    {
      std::string process = {};
      State state = State::pending;
      response_t result = {};
    };
    struct task_t
    {
      std::string name = {};
      std::vector< char > bytes = {};
    };
    struct worker_t
    {
      std::unique_ptr< Process > process = {};
      std::deque< task_t > queue = {};
      std::string current = {};
      bool stopping = false;
    };
    std::map< std::string, worker_t > workers_ = {};
    std::map< std::string, calculation_t > calculations_ = {};
    void startNext(worker_t &worker);
    void cancelQueued(worker_t &worker);
    void disconnect(worker_t &worker);
  };
}

#endif
