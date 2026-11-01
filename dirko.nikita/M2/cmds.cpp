#include "cmds.hpp"
#include <cstddef>
#include <chrono>
#include <iomanip>
#include <thread>
#include <cctype>
#include <utility>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <map>
#include <stdexcept>
#include <string>
#include <iostream>
#include <vector>
#include "../common/shapes.hpp"
#include "worker.hpp"

namespace dirko {

  namespace {
    std::ostream &operator<<(std::ostream &out, dirko::point_t point)
    {
      return out << "(" << point.x << " " << point.y << ")";
    }

    void printShape(std::ostream &out, const dirko::shape_t &shape)
    {
      using namespace dirko;
      if (shape.type == shapeType::CIRCLE) {
        out << shape.rx << " " << shape.position << "\n";
      } else {
        out << shape.rx << " " << shape.ry << " " << shape.position << "\n";
      }
    }

    std::vector< std::string > readArgs(std::istream &in)
    {
      std::string line;
      std::getline(in, line);

      std::vector< std::string > arguments;
      size_t i = 0;

      while (i < line.size()) {
        if (i < line.size() && std::isspace(line[i])) {
          ++i;
          continue;
        }

        const size_t start = i;

        while (i < line.size() && !std::isspace(line[i])) {
          ++i;
        }

        if (start < i) {
          arguments.push_back(line.substr(start, i - start));
        }
      }

      return arguments;
    }
    size_t parseSize(const std::string &token)
    {
      if (token.empty() || token[0] == '-') {
        throw std::invalid_argument("invalid");
      }
      try {
        std::size_t pos = 0;
        const size_t value = std::stoull(token, &pos, 10);
        if (pos != token.size()) {
          throw std::invalid_argument("invalid");
        }
        return value;
      } catch (const std::out_of_range &) {
        throw std::invalid_argument("invalid");
      }
    }

    void updateStates(Data &data)
    {
      for (auto &worker_it : data.workers) {
        Worker &worker = worker_it.second;
        while (worker.output >= 0) {
          Result s{};
          const size_t n = read(worker.output, &s, sizeof(s));
          if (n == (sizeof(s))) {
            for (auto &calc_it : data.calcs) {
              Calc &calc = calc_it.second;
              if (calc.calc_id == s.calc_id && calc.state == CalcState::WORKING) {
                calc.state = CalcState::DONE;
                calc.ms = s.ms;
                calc.area = s.area;
              }
            }
            continue;
          }
          if (n == 0) {
            close(worker.output);
            worker.output = -1;
          }
          break;
        }
      }
    }
    void printStatus(std::ostream &out, const Calc &calc)
    {
      if (calc.state == CalcState::DONE) {
        out << calc.proc << " " << std::fixed << std::setprecision(3) << calc.ms << " " << calc.area << "\n";
      } else {
        out << calc.proc << " <IN PROGRESS>\n";
      }
    }
    bool validName(const std::string &name)
    {
      return !name.empty() && ((name[0] >= 'a' && name[0] <= 'z') || (name[0] >= 'A' && name[0] <= 'Z'));
    }
  }

  void cmdCircle(std::istream &in, std::ostream &, Data &data)
  {
    std::vector< std::string > args = readArgs(in);
    if (args.size() != 4) {
      throw std::invalid_argument("invalid");
    }
    const std::string &name = args[0];
    const double r = std::stod(args[1]);
    const double x = std::stod(args[2]);
    const double y = std::stod(args[3]);
    if (!validName(name) || r <= 0 || data.shapes.count(name)) {
      throw std::invalid_argument("invalid");
    }
    data.shapes[name] = {shapeType::CIRCLE, r, r, {x, y}};
  }

  void cmdSet(std::istream &in, std::ostream &, Data &data)
  {
    std::vector< std::string > args = readArgs(in);

    if (args.size() < 2) {
      throw std::invalid_argument("invalid");
    }

    const std::string &name = args[0];
    const size_t n = parseSize(args[1]);

    if (!validName(name) || n == 0 || data.sets.count(name)) {
      throw std::invalid_argument("invalid");
    }

    if (args.size() != n + 2) {
      throw std::invalid_argument("invalid");
    }

    Set s;
    s.reserve(n);

    for (size_t i = 0; i < n; ++i) {
      const std::string &shape = args[i + 2];

      if (!data.shapes.count(shape)) {
        throw std::invalid_argument("invalid");
      }

      s.push_back(shape);
    }

    data.sets[name] = std::move(s);
  }

  void cmdShow(std::istream &in, std::ostream &out, Data &data)
  {
    std::string name;
    if (!(in >> name)) {
      throw std::invalid_argument("invalid");
    }
    auto it = data.shapes.find(name);
    if (it == data.shapes.end()) {
      throw std::invalid_argument("invalid");
    }
    printShape(out, it->second);
  }

  void cmdShowSet(std::istream &in, std::ostream &out, Data &data)
  {
    std::string name;
    if (!(in >> name)) {
      throw std::invalid_argument("invalid");
    }
    auto it = data.sets.find(name);
    if (it == data.sets.end()) {
      throw std::invalid_argument("invalid");
    }
    for (const std::string &shape_name : it->second) {
      printShape(out, data.shapes.at(shape_name));
    }
  }

  void cmdFrame(std::istream &in, std::ostream &out, Data &data)
  {
    std::string name;
    if (!(in >> name)) {
      throw std::invalid_argument("invalid");
    }
    auto it = data.shapes.find(name);
    if (it == data.shapes.end()) {
      throw std::invalid_argument("invalid");
    }
    const point_t min_point = getMinCorner(it->second);
    const point_t max_point = getMaxCorner(it->second);
    out << min_point << " " << max_point << "\n";
  }

