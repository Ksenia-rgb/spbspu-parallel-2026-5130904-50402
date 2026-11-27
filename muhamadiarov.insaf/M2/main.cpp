#include "commands.hpp"
#include <iostream>
#include <signal.h>
#include <stdexcept>
#include <string>
#include <cstddef>
#include <map>
#include <limits>

int main()
{
  namespace muh = muhamadiarov;
  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
  {
    return 1;
  }
  using cmd_t = void (*)(std::istream &, std::ostream &, muh::Data &);

  muh::Data data;
  std::map< std::string, cmd_t > cmds;
  cmds["circle"] = muh::cmdCircle;
  cmds["set"] = muh::cmdSet;
  cmds["show"] = muh::cmdShow;
  cmds["showset"] = muh::cmdShowSet;
  cmds["frame"] = muh::cmdFrame;
  cmds["frameset"] = muh::cmdFrameSet;
  cmds["spawn"] = muh::cmdSpawn;
  cmds["area-on"] = muh::cmdAreaOn;
  cmds["status"] = muh::cmdStatus;
  cmds["wait"] = muh::cmdWait;

  std::string cmd;
  while (std::cin >> cmd)
  {
    auto it = cmds.find(cmd);
    if (it == cmds.end())
    {
      std::cout << "<UNKNOWN COMMAND>\n";
      std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
      continue;
    }

    try
    {
      it->second(std::cin, std::cout, data);
    }
    catch (const std::exception &)
    {
      std::cout << "<INVALID COMMAND>\n";
    }
  }

  return 0;
}
