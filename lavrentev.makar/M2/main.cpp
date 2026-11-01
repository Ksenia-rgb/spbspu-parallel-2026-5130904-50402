#include <iostream>
#include <limits>
#include <circle.hpp>
#include "child.hpp"
#include "commands.hpp"

namespace lavrentev
{
  using cmd_t = void (*)(
    std::istream &, std::ostream &,
    Data &
  );
}

int main(int argc, char* argv[])
{
  if (argc == 3 && std::string(argv[0]) == "child")
  {
    return run_child(std::atoi(argv[1]), std::atoi(argv[2]));
  }

  lavrentev::set_map sets;
  lavrentev::circle_data all_circles;
  lavrentev::process_map pm;
  lavrentev::calc_map cm;
  lavrentev::Data data{sets, all_circles, pm, cm};
  std::unordered_map<std::string, lavrentev::cmd_t> cmds;

  cmds["circle"] = lavrentev::circle;
  cmds["set"] = lavrentev::set;
  cmds["show"] = lavrentev::show;
  cmds["showset"] = lavrentev::showset;
  cmds["frame"] = lavrentev::frame;
  cmds["frameset"] = lavrentev::frameset;
  cmds["spawn"] = lavrentev::spawn;
  cmds["area-on"] = lavrentev::area_on;
  cmds["status"] = lavrentev::status;
  cmds["wait"] = lavrentev::wait;

  std::string cmd;
  while (std::cin >> cmd)
  {
    try
    {
      cmds.at(cmd)(std::cin, std::cout, data);
    } catch (const std::out_of_range &)
    {
      std::cout << "<UNKNOWN COMMAND>\n";
      auto toignore = std::numeric_limits<std::streamsize>::max();
      std::cin.ignore(toignore, '\n');
    } catch (const std::logic_error &)
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
