#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include "commands.hpp"

int main()
{
  novikov::Data data;

  using cmd_t = void (*)(std::istream& in, std::ostream& out, novikov::Data& data);
  std::unordered_map< std::string, cmd_t > cmds;

  cmds["circle"] = novikov::circle;
  cmds["ellipse"] = novikov::ellipse;
  cmds["set"] = novikov::set;
  cmds["show"] = novikov::show;
  cmds["showset"] = novikov::showset;
  cmds["frame"] = novikov::frame;
  cmds["frameset"] = novikov::frameset;
  cmds["spawn"] = novikov::spawn;
  cmds["area-on"] = novikov::areaOn;
  cmds["status"] = novikov::status;
  cmds["wait"] = novikov::wait;

  std::string cmd;
  while (std::cin >> cmd)
  {
    try
    {
      const auto func = cmds.at(cmd);
      func(std::cin, std::cout, data);
    }
    catch (const std::out_of_range& e)
    {
      std::cout << "<UNKNOWN COMMAND>\n";
      const auto to_ignore = std::numeric_limits< std::streamsize >::max();
      std::cin.ignore(to_ignore, '\n');
    }
    catch (...)
    {
      std::cout << "<INVALID COMMAND>\n";
    }
  }
  if (!std::cin.eof())
  {
    std::cerr << "Invalid input\n";
    return 1;
  }
}
