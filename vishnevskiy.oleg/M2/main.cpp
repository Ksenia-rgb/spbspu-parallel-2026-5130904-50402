#include <iostream>
#include <map>
#include <string>
#include <sys/wait.h>
#include <sstream>
#include "commands.hpp"

int main(int argc, char *[])
{
  if (argc != 1)
  {
    return 1;
  }
  std::map< std::string, void (*)(std::istream &, std::ostream &, vishnevskiy::Maps &) > cmds;
  cmds["circle"] = vishnevskiy::circle;
  cmds["set"] = vishnevskiy::set;
  cmds["show"] = vishnevskiy::show;
  cmds["showset"] = vishnevskiy::showset;
  cmds["frame"] = vishnevskiy::frame;
  cmds["frameset"] = vishnevskiy::frameset;
  cmds["spawn"] = vishnevskiy::spawn;
  cmds["area-on"] = vishnevskiy::areaOn;
  cmds["status"] = vishnevskiy::status;
  cmds["wait"] = vishnevskiy::wait;
  vishnevskiy::Maps mp{};
  std::string line;
  while (std::getline(std::cin, line))
  {
    std::istringstream iss(line);
    std::string cmd;
    iss >> cmd;
    try
    {
      cmds.at(cmd)(iss, std::cout, mp);
    }
    catch (const std::runtime_error &e)
    {
      std::cout << "<INVALID COMMAND>\n";
    }
    catch (...)
    {
      std::cout << "<UNKNOWN COMMAND>\n";
    }
  }
  if (!std::cin.eof())
  {
    std::cerr << "Bad input!\n";
    return 1;
  }
  for (std::map< std::string, vishnevskiy::ProcInfo >::iterator it = mp.procs.begin(); it != mp.procs.end(); ++it)
  {
    close(it->second.pipe_to_child);
  }
  for (std::map< std::string, vishnevskiy::ProcInfo >::iterator it = mp.procs.begin(); it != mp.procs.end(); ++it)
  {
    waitpid(it->second.pid, nullptr, 0);
  }
  for (std::map< std::string, vishnevskiy::ProcInfo >::iterator it = mp.procs.begin(); it != mp.procs.end(); ++it)
  {
    close(it->second.pipe_to_parent);
  }
  return 0;
}
