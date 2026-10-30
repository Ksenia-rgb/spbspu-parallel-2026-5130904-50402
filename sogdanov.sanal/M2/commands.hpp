#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iosfwd>
#include <map>
#include <string>
#include <vector>

#include "figure.hpp"
#include "worker.hpp"

namespace sogdanov
{
  using Arguments = std::vector< std::string >;

  struct Calc
  {
    size_t id_;
    std::string process_name_;
    bool done_ = false;
    double milliseconds_ = 0.0;
    double area_ = 0.0;
  };

  struct Data
  {
    std::map< std::string, Figure > figures_;
    std::map< std::string, std::vector< Figure > > sets_;
    std::map< std::string, Worker > workers_;
    std::map< std::string, Calc > calcs_;
    size_t next_calculation_id_ = 0;

    Data() = default;
    Data(const Data&) = delete;
    ~Data();

    Data& operator=(const Data&) = delete;
  };

  using command_t = void (*)(const Arguments&, std::ostream&, Data&);

  Arguments tokenize(const std::string& line);

  void cmdCircle(const Arguments& arguments, std::ostream& output, Data& data);
  void cmdSet(const Arguments& arguments, std::ostream& output, Data& data);
  void cmdShow(const Arguments& arguments, std::ostream& output, Data& data);
  void cmdShowSet(const Arguments& arguments, std::ostream& output, Data& data);
  void cmdFrame(const Arguments& arguments, std::ostream& output, Data& data);
  void cmdFrameSet(const Arguments& arguments, std::ostream& output, Data& data);
  void cmdSpawn(const Arguments& arguments, std::ostream& output, Data& data);
  void cmdAreaOn(const Arguments& arguments, std::ostream& output, Data& data);
  void cmdStatus(const Arguments& arguments, std::ostream& output, Data& data);
  void cmdWait(const Arguments& arguments, std::ostream& output, Data& data);

  bool waitForEvents(Data& data, bool read_input);
  void waitForAll(Data& data);
}

#endif
