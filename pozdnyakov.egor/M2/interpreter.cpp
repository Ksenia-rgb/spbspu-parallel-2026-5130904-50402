#include "interpreter.hpp"
#include <cstddef>
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include "calculation.hpp"
#include "parse.hpp"
#include "registry.hpp"
#include "shapes.hpp"
#include "worker.hpp"

namespace
{
  void checkCount(const pozdnyakov::words_t& words, std::size_t count)
  {
    if (words.size() != count)
    {
      throw std::invalid_argument("wrong number of parameters");
    }
  }

  double parseCoordinate(const std::string& text)
  {
    return static_cast< double >(pozdnyakov::parseInteger(text));
  }

  std::size_t parsePositive(const std::string& text)
  {
    const std::size_t value = pozdnyakov::parseSize(text);
    if (value == 0)
    {
      throw std::invalid_argument("value must be positive: " + text);
    }
    return value;
  }
}

bool pozdnyakov::Interpreter::execute(const words_t& words, std::ostream& out)
{
  static const std::map< std::string, command_t > commands{
      {"circle", &Interpreter::createCircle},
      {"set", &Interpreter::createSet},
      {"show", &Interpreter::showShape},
      {"showset", &Interpreter::showSet},
      {"frame", &Interpreter::showShapeFrame},
      {"frameset", &Interpreter::showSetFrame},
      {"spawn", &Interpreter::spawnProcess},
      {"area-on", &Interpreter::startCalculation},
      {"status", &Interpreter::showStatus},
      {"wait", &Interpreter::waitCalculation},
  };
  if (words.empty())
  {
    return true;
  }
  const std::map< std::string, command_t >::const_iterator command = commands.find(words.front());
  if (command == commands.cend())
  {
    return false;
  }
  (this->*(command->second))(words, out);
  return true;
}

void pozdnyakov::Interpreter::createCircle(const words_t& words, std::ostream&)
{
  const std::size_t count = 5;
  checkCount(words, count);
  const double radius = parseCoordinate(words[2]);
  const point_t center{parseCoordinate(words[3]), parseCoordinate(words[4])};
  registry_.addShape(words[1], makeCircle(radius, center));
}

void pozdnyakov::Interpreter::createSet(const words_t& words, std::ostream&)
{
  const std::size_t header = 3;
  if (words.size() < header)
  {
    throw std::invalid_argument("wrong number of parameters");
  }
  const std::size_t size = parsePositive(words[2]);
  checkCount(words, header + size);
  const names_t shapes(words.cbegin() + header, words.cend());
  registry_.addSet(words[1], shapes);
}

void pozdnyakov::Interpreter::showShape(const words_t& words, std::ostream& out)
{
  const std::size_t count = 2;
  checkCount(words, count);
  out << registry_.getShape(words[1]) << '\n';
}

void pozdnyakov::Interpreter::showSet(const words_t& words, std::ostream& out)
{
  const std::size_t count = 2;
  checkCount(words, count);
  for (const ellipse_t& shape : registry_.getSet(words[1]))
  {
    out << shape << '\n';
  }
}

void pozdnyakov::Interpreter::showShapeFrame(const words_t& words, std::ostream& out)
{
  const std::size_t count = 2;
  checkCount(words, count);
  out << getFrame(registry_.getShape(words[1])) << '\n';
}

void pozdnyakov::Interpreter::showSetFrame(const words_t& words, std::ostream& out)
{
  const std::size_t count = 2;
  checkCount(words, count);
  out << getFrame(registry_.getSet(words[1])) << '\n';
}

void pozdnyakov::Interpreter::spawnProcess(const words_t& words, std::ostream&)
{
  const std::size_t count = 3;
  checkCount(words, count);
  const std::string& name = words[1];
  const std::size_t seed = parseSize(words[2]);
  if (!isValidName(name) || (processes_.count(name) != 0))
  {
    throw std::invalid_argument("process cannot be spawned: " + name);
  }
  processes_.emplace(name, process_t{Worker(seed), {}});
}

void pozdnyakov::Interpreter::startCalculation(const words_t& words, std::ostream&)
{
  const std::size_t count = 6;
  const std::size_t name_index = 2;
  checkCount(words, count);
  const std::string& name = words[name_index];
  if (!isValidName(name) || registry_.hasCalculation(name))
  {
    throw std::invalid_argument("calculation cannot be started: " + name);
  }
  process_t& process = processes_.at(words[1]);
  const calculation_request_t request{parsePositive(words[4]), parsePositive(words[5]), registry_.getSet(words[3])};
  process.worker.send(request);
  registry_.addCalculation(name, words[1]);
  process.pending.push(name);
}

void pozdnyakov::Interpreter::showStatus(const words_t& words, std::ostream& out)
{
  const std::size_t count = 2;
  checkCount(words, count);
  const calculation_t& calculation = registry_.getCalculation(words[1]);
  collectResults();
  out << calculation << '\n';
}

void pozdnyakov::Interpreter::waitCalculation(const words_t& words, std::ostream& out)
{
  const std::size_t count = 2;
  checkCount(words, count);
  const calculation_t& calculation = registry_.getCalculation(words[1]);
  process_t& process = processes_.at(calculation.process);
  while (calculation.status == calculation_status_t::in_progress)
  {
    receiveResult(process);
  }
  out << calculation << '\n';
}

void pozdnyakov::Interpreter::collectResults()
{
  for (std::pair< const std::string, process_t >& process : processes_)
  {
    while (!process.second.pending.empty() && process.second.worker.isReady(0))
    {
      receiveResult(process.second);
    }
  }
}

void pozdnyakov::Interpreter::receiveResult(process_t& process)
{
  const calculation_result_t result = process.worker.receive();
  calculation_t& calculation = registry_.getCalculation(process.pending.front());
  calculation.status = calculation_status_t::done;
  calculation.result = result;
  process.pending.pop();
}
