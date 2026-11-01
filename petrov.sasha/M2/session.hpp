#ifndef SESSION_HPP
#define SESSION_HPP

#include <iosfwd>
#include <string>

namespace petrov {

  class Session {
  public:
    Session(std::istream& input, std::ostream& output);
    Session(const Session& other) = delete;
    Session& operator=(const Session& other) = delete;

    void runCommands();

  private:
    void executeLine(const std::string& line);

    std::istream& input_;
    std::ostream& output_;
  };

}

#endif
