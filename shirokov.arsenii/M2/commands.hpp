#ifndef COMMANDS_HPP
#define COMMANDS_HPP
#include <istream>
#include <ostream>
#include <string>
#include <unordered_map>
#include "../common/ellipse.hpp"
#include "pipe.hpp"

namespace shirokov
{
  using ellipse_map = std::unordered_map< std::string, Ellipse >;
  using set_map = std::unordered_map< std::string, ellipse_list >;
  using channel_map = std::unordered_map< std::string, DuplexChannel >;

  struct Data
  {
    ellipse_map ellipses;
    set_map sets;
    channel_map channels;
  };

  void circle(std::istream& in, std::ostream&, Data& data);
  void ellipse(std::istream& in, std::ostream&, Data& data);
  void set(std::istream& in, std::ostream&, Data& data);
  void show(std::istream& in, std::ostream& out, Data& data);
  void showset(std::istream& in, std::ostream& out, Data& data);
  void frame(std::istream& in, std::ostream& out, Data& data);
  void frameset(std::istream& in, std::ostream& out, Data& data);
  void spawn(std::istream& in, std::ostream&, Data& data);
  void areaOn(std::istream&, std::ostream&, Data&);
  void status(std::istream&, std::ostream&, Data&);
  void wait(std::istream&, std::ostream&, Data&);
}

#endif
