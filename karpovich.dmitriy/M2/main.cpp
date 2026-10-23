#include <iostream>
#include <limits>
#include <signal.h>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_map>
#include "commands.hpp"
#include "worker.hpp"

int main()
{
  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
  {
    return 1;
  }
  using cmd_t = void (*)(std::istream &, std::ostream &, karpovich::Data &);

  karpovich::Data data;
  std::unordered_map< std::string, cmd_t > cmds;
  cmds["circle"] = karpovich::cmdCircle;
  cmds["set"] = karpovich::cmdSet;
  cmds["show"] = karpovich::cmdShow;
  cmds["showset"] = karpovich::cmdShowSet;
  cmds["frame"] = karpovich::cmdFrame;
  cmds["frameset"] = karpovich::cmdFrameSet;
  cmds["spawn"] = karpovich::cmdSpawn;
  cmds["area-on"] = karpovich::cmdAreaOn;
  cmds["status"] = karpovich::cmdStatus;
  cmds["wait"] = karpovich::cmdWait;

  std::string cmd;
  while (std::cin >> cmd)
  {
    try
    {
      auto func = cmds.at(cmd);
      func(std::cin, std::cout, data);
    }
    catch (const std::out_of_range &)
    {
      std::cout << "<UNKNOWN COMMAND>\n";
      std::cin.clear();
      std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
    }
    catch (const std::invalid_argument &)
    {
      std::cout << "<INVALID COMMAND>\n";
      std::cin.clear();
      std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
    }
  }

  for (auto &kv : data.workers)
  {
    const karpovich::Worker &w = kv.second;
    if (w.wr >= 0)
    {
      close(w.wr);
    }
    if (w.result_rd >= 0)
    {
      close(w.result_rd);
    }
    if (w.pid > 0)
    {
      waitpid(w.pid, nullptr, 0);
    }
  }
}
