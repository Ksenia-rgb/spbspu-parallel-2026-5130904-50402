#include "commands.hpp"
#include <cerrno>
#include <cstddef>
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <sys/poll.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include "child.hpp"
#include "protocol.hpp"
#include "shapes.hpp"

namespace {
  void checkArgs(const chernikov::args_t &args, size_t count)
  {
    if (args.size() != count) {
      throw std::invalid_argument("wrong number of arguments");
    }
  }

  void checkName(const std::string &name)
  {
    const char c = name.front();
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) {
      throw std::invalid_argument("bad name");
    }
  }

  const chernikov::Shape &findShape(const chernikov::state_t &state, const std::string &name)
  {
    const auto it = state.shapes.find(name);
    if (it == state.shapes.end()) {
      throw std::invalid_argument("no such shape");
    }
    return *it->second;
  }

  const std::vector< std::string > &findSet(const chernikov::state_t &state, const std::string &name)
  {
    const auto it = state.sets.find(name);
    if (it == state.sets.end()) {
      throw std::invalid_argument("no such set");
    }
    return it->second;
  }

  chernikov::calc_t &findCalc(chernikov::state_t &state, const std::string &name)
  {
    const auto it = state.calcs.find(name);
    if (it == state.calcs.end()) {
      throw std::invalid_argument("no such calc");
    }
    return it->second;
  }

  void applyResult(chernikov::state_t &state, const chernikov::result_t &result)
  {
    if (result.calc_id >= state.calc_names.size()) {
      return;
    }
    chernikov::calc_t &calc = state.calcs.at(state.calc_names[result.calc_id]);
    calc.done = true;
    calc.ms = result.ms;
    calc.area = result.area;
  }

  void collectReady(chernikov::state_t &state, const chernikov::process_t &proc)
  {
    pollfd fds = {proc.result_fd, POLLIN, 0};
    while (poll(&fds, 1, 0) > 0 && (fds.revents & POLLIN) != 0) {
      chernikov::result_t result = {};
      if (!chernikov::receiveResult(proc.result_fd, result)) {
        return;
      }
      applyResult(state, result);
    }
  }

  void printCalc(const chernikov::calc_t &calc, std::ostream &out)
  {
    out << calc.process << " ";
    if (calc.done) {
      const std::ios_base::fmtflags flags = out.flags();
      const std::streamsize precision = out.precision();
      out << std::fixed << std::setprecision(3) << calc.ms << " " << calc.area;
      out.flags(flags);
      out.precision(precision);
    } else {
      out << "<IN PROGRESS>";
    }
    out << "\n";
  }

  void closeFd(int fd)
  {
    if (fd >= 0) {
      ::close(fd);
    }
  }
}

long long chernikov::parseInteger(const std::string &str)
{
  size_t pos = 0;
  const long long value = std::stoll(str, &pos);
  if (pos != str.size()) {
    throw std::invalid_argument("not an integer");
  }
  return value;
}

chernikov::args_t chernikov::splitWords(const std::string &line)
{
  args_t words;
  std::string word;
  for (const char c : line) {
    if (c == ' ' || c == '\t' || c == '\r') {
      if (!word.empty()) {
        words.push_back(word);
        word.clear();
      }
    } else {
      word += c;
    }
  }
  if (!word.empty()) {
    words.push_back(word);
  }
  return words;
}

void chernikov::doCircle(state_t &state, const args_t &args, std::ostream &)
{
  const size_t circle_args = 5;
  checkArgs(args, circle_args);
  const std::string &name = args[1];
  checkName(name);
  if (state.shapes.count(name) != 0) {
    throw std::invalid_argument("shape exists");
  }
  const long long r = parseInteger(args[2]);
  const long long x = parseInteger(args[3]);
  const long long y = parseInteger(args[4]);
  if (r <= 0) {
    throw std::invalid_argument("bad radius");
  }
  const point_t center = {static_cast< double >(x), static_cast< double >(y)};
  state.shapes[name] = std::unique_ptr< Shape >(new Circle(static_cast< double >(r), center));
}

void chernikov::doSet(state_t &state, const args_t &args, std::ostream &)
{
  if (args.size() < 3) {
    throw std::invalid_argument("wrong number of arguments");
  }
  const std::string &name = args[1];
  checkName(name);
  if (state.sets.count(name) != 0) {
    throw std::invalid_argument("set exists");
  }
  const long long size = parseInteger(args[2]);
  if (size <= 0 || static_cast< size_t >(size) != args.size() - 3) {
    throw std::invalid_argument("bad set size");
  }
  const std::vector< std::string > names(args.begin() + 3, args.end());
  for (const std::string &shape : names) {
    findShape(state, shape);
  }
  state.sets[name] = names;
}

void chernikov::doShow(state_t &state, const args_t &args, std::ostream &out)
{
  checkArgs(args, 2);
  findShape(state, args[1]).print(out);
  out << "\n";
}

