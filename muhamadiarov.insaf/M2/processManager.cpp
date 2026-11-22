#include "processManager.hpp"
#include <stdexcept>
#include <unistd.h>
#include <sys/wait.h>

namespace muhamadiarov
{

  ProcessManager::~ProcessManager()
  {
    for (auto& kv : processes_)
    {
      if (kv.second.pid > 0)
      {
        kill(kv.second.pid, SIGTERM);
        int st = 0;
        waitpid(kv.second.pid, &st, 0);
      }
    }
  }

  void ProcessManager::spawn(const std::string& name, std::size_t seed)
  {
    if (processes_.count(name))
    {
      throw std::invalid_argument("process already exists");
    }

    int toChild[2];
    int toParent[2];
    if (pipe(toChild) < 0 || pipe(toParent) < 0)
    {
      throw std::runtime_error("pipe failed");
    }

    const pid_t pid = fork();
    if (pid < 0)
    {
      close(toChild[0]);
      close(toChild[1]);
      close(toParent[0]);
      close(toParent[1]);
      throw std::runtime_error("fork failed");
    }

    if (pid == 0)
    {
      close(toChild[1]);
      close(toParent[0]);
      childLoop(toChild[0], toParent[1]);
    }

    close(toChild[0]);
    close(toParent[1]);
    processes_.emplace(name, InfoProcess{name, pid, StateProcess::STAND, toChild[1], toParent[0], seed});
  }

  bool ProcessManager::hasProcess(const std::string& processName) const noexcept
  {
    return processes_.count(processName) > 0;
  }

  void ProcessManager::areaOn(const std::string& calcName, const Calculation& calc, const Data& data)
  {
    if (calcs_.count(calcName))
    {
      throw std::invalid_argument("dup calc");
    }

    if (!hasProcess(calc.processName))
    {
      throw std::invalid_argument("no process");
    }

    if (!data.sets.count(calc.setName))
    {
      throw std::invalid_argument("no set");
    }

    if (calc.tries <= 0)
    {
      throw std::invalid_argument("no tries");
    }

    if (calc.threads < 0)
    {
      throw std::invalid_argument("no threads");
    }

    const Set_t& set = data.sets.at(calc.setName);
    InfoProcess& process = processes_.at(calc.processName);
    Task task{calc.threads, calc.tries, process.seed, set.size()};
    int err = 0;
    if (send(err, process.writeFd, &task, sizeof(Task)) != sizeof(Task))
    {
      throw std::runtime_error("did not send task");
    }

    std::vector< TaskShape > raw;
    raw.reserve(set.size());
    for (const auto& sn : set)
    {
      const Shape& s = *(data.shapes).at(sn);
      const double r = (s.getMaxX() - s.getMinX()) / 2.0;
      const double x = (s.getMaxX() + s.getMinX()) / 2.0;
      const double y = (s.getMaxY() + s.getMinY()) / 2.0;
      raw.push_back(TaskShape{r, x, y});
    }

    if (!raw.empty())
    {
      const std::size_t bytes = sizeof(TaskShape) * raw.size();
      if (send(err, process.writeFd, raw.data(), bytes) != bytes)
      {
        throw std::runtime_error("did not send shapes");
      }
    }

    Calculation c = calc;
    c.state = StateProcess::ACTIVE;
    calcs_.emplace(calcName, c);
    process.state = StateProcess::ACTIVE;
  }

  void ProcessManager::status(const std::string calcName, std::ostream& out) const
  {
    auto it = calcs_.find(calcName);
    if (it == calcs_.end())
    {
      throw std::runtime_error("no calc");
    }

    const Calculation& c = it->second;
    const InfoProcess& p = processes_.at(c.processName);

    if (p.state == StateProcess::DONE)
    {
      char buf[128];
      std::snprintf(buf, sizeof buf, "%s %.3f %.3f", p.name.c_str(), c.result.time, c.result.area);
      out << buf << '\n';
    }
    else if (p.state == StateProcess::CANCELED)
    {
      out << p.name << " <CANCELED>\n";
    }
    else
    {
      out << p.name << " <IN PROGRESS>\n";
    }
  }

  void ProcessManager::wait(const std::string calcName, std::ostream& out)
  {
    auto it = calcs_.find(calcName);
    if (it == calcs_.end())
    {
      throw std::invalid_argument("no calc");
    }

    Calculation& c = it->second;
    InfoProcess& p = processes_.at(c.processName);
    if (p.state == StateProcess::CANCELED)
    {
      throw std::runtime_error("error working the process");
    }

    if (p.state == StateProcess::ACTIVE)
    {
      Result r;
      int err = 0;
      if (take(err, p.readFd, &r, sizeof(Result)) != sizeof(Result))
      {
        p.state = StateProcess::CANCELED;
        c.state = StateProcess::CANCELED;
      }
      else
      {
        c.result = r;
        c.state = StateProcess::DONE;
        p.state = StateProcess::DONE;
      }
    }

    status(calcName, out);
  }

}
