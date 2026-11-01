#ifndef COMMANDS_HPP
#define COMMANDS_HPP
#include <istream>
#include <ostream>
#include <string>
#include <unordered_map>
#include "../common/ellipse.hpp"
#include <queue>
#include "pipe.hpp"

namespace shirokov
{
  struct CalcRes;

  using ellipse_map = std::unordered_map< std::string, Ellipse >;
  using set_map = std::unordered_map< std::string, ellipse_list >;
  using channel_map = std::unordered_map< std::string, DuplexChannel >;
  using calc_map = std::unordered_map< std::string, CalcRes >;
  using proc_queue_map = std::unordered_map< std::string, std::queue< std::string > >;

  struct Data
  {
    ellipse_map ellipses;
    set_map sets;
    channel_map channels;
    calc_map calcs;
    proc_queue_map proc_queues;
  };

  struct CalcRes
  {
    std::string process_name;
    double time;
    double result;
  };

  void circle(std::istream& in, std::ostream&, Data& data);
  void ellipse(std::istream& in, std::ostream&, Data& data);
  void set(std::istream& in, std::ostream&, Data& data);
  void show(std::istream& in, std::ostream& out, Data& data);
  void showset(std::istream& in, std::ostream& out, Data& data);
  void frame(std::istream& in, std::ostream& out, Data& data);
  void frameset(std::istream& in, std::ostream& out, Data& data);
  void spawn(std::istream& in, std::ostream&, Data& data);
  void areaOn(std::istream& in, std::ostream&, Data& data);
  void status(std::istream& in, std::ostream& out, Data& data);
  void wait(std::istream& in, std::ostream& out, Data& data);
}

#endif
