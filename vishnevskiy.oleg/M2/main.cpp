#include <iostream>
#include <map>
#include <string>
#include "commands.hpp"

int main(int argc, char* argv[])
{
  if (argc != 1)
  {
    return 1;
  }
  std::map<std::string, void(*)(std::istream &, std::ostream &, vishnevskiy::Maps &)> cmds;
  cmds["circle"] = vishnevskiy::circle;
  cmds["set"] = vishnevskiy::set;
  cmds["show"] = vishnevskiy::show;
  cmds["showset"] = vishnevskiy::showset;
  cmds["frame"] = vishnevskiy::frame;
  cmds["frameset"] = vishnevskiy::frameset;
  cmds["spawn"] = vishnevskiy::spawn;
  cmds["area-on"] = vishnevskiy::area_on;
  cmds["status"] = vishnevskiy::status;
  cmds["wait"] = vishnevskiy::wait;
  vishnevskiy::Maps mp{};
  std::string cmd;
  while (std::cin >> cmd)
  {
    try
    {
      cmds.at(cmd)(std::cin, std::cout, mp);
    }
    catch(std::runtime_error)
    {
      std::cout << "<INVALID COMMAND>\n";
      auto toignore = std::numeric_limits<std::streamsize>::max();
      std::cin.ignore(toignore, '\n');
    }
    catch (...)
    {
      std::cout << "<UNKNOWN COMMAND>\n";
      auto toignore = std::numeric_limits<std::streamsize>::max();
      std::cin.ignore(toignore, '\n');
    }
  }
  if (!std::cin.eof())
  {
    std::cerr << "Bad input!\n";
    return 1;
  }
  return 0;
}
