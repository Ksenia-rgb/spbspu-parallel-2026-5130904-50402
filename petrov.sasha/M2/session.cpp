#include "session.hpp"

#include <istream>
#include <ostream>
#include <string>

namespace petrov {

  namespace {

    constexpr char unknown_command_message[] = "<UNKNOWN COMMAND>";

  }

  Session::Session(std::istream& input, std::ostream& output):
    input_(input),
    output_(output)
  {}

  void Session::runCommands()
  {
    std::string line;
    while (std::getline(input_, line)) {
      output_ << unknown_command_message << '\n';
    }
  }

}