void chernikov::doShowSet(state_t &state, const args_t &args, std::ostream &out)
{
  checkArgs(args, 2);
  for (const std::string &name : findSet(state, args[1])) {
    findShape(state, name).print(out);
    out << "\n";
  }
}

void chernikov::doFrame(state_t &state, const args_t &args, std::ostream &out)
{
  checkArgs(args, 2);
  printFrame(out, findShape(state, args[1]).getFrame());
  out << "\n";
}

void chernikov::doFrameSet(state_t &state, const args_t &args, std::ostream &out)
{
  checkArgs(args, 2);
  const std::vector< std::string > &names = findSet(state, args[1]);
  frame_t frame = findShape(state, names.front()).getFrame();
  for (const std::string &name : names) {
    frame = unite(frame, findShape(state, name).getFrame());
  }
  printFrame(out, frame);
  out << "\n";
}

void chernikov::doSpawn(state_t &state, const args_t &args, std::ostream &out)
{
  checkArgs(args, 3);
  const std::string &name = args[1];
  checkName(name);
  if (state.processes.count(name) != 0) {
    throw std::invalid_argument("process exists");
  }
  const long long seed = parseInteger(args[2]);
  if (seed < 0) {
    throw std::invalid_argument("bad seed");
  }
  int request[2] = {-1, -1};
  int result[2] = {-1, -1};
  if (::pipe(request) == -1) {
    throw std::runtime_error("pipe failed");
  }
  if (::pipe(result) == -1) {
    closeFd(request[0]);
    closeFd(request[1]);
    throw std::runtime_error("pipe failed");
  }
  out.flush();
  std::cout.flush();
  const int pid = ::fork();
  if (pid == -1) {
    closeFd(request[0]);
    closeFd(request[1]);
    closeFd(result[0]);
    closeFd(result[1]);
    throw std::runtime_error("fork failed");
  }
  if (pid == 0) {
    closeFd(request[1]);
    closeFd(result[0]);
    for (const auto &proc : state.processes) {
      closeFd(proc.second.request_fd);
      closeFd(proc.second.result_fd);
    }
    int code = 0;
    try {
      runChild(request[0], result[1], static_cast< size_t >(seed));
    } catch (...) {
      code = 2;
    }
    closeFd(request[0]);
    closeFd(result[1]);
    ::_exit(code);
  }
  closeFd(request[0]);
  closeFd(result[1]);
  state.processes[name] = {pid, request[1], result[0]};
}

void chernikov::doAreaOn(state_t &state, const args_t &args, std::ostream &)
{
  const size_t area_args = 6;
  checkArgs(args, area_args);
  const auto proc = state.processes.find(args[1]);
  if (proc == state.processes.end()) {
    throw std::invalid_argument("no such process");
  }
  const std::string &calc_name = args[2];
  checkName(calc_name);
  if (state.calcs.count(calc_name) != 0) {
    throw std::invalid_argument("calc exists");
  }
  const std::vector< std::string > &names = findSet(state, args[3]);
  const long long threads = parseInteger(args[4]);
  const long long tries = parseInteger(args[5]);
  if (threads < 0 || tries <= 0) {
    throw std::invalid_argument("bad calc parameters");
  }
  std::vector< shape_data_t > shapes;
  shapes.reserve(names.size());
  for (const std::string &name : names) {
    shapes.push_back(findShape(state, name).toData());
  }
  const request_header_t header
      = {state.calc_names.size(), static_cast< size_t >(threads), static_cast< size_t >(tries), shapes.size()};
  if (!sendRequest(proc->second.request_fd, header, shapes)) {
    throw std::runtime_error("cannot send request");
  }
  state.calc_names.push_back(calc_name);
  state.calcs[calc_name] = {proc->first, false, 0.0, 0.0};
}

void chernikov::doStatus(state_t &state, const args_t &args, std::ostream &out)
{
  checkArgs(args, 2);
  const calc_t &calc = findCalc(state, args[1]);
  if (!calc.done) {
    collectReady(state, state.processes.at(calc.process));
  }
  printCalc(calc, out);
}

void chernikov::doWait(state_t &state, const args_t &args, std::ostream &out)
{
  checkArgs(args, 2);
  const calc_t &calc = findCalc(state, args[1]);
  const process_t &proc = state.processes.at(calc.process);
  while (!calc.done) {
    result_t result = {};
    if (!receiveResult(proc.result_fd, result)) {
      break;
    }
    applyResult(state, result);
  }
  printCalc(calc, out);
}

void chernikov::shutdown(state_t &state)
{
  for (auto &proc : state.processes) {
    closeFd(proc.second.request_fd);
    proc.second.request_fd = -1;
  }
  for (auto &proc : state.processes) {
    int status = 0;
    while (::waitpid(proc.second.pid, &status, 0) == -1 && errno == EINTR) {
    }
    closeFd(proc.second.result_fd);
    proc.second.result_fd = -1;
  }
}
