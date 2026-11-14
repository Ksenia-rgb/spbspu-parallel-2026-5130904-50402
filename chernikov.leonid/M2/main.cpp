#include <csignal>
#include <signal.h>
#include <exception>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include "commands.hpp"

int main()
{
  std::signal(SIGPIPE, SIG_IGN);
  using command_t = void (*)(chernikov::state_t &, const chernikov::args_t &, std::ostream &);
  const std::map< std::string, command_t > commands = {{"circle", chernikov::doCircle}, {"set", chernikov::doSet},
      {"show", chernikov::doShow}, {"showset", chernikov::doShowSet}, {"frame", chernikov::doFrame},
      {"frameset", chernikov::doFrameSet}, {"spawn", chernikov::doSpawn}, {"area-on", chernikov::doAreaOn},
      {"status", chernikov::doStatus}, {"wait", chernikov::doWait}};
  chernikov::state_t state;
  std::string line;
  while (std::getline(std::cin, line)) {
    const chernikov::args_t words = chernikov::splitWords(line);
    if (words.empty()) {
      continue;
    }
    const auto it = commands.find(words.front());
    if (it == commands.end()) {
      std::cout << "<UNKNOWN COMMAND>\n";
      continue;
    }
    try {
      it->second(state, words, std::cout);
    } catch (const std::exception &) {
      std::cout << "<INVALID COMMAND>\n";
    }
  }
  std::cout.flush();
  chernikov::shutdown(state);
  return 0;
}
