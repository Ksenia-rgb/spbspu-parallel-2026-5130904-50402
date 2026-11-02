#include "session.hpp"

#include <cctype>
#include <cstddef>
#include <exception>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <iomanip>

namespace petrov {

  namespace {

    constexpr std::size_t argument_index = 1;
    constexpr std::size_t circle_radius_index = 2;
    constexpr std::size_t circle_x_index = 3;
    constexpr std::size_t circle_y_index = 4;
    constexpr std::size_t set_size_index = 2;
    constexpr std::size_t spawn_seed_index = 2;
    constexpr std::size_t area_calculation_index = 2;
    constexpr std::size_t area_set_index = 3;
    constexpr std::size_t area_threads_index = 4;
    constexpr std::size_t area_tries_index = 5;

    constexpr std::size_t name_token_count = 2;
    constexpr std::size_t set_token_count = 3;
    constexpr std::size_t spawn_token_count = 3;
    constexpr std::size_t circle_token_count = 5;
    constexpr std::size_t area_token_count = 6;
    constexpr int output_precision = 3;

    constexpr char invalid_command_message[] = "<INVALID COMMAND>";
    constexpr char unknown_command_message[] = "<UNKNOWN COMMAND>";
    constexpr char in_progress_message[] = "<IN PROGRESS>";

    bool isName(const std::string& text)
    {
      if (text.empty()) {
        return false;
      }
      const char first = text[0];
      const bool is_lower_letter = ('a' <= first) && (first <= 'z');
      const bool is_upper_letter = ('A' <= first) && (first <= 'Z');
      return is_lower_letter || is_upper_letter;
    }

    bool parseInteger(const std::string& text, int& value)
    {
      std::size_t parsed_length = 0;
      try {
        value = std::stoi(text, &parsed_length);
      } catch (const std::exception&) {
        return false;
      }
      return parsed_length == text.size();
    }

    bool parseSize(const std::string& text, std::size_t& value)
    {
      std::size_t parsed_length = 0;
      long long parsed = 0;
      try {
        parsed = std::stoll(text, &parsed_length);
      } catch (const std::exception&) {
        return false;
      }
      if ((parsed_length != text.size()) || (parsed < 0)) {
        return false;
      }
      value = static_cast< std::size_t >(parsed);
      return true;
    }

    std::vector< std::string > splitTokens(const std::string& line)
    {
      std::vector< std::string > tokens;
      std::string token;
      for (const char symbol : line) {
        if (std::isspace(static_cast< unsigned char >(symbol)) != 0) {
          if (!token.empty()) {
            tokens.push_back(token);
            token.clear();
          }
        } else {
          token += symbol;
        }
      }
      if (!token.empty()) {
        tokens.push_back(token);
      }
      return tokens;
    }

  }

  Session::Session(std::istream& input, std::ostream& output):
    input_(input),
    output_(output),
    commands_{{"circle", &Session::createCircle},
              {"set", &Session::createSet},
              {"show", &Session::showShape},
              {"showset", &Session::showSet},
              {"frame", &Session::showFrame},
              {"frameset", &Session::showSetFrame},
              {"spawn", &Session::spawnProcess},
              {"area-on", &Session::startCalculation},
              {"status", &Session::showStatus}
            }
  {}

  void Session::runCommands()
  {
    std::string line;
    while (std::getline(input_, line)) {
      executeLine(line);
    }
  }

  Session::~Session()
  {
    closeProcesses();
  }

  const shape_t* Session::findShape(const std::string& name) const
  {
    const auto shape = shapes_.find(name);
    if (shape == shapes_.end()) {
      return nullptr;
    }
    return &shape->second;
  }

  const std::vector< std::string >* Session::findSet(const std::string& name) const
  {
    const auto set = sets_.find(name);
    if (set == sets_.end()) {
      return nullptr;
    }
    return &set->second;
  }

