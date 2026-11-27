#ifndef PROCESS_MANAGER_HPP
#define PROCESS_MANAGER_HPP

#include "process.hpp"
#include "commands.hpp"
#include <cstddef>
#include <string>
#include <map>
#include <ostream>

namespace muhamadiarov
{

  struct Calculation
  {
    std::string process_name;
    std::string set_name;
    int threads = 0;
    int tries = 0;
    StateProcess state = StateProcess::ACTIVE;
    Result result;
  };

  class ProcessManager
  {
  public:
    ProcessManager() = default;
    ProcessManager(const ProcessManager&) = delete;
    ProcessManager& operator=(const ProcessManager&) = delete;
    ~ProcessManager();

    void spawn(const std::string& name, std::size_t seed);
    bool hasProcess(const std::string& process_name) const noexcept;
    void areaOn(const std::string& calc_name, const Calculation& calc, const Data& data);

    void status(const std::string& calc_name, std::ostream& out) const;
    void wait(const std::string& calc_name, std::ostream& out);

  private:
    std::map< std::string, InfoProcess > processes_;
    std::map< std::string, Calculation > calcs_;
  };

}

#endif
