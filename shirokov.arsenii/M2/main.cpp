#include <ios>
#include <iostream>
#include <istream>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include "commands.hpp"

int main()
{
  shirokov::Data data;

  using cmd_t = void (*)(std::istream& in, std::ostream& out, shirokov::Data& data);
  std::unordered_map< std::string, cmd_t > cmds;

  cmds["circle"] = shirokov::circle;
  cmds["ellipse"] = shirokov::ellipse;
  cmds["set"] = shirokov::set;
  cmds["show"] = shirokov::show;
  cmds["showset"] = shirokov::showset;
  cmds["frame"] = shirokov::frame;
  cmds["frameset"] = shirokov::frameset;
  cmds["spawn"] = shirokov::spawn;
  cmds["area-on"] = shirokov::areaOn;
  cmds["status"] = shirokov::status;
  cmds["wait"] = shirokov::wait;

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
    std::cerr << "Bad input\n";
    return 1;
  }
}
