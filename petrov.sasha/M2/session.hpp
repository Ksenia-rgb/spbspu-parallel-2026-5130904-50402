#ifndef SESSION_HPP
#define SESSION_HPP

#include <iosfwd>

namespace petrov {

  class Session {
  public:
    Session(std::istream& input, std::ostream& output);
    Session(const Session& other) = delete;
    Session& operator=(const Session& other) = delete;

    void runCommands();

  private:
    std::istream& input_;
    std::ostream& output_;
  };

}

#endif
