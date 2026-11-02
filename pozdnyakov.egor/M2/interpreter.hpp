#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <iosfwd>
#include <map>
#include <queue>
#include <string>
#include "parse.hpp"
#include "registry.hpp"
#include "worker.hpp"

namespace pozdnyakov
{
  struct process_t
  {
    Worker worker;
    std::queue< std::string > pending;
  };

  class Interpreter
  {
  public:
    bool execute(const words_t& words, std::ostream& out);

  private:
    using command_t = void (Interpreter::*)(const words_t&, std::ostream&);
    using processes_t = std::map< std::string, process_t >;

    Registry registry_;
    processes_t processes_;

    void createCircle(const words_t& words, std::ostream& out);
    void createSet(const words_t& words, std::ostream& out);
    void showShape(const words_t& words, std::ostream& out);
    void showSet(const words_t& words, std::ostream& out);
    void showShapeFrame(const words_t& words, std::ostream& out);
    void showSetFrame(const words_t& words, std::ostream& out);
    void spawnProcess(const words_t& words, std::ostream& out);
    void startCalculation(const words_t& words, std::ostream& out);
    void showStatus(const words_t& words, std::ostream& out);
    void waitCalculation(const words_t& words, std::ostream& out);
    void collectResults();
    void receiveResult(process_t& process);
  };
}

#endif