  process_t* Session::findProcess(const std::string& name)
  {
    for (process_t& process : processes_) {
      if (process.name == name) {
        return &process;
      }
    }
    return nullptr;
  }

  calculation_t* Session::findCalculation(const std::string& name)
  {
    const auto calculation = calculations_.find(name);
    if (calculation == calculations_.end()) {
      return nullptr;
    }
    return &calculation->second;
  }

  bool Session::collectShapes(const std::string& name, std::vector< shape_t >& shapes) const
  {
    const std::vector< std::string >* const members = findSet(name);
    if (members == nullptr) {
      return false;
    }
    shapes.clear();
    for (const std::string& member : *members) {
      const shape_t* const shape = findShape(member);
      if (shape == nullptr) {
        return false;
      }
      shapes.push_back(*shape);
    }
    return true;
  }

  void Session::executeLine(const std::string& line)
  {
    const std::vector< std::string > tokens = splitTokens(line);
    if (tokens.empty()) {
      output_ << invalid_command_message << '\n';
      return;
    }
    const auto handler = commands_.find(tokens[0]);
    if (handler == commands_.end()) {
      output_ << unknown_command_message << '\n';
      return;
    }
    const command_handler_t command = handler->second;
    if (!(this->*command)(tokens)) {
      output_ << invalid_command_message << '\n';
    }
  }

  bool Session::createCircle(const std::vector< std::string >& tokens)
  {
    if (tokens.size() != circle_token_count) {
      return false;
    }
    if (!isName(tokens[argument_index])) {
      return false;
    }
    int radius = 0;
    int x = 0;
    int y = 0;
    const bool is_parsed = parseInteger(tokens[circle_radius_index], radius) && parseInteger(tokens[circle_x_index], x)
        && parseInteger(tokens[circle_y_index], y);
    if (!is_parsed || (radius <= 0)) {
      return false;
    }
    shape_t shape = {};
    shape.radius = static_cast< double >(radius);
    shape.center.x = static_cast< double >(x);
    shape.center.y = static_cast< double >(y);
    return shapes_.emplace(tokens[argument_index], shape).second;
  }

  bool Session::createSet(const std::vector< std::string >& tokens)
  {
    if (tokens.size() < set_token_count) {
      return false;
    }
    if (!isName(tokens[argument_index])) {
      return false;
    }
    int size = 0;
    if (!parseInteger(tokens[set_size_index], size)) {
      return false;
    }
    if ((size <= 0) || (static_cast< std::size_t >(size) != (tokens.size() - set_token_count))) {
      return false;
    }
    std::vector< std::string > members;
    for (std::size_t index = set_token_count; index < tokens.size(); ++index) {
      if (!isName(tokens[index]) || (findShape(tokens[index]) == nullptr)) {
        return false;
      }
      members.push_back(tokens[index]);
    }
    return sets_.emplace(tokens[argument_index], members).second;
  }

  bool Session::showShape(const std::vector< std::string >& tokens)
  {
    if (tokens.size() != name_token_count) {
      return false;
    }
    const shape_t* const shape = findShape(tokens[argument_index]);
    if (shape == nullptr) {
      return false;
    }
    output_ << *shape << '\n';
    return true;
  }

  bool Session::showSet(const std::vector< std::string >& tokens)
  {
    if (tokens.size() != name_token_count) {
      return false;
    }
    std::vector< shape_t > shapes;
    if (!collectShapes(tokens[argument_index], shapes)) {
      return false;
    }
    for (const shape_t& shape : shapes) {
      output_ << shape << '\n';
    }
    return true;
  }

  bool Session::showFrame(const std::vector< std::string >& tokens)
  {
    if (tokens.size() != name_token_count) {
      return false;
    }
    const shape_t* const shape = findShape(tokens[argument_index]);
    if (shape == nullptr) {
      return false;
    }
    output_ << findFrame(*shape) << '\n';
    return true;
  }

