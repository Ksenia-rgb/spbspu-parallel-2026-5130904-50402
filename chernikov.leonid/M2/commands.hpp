#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <vector>
#include "shapes.hpp"

namespace chernikov {
  struct process_t {
    int pid;
    int request_fd;
    int result_fd;
  };
  struct calc_t {
    std::string process;
    bool done;
    double ms;
    double area;
  };
  struct state_t {
    std::map< std::string, std::unique_ptr< Shape > > shapes;
    std::map< std::string, std::vector< std::string > > sets;
    std::map< std::string, process_t > processes;
    std::map< std::string, calc_t > calcs;
    std::vector< std::string > calc_names;
  };
  using args_t = std::vector< std::string >;

  long long parseInteger(const std::string &str);
  args_t splitWords(const std::string &line);

  void doCircle(state_t &state, const args_t &args, std::ostream &out);
  void doSet(state_t &state, const args_t &args, std::ostream &out);
  void doShow(state_t &state, const args_t &args, std::ostream &out);
  void doShowSet(state_t &state, const args_t &args, std::ostream &out);
  void doFrame(state_t &state, const args_t &args, std::ostream &out);
  void doFrameSet(state_t &state, const args_t &args, std::ostream &out);
  void doSpawn(state_t &state, const args_t &args, std::ostream &out);
  void doAreaOn(state_t &state, const args_t &args, std::ostream &out);
  void doStatus(state_t &state, const args_t &args, std::ostream &out);
  void doWait(state_t &state, const args_t &args, std::ostream &out);
  void shutdown(state_t &state);
}
#endif