  void cmdFrameSet(std::istream &in, std::ostream &out, Data &data)
  {
    std::string name;
    if (!(in >> name)) {
      throw std::invalid_argument("invalid");
    }
    auto it = data.sets.find(name);
    if (it == data.sets.end()) {
      throw std::invalid_argument("invalid");
    }

    point_t min_point = getMinCorner(data.shapes.at(it->second.front()));
    point_t max_point = getMaxCorner(data.shapes.at(it->second.front()));
    for (const std::string &shape_name : it->second) {
      const shape_t &shape = data.shapes.at(shape_name);
      const point_t a = getMinCorner(shape);
      const point_t b = getMaxCorner(shape);
      min_point.x = std::min(min_point.x, a.x);
      min_point.y = std::min(min_point.y, a.y);
      max_point.x = std::max(max_point.x, b.x);
      max_point.y = std::max(max_point.y, b.y);
    }
    out << min_point << " " << max_point << "\n";
  }

  void cmdSpawn(std::istream &in, std::ostream &, Data &data)
  {
    std::vector< std::string > args = readArgs(in);
    if (args.size() != 2) {
      throw std::invalid_argument("invalid");
    }

    const std::string &name = args[0];
    const size_t seed = parseSize(args[1]);

    if (data.workers.count(name)) {
      throw std::invalid_argument("invalid");
    }

    int pipe_in[2] = {-1, -1};
    int pipe_out[2] = {-1, -1};

    if (pipe(pipe_in) < 0) {
      throw std::invalid_argument("pipe");
    }

    if (pipe(pipe_out) < 0) {
      close(pipe_in[0]);
      close(pipe_in[1]);
      throw std::invalid_argument("pipe");
    }

    const int flags = fcntl(pipe_out[0], F_GETFL, 0);
    if (flags < 0 || fcntl(pipe_out[0], F_SETFL, flags | O_NONBLOCK) < 0) {
      close(pipe_in[0]);
      close(pipe_in[1]);
      close(pipe_out[0]);
      close(pipe_out[1]);
      throw std::runtime_error("set flag");
    }

    const pid_t pid = fork();

    if (pid < 0) {
      close(pipe_in[0]);
      close(pipe_in[1]);
      close(pipe_out[0]);
      close(pipe_out[1]);
      throw std::invalid_argument("fork");
    }

    if (pid == 0) {
      close(pipe_in[1]);
      close(pipe_out[0]);

      for (const auto &kv : data.workers) {
        if (kv.second.input >= 0) {
          close(kv.second.input);
        }
        if (kv.second.output >= 0) {
          close(kv.second.output);
        }
      }

      runWorker(pipe_in[0], pipe_out[1], seed);
      _exit(0);
    }

    close(pipe_in[0]);
    close(pipe_out[1]);

    Worker w;
    w.pid = pid;
    w.input = pipe_in[1];
    w.output = pipe_out[0];
    data.workers[name] = w;
  }

  void cmdAreaOn(std::istream &in, std::ostream &, Data &data)
  {
    constexpr int AREA_ARGS_AMOUNT = 5;
    std::vector< std::string > args = readArgs(in);
    if (args.size() != AREA_ARGS_AMOUNT) {
      throw std::invalid_argument("invalid");
    }

    const std::string &proc = args[0];
    const std::string &calc = args[1];
    const std::string &set_name = args[2];

    const size_t threads = parseSize(args[3]);
    const size_t tries = parseSize(args[4]);

    if (threads == 0 || tries == 0) {
      throw std::invalid_argument("invalid");
    }

    auto worker_iter = data.workers.find(proc);
    if (worker_iter == data.workers.end()) {
      throw std::invalid_argument("invalid");
    }

    auto set_iter = data.sets.find(set_name);
    if (set_iter == data.sets.end()) {
      throw std::invalid_argument("invalid");
    }

    if (data.calcs.count(calc)) {
      throw std::invalid_argument("invalid");
    }

    std::vector< shape_t > sended_shapes;
    sended_shapes.reserve(set_iter->second.size());

    for (const std::string &name : set_iter->second) {
      sended_shapes.push_back(data.shapes.at(name));
    }

    const size_t calc_id = data.next_calc_id++;
    size_t header[4] = {calc_id, threads, tries, sended_shapes.size()};

    int err = 0;
    send(err, worker_iter->second.input, header, sizeof(header));

    if (err <= 0) {
      throw std::invalid_argument("write");
    }

    if (!sended_shapes.empty()) {
      send(err, worker_iter->second.input, sended_shapes.data(), sended_shapes.size() * sizeof(shape_t));

      if (err <= 0) {
        throw std::invalid_argument("write");
      }
    }

    Calc c{};
    c.proc = proc;
    c.calc_id = calc_id;
    data.calcs[calc] = c;
  }

  void cmdStatus(std::istream &in, std::ostream &out, Data &data)
  {
    std::vector< std::string > args = readArgs(in);
    if (args.size() != 1) {
      throw std::invalid_argument("invalid");
    }

    auto it = data.calcs.find(args[0]);
    if (it == data.calcs.end()) {
      throw std::invalid_argument("invalid");
    }
    updateStates(data);
    printStatus(out, it->second);
  }

  void cmdWait(std::istream &in, std::ostream &out, Data &data)
  {
    std::vector< std::string > args = readArgs(in);
    if (args.size() != 1) {
      throw std::invalid_argument("invalid");
    }

    auto it = data.calcs.find(args[0]);
    if (it == data.calcs.end()) {
      throw std::invalid_argument("invalid");
    }

    while (it->second.state != CalcState::DONE) {
      updateStates(data);

      if (it->second.state == CalcState::DONE) {
        break;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    printStatus(out, it->second);
  }
}