  bool Session::showSetFrame(const std::vector< std::string >& tokens)
  {
    if (tokens.size() != name_token_count) {
      return false;
    }
    std::vector< shape_t > shapes;
    if (!collectShapes(tokens[argument_index], shapes)) {
      return false;
    }
    output_ << findFrame(shapes) << '\n';
    return true;
  }

  bool Session::spawnProcess(const std::vector< std::string >& tokens)
  {
    if (tokens.size() != spawn_token_count) {
      return false;
    }
    if (!isName(tokens[argument_index]) || (findProcess(tokens[argument_index]) != nullptr)) {
      return false;
    }
    std::size_t seed = 0;
    if (!parseSize(tokens[spawn_seed_index], seed)) {
      return false;
    }
    std::vector< int > inherited;
    for (const process_t& process : processes_) {
      inherited.push_back(process.request_descriptor);
      inherited.push_back(process.result_descriptor);
    }
    try {
      processes_.push_back(createProcess(tokens[argument_index], seed, inherited));
    } catch (const std::exception&) {
      return false;
    }
    return true;
  }

  bool Session::startCalculation(const std::vector< std::string >& tokens)
  {
    if (tokens.size() != area_token_count) {
      return false;
    }
    if (!isName(tokens[argument_index]) || !isName(tokens[area_calculation_index])) {
      return false;
    }
    const process_t* const process = findProcess(tokens[argument_index]);
    if ((process == nullptr) || (findCalculation(tokens[area_calculation_index]) != nullptr)) {
      return false;
    }
    std::vector< shape_t > shapes;
    if (!collectShapes(tokens[area_set_index], shapes)) {
      return false;
    }
    int threads = 0;
    int tries = 0;
    const bool is_parsed
        = parseInteger(tokens[area_threads_index], threads) && parseInteger(tokens[area_tries_index], tries);
    if (!is_parsed || (threads < 0) || (tries <= 0)) {
      return false;
    }
    request_t request = {};
    request.calculation_id = next_calculation_id_;
    request.threads = threads;
    request.tries = tries;
    request.shape_count = static_cast< int >(shapes.size());
    if (!sendCalculation(*process, request, shapes)) {
      return false;
    }
    calculation_t calculation = {};
    calculation.id = next_calculation_id_;
    calculation.process_name = tokens[argument_index];
    calculations_.emplace(tokens[area_calculation_index], calculation);
    ++next_calculation_id_;
    return true;
  }

  bool Session::showStatus(const std::vector< std::string >& tokens)
  {
    if (tokens.size() != name_token_count) {
      return false;
    }
    updateResults();
    const calculation_t* const calculation = findCalculation(tokens[argument_index]);
    if (calculation == nullptr) {
      return false;
    }
    printCalculation(*calculation);
    return true;
  }

  void Session::updateResults()
  {
    for (process_t& process : processes_) {
      result_t result = {};
      while (receiveResult(process, result)) {
        storeResult(result);
      }
    }
  }

  void Session::storeResult(const result_t& result)
  {
    for (auto& entry : calculations_) {
      if (entry.second.id == result.calculation_id) {
        entry.second.is_finished = true;
        entry.second.milliseconds = result.milliseconds;
        entry.second.area = result.area;
        return;
      }
    }
  }

  void Session::printCalculation(const calculation_t& calculation) const
  {
    output_ << calculation.process_name << ' ';
    if (!calculation.is_finished) {
      output_ << in_progress_message << '\n';
      return;
    }
    const std::ios_base::fmtflags flags = output_.flags();
    const std::streamsize precision = output_.precision();
    output_ << std::fixed << std::setprecision(output_precision) << calculation.milliseconds << ' ' << calculation.area
            << '\n';
    output_.flags(flags);
    output_.precision(precision);
  }

  void Session::closeProcesses()
  {
    for (process_t& process : processes_) {
      closeProcess(process);
    }
  }

}
