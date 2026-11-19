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
    if(pipe(toChild) < 0 || pipe(toParent) < 0)
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
      childLoop(toChild[0], toParent[1], seed)
    }
    
    close(toChild[0]);
    close(toParent[1]);
    processes_.emplace(name, InfoProcess(name, seed, pid, toChild[1], toParent[0]));
  }

  bool ProcessManager::hasProcess(const std::string& processName)
  {
    return processes_.count(processName) > 0;
  }

  void ProcessManager::areaOn(const std::string& calcName,
              const Calculation& calc, 
              const Set_t& set);

  void status(const std::string calcName) const;
  void wait(const std::string calcName);

}
