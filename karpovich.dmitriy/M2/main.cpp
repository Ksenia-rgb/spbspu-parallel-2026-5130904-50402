#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include "commands.hpp"

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
}
