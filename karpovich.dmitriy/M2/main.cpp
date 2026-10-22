#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include "commands.hpp"
#include <sys/wait.h>
#include <unistd.h>

int main()
{
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

  std::string cmd;
  while (std::cin >> cmd)
  {
    try
    {
      cmds.at(cmd)(std::cin, std::cout, data);
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
  for (auto &entry : data.workers)
  {
    karpovich::Worker &w = entry.second;
    close(w.wr);
    waitpid(w.pid, nullptr, 0);
  }
}
