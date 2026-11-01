#include "session.hpp"

#include <cctype>
#include <istream>
#include <ostream>
#include <string>
#include <vector>

namespace petrov {

  namespace {

    constexpr char invalid_command_message[] = "<INVALID COMMAND>";
    constexpr char unknown_command_message[] = "<UNKNOWN COMMAND>";

    std::vector< std::string > splitTokens(const std::string& line)
    {
      std::vector< std::string > tokens;
      std::string token;
      for (const char symbol : line) {
        if (std::isspace(static_cast< unsigned char >(symbol)) != 0) {
          if (!token.empty()) {
            tokens.push_back(token);
            token.clear();
          }
        } else {
          token += symbol;
        }
      }
      if (!token.empty()) {
        tokens.push_back(token);
      }
      return tokens;
    }

  }

  Session::Session(std::istream& input, std::ostream& output):
    input_(input),
    output_(output)
  {}

  void Session::runCommands()
  {
    std::string line;
    while (std::getline(input_, line)) {
      executeLine(line);
    }
  }

  void Session::executeLine(const std::string& line)
  {
    const std::vector< std::string > tokens = splitTokens(line);
    if (tokens.empty()) {
      output_ << invalid_command_message << '\n';
      return;
    }
    output_ << unknown_command_message << '\n';
  }

}
