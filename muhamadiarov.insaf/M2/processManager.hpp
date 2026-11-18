#ifndef PROCESSMANAGER_HPP
#define PROCESSMANAGER_HPP

#include "process.hpp"
#include "commands.hpp"
#include <cstddef>
#include <string>
#include <map>

namespace muhamadiarov
{

  struct Calculation
  {
    std::string processName;
    std::string setName;
    int threads = 0;
    int tries   = 0;
    StateProcess state = StateProcess::ACTIVE;
  };

  class ProcessManager
  {
  public:
    void spawn(const std::string& name, std::size_t seed) noexcept;
    void areaOn(const std::string& procName,
                const std::string& calcName,
                const std::string& setName,
                int threads,
                int tries,
                const Data& data);
    void status(const std::string calcName) const;
    void wait(const std::string calcName);
    const InfoProcess& getCalculation(const std::string& calcName) const noexcept;

  private:
    std::map< std::string, InfoProcess > processes_;
    std::map< std::string, Calculation > calcs_;
  };

}

#endif
