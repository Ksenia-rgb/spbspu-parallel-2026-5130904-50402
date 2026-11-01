#ifndef SESSION_HPP
#define SESSION_HPP

#include <iosfwd>
#include <map>
#include <string>
#include <vector>

#include <shape.hpp>

namespace petrov {

  class Session {
  public:
    Session(std::istream& input, std::ostream& output);
    Session(const Session& other) = delete;
    Session& operator=(const Session& other) = delete;

    void runCommands();

  private:
    const shape_t* findShape(const std::string& name) const;

    void executeLine(const std::string& line);
    bool createCircle(const std::vector< std::string >& tokens);
    bool showShape(const std::vector< std::string >& tokens);

    std::istream& input_;
    std::ostream& output_;
    std::map< std::string, shape_t > shapes_;
  };

}

#endif
