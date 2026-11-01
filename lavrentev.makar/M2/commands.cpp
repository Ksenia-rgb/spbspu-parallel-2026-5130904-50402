#include "commands.hpp"

#include <algorithm>
#include <climits>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <circle.hpp>

#include "calcmap.hpp"
#include "circledata.hpp"
#include "processmap.hpp"
#include "setmap.hpp"

namespace
{
  const std::size_t CIRCLE_ARGS = 4;
  const std::size_t SET_MIN_ARGS = 2;
  const std::size_t SPAWN_ARGS = 2;
  const std::size_t AREA_ON_ARGS = 5;
  const std::size_t STATUS_ARGS = 1;
  const std::size_t WAIT_ARGS = 1;
  const int PRECISION = 3;

  std::vector< std::string > split(const std::string &str)
  {
    std::vector< std::string > res;
    std::string elem;
    for (const char symbol : str)
    {
      if ((symbol == ' ') && (!elem.empty()))
      {
        res.push_back(elem);
        elem.clear();
      }
      else if (symbol != ' ')
      {
        elem += symbol;
      }
    }
    if (!elem.empty())
    {
      res.push_back(elem);
    }
    return res;
  }

  bool isValidName(const std::string &name)
  {
    if (name.empty() || ((!std::isalpha(name[0])) && (name[0] != '_')))
    {
      return false;
    }
    for (const char c : name)
    {
      if ((!std::isalnum(c)) && (c != '_'))
      {
        return false;
      }
    }
    return true;
  }
}

void lavrentev::circle(std::istream &in, std::ostream &, Data &data)
{
  std::string line;
  if (!std::getline(in, line))
  {
    throw std::logic_error("Empty line");
  }

  const std::vector< std::string > dt = split(line);
  if (dt.size() != CIRCLE_ARGS)
  {
    throw std::logic_error("Invalid arguments count for circle");
  }

  const std::string name = dt[0];
  if (!isValidName(name))
  {
    throw std::logic_error("Incorrect name");
  }
  if (data.ac.find(name) != data.ac.end())
  {
    throw std::logic_error("Circle already exists");
  }

  int radius = 0;
  int x = 0;
  int y = 0;
  try
  {
    radius = std::stoi(dt[1]);
    x = std::stoi(dt[2]);
    y = std::stoi(dt[3]);
  }
  catch (const std::exception &)
  {
    throw std::logic_error("Invalid circle parameters");
  }

  if (radius <= 0)
  {
    throw std::logic_error("Radius must be positive");
  }

  data.ac[name] = std::make_shared< lavrentev::Circle >(name, radius, x, y);
}

void lavrentev::set(std::istream &in, std::ostream &, Data &data)
{
  std::string line;
  if (!std::getline(in, line))
  {
    throw std::logic_error("Empty line");
  }

  const std::vector< std::string > dt = split(line);
  if (dt.size() < SET_MIN_ARGS)
  {
    throw std::logic_error("Not enough arguments");
  }

  const std::string name = dt[0];
  if (data.sets.find(name) != data.sets.end())
  {
    throw std::logic_error("Set already exists");
  }

  int raw_size = 0;
  try
  {
    raw_size = std::stoi(dt[1]);
  }
  catch (const std::exception &)
  {
    throw std::logic_error("Invalid set size");
  }

  if ((raw_size < 0) || (dt.size() != (SET_MIN_ARGS + static_cast< std::size_t >(raw_size))))
  {
    throw std::logic_error("Arguments count mismatch");
  }

  const std::size_t set_size = static_cast< std::size_t >(raw_size);
  for (std::size_t i = 0; i < set_size; ++i)
  {
    const std::string shape_name = dt[i + SET_MIN_ARGS];
    if (data.ac.find(shape_name) == data.ac.end())
    {
      throw std::logic_error("Circle does not exist");
    }
    data.sets[name].push_back(data.ac[shape_name]);
  }
}

void lavrentev::show(std::istream &in, std::ostream &out, Data &data)
{
  std::string line;
  std::getline(in, line);
  const std::vector< std::string > dt = split(line);

  std::string shape_name;
  try
  {
    shape_name = dt.at(0);
  }
  catch (const std::out_of_range &e)
  {
    throw std::logic_error(e.what());
  }

  if (data.ac.find(shape_name) == data.ac.end())
  {
    throw std::logic_error("Circle does not exist");
  }

  out << data.ac[shape_name]->getRadius() << " (";
  out << data.ac[shape_name]->getX() << " " << data.ac[shape_name]->getY() << ")\n";
}

void lavrentev::showset(std::istream &in, std::ostream &out, Data &data)
{
  std::string line;
  std::getline(in, line);
  const std::vector< std::string > dt = split(line);

  std::string set_name;
  try
  {
    set_name = dt.at(0);
  }
  catch (const std::out_of_range &e)
  {
    throw std::logic_error(e.what());
  }

  if (data.sets.find(set_name) == data.sets.end())
  {
    throw std::logic_error("Set does not exist");
  }

  for (const auto &circle_ptr : data.sets[set_name])
  {
    out << circle_ptr->getRadius() << " (";
    out << circle_ptr->getX() << " " << circle_ptr->getY() << ")\n";
  }
}

void lavrentev::frame(std::istream &in, std::ostream &out, Data &data)
{
  std::string line;
  std::getline(in, line);
  const std::vector< std::string > dt = split(line);

  std::string shape_name;
  try
  {
    shape_name = dt.at(0);
  }
  catch (const std::out_of_range &e)
  {
    throw std::logic_error(e.what());
  }

  if (data.ac.find(shape_name) == data.ac.end())
  {
    throw std::logic_error("Circle does not exist");
  }

  const int r = data.ac[shape_name]->getRadius();
  const int x = data.ac[shape_name]->getX();
  const int y = data.ac[shape_name]->getY();

  const int min_x = x - r;
  const int min_y = y - r;
  const int max_x = x + r;
  const int max_y = y + r;

  out << "(" << min_x << " " << min_y << ") (" << max_x << " " << max_y << ")\n";
}

void lavrentev::frameset(std::istream &in, std::ostream &out, Data &data)
{
  std::string line;
  std::getline(in, line);
  const std::vector< std::string > dt = split(line);

  std::string set_name;
  try
  {
    set_name = dt.at(0);
  }
  catch (const std::out_of_range &e)
  {
    throw std::logic_error(e.what());
  }

  if (data.sets.find(set_name) == data.sets.end())
  {
    throw std::logic_error("Set does not exist");
  }

  int min_x = INT_MAX;
  int min_y = INT_MAX;
  int max_x = INT_MIN;
  int max_y = INT_MIN;

  for (const auto &circle_ptr : data.sets[set_name])
  {
    const int r = circle_ptr->getRadius();
    const int x = circle_ptr->getX();
    const int y = circle_ptr->getY();

    const int circle_min_x = x - r;
    const int circle_min_y = y - r;
    const int circle_max_x = x + r;
    const int circle_max_y = y + r;

    min_x = std::min(min_x, circle_min_x);
    min_y = std::min(min_y, circle_min_y);
    max_x = std::max(max_x, circle_max_x);
    max_y = std::max(max_y, circle_max_y);
  }

  out << "(" << min_x << " " << min_y << ") (" << max_x << " " << max_y << ")\n";
}

void lavrentev::spawn(std::istream &in, std::ostream &, Data &data)
{
  std::string line;
  std::getline(in, line);
  const std::vector< std::string > dt = split(line);

  if (dt.size() != SPAWN_ARGS)
  {
    throw std::logic_error("Invalid amount of arguments for spawn");
  }

  const std::string process_name = dt[0];
  if (!isValidName(process_name))
  {
    throw std::logic_error("Invalid process name");
  }
  if (data.pm.find(process_name) != data.pm.end())
  {
    throw std::logic_error("Process already exists");
  }

  int seed = 0;
  try
  {
    seed = std::stoi(dt[1]);
  }
  catch (const std::exception &)
  {
    throw std::logic_error("Invalid seed");
  }

  std::unique_ptr< Process > prc = std::make_unique< Process >(process_name);
  prc->setSeed(seed);
  data.pm.emplace(process_name, std::move(prc));
}

void lavrentev::area_on(std::istream &in, std::ostream &, Data &data)
{
  std::string line;
  std::getline(in, line);
  const std::vector< std::string > dt = split(line);

  if (dt.size() != AREA_ON_ARGS)
  {
    throw std::logic_error("Invalid amount of arguments for area-on");
  }

  const std::string process_name = dt[0];
  if (data.pm.find(process_name) == data.pm.end())
  {
    throw std::logic_error("Process does not exist");
  }

  const std::string calc_name = dt[1];
  if (!isValidName(calc_name))
  {
    throw std::logic_error("Invalid calculation name");
  }
  if (data.cm.find(calc_name) != data.cm.end())
  {
    throw std::logic_error("Calculation already exists");
  }

  const std::string set_name = dt[2];
  if (data.sets.find(set_name) == data.sets.end())
  {
    throw std::logic_error("Set does not exist");
  }

  int raw_threads = 0;
  try
  {
    raw_threads = std::stoi(dt[3]);
  }
  catch (const std::exception &)
  {
    throw std::logic_error("Invalid threads");
  }
  if (raw_threads <= 0)
  {
    throw std::logic_error("Threads must be positive");
  }
  const std::size_t threads = static_cast< std::size_t >(raw_threads);

  int raw_tries = 0;
  try
  {
    raw_tries = std::stoi(dt[4]);
  }
  catch (const std::exception &)
  {
    throw std::logic_error("Invalid tries");
  }
  if (raw_tries <= 0)
  {
    throw std::logic_error("Tries must be positive");
  }
  const std::size_t tries = static_cast< std::size_t >(raw_tries);

  data.pm[process_name]->start(data.sets[set_name], calc_name, threads, tries);
  data.cm[calc_name] = CalcRes{process_name, 0.0, 0.0};
}

void lavrentev::status(std::istream &in, std::ostream &out, Data &data)
{
  std::string line;
  std::getline(in, line);
  const std::vector< std::string > dt = split(line);

  if (dt.size() != STATUS_ARGS)
  {
    throw std::logic_error("Invalid amount of arguments for status");
  }

  const std::string calc_name = dt[0];
  if (data.cm.find(calc_name) == data.cm.end())
  {
    throw std::logic_error("Calculation does not exist");
  }

  const std::string process_name = data.cm[calc_name].process_name;
  if (data.pm.find(process_name) != data.pm.end())
  {
    data.pm[process_name]->pollResults(data.cm);
  }

  out << process_name << " ";
  if (data.cm[calc_name].time == 0.0)
  {
    out << "<IN PROGRESS>\n";
  }
  else
  {
    out << static_cast< long >(data.cm[calc_name].time) << " ";
    out << std::fixed << std::setprecision(PRECISION) << data.cm[calc_name].result << "\n";
    out << std::defaultfloat;
  }
}

void lavrentev::wait(std::istream &in, std::ostream &out, Data &data)
{
  std::string line;
  std::getline(in, line);
  const std::vector< std::string > dt = split(line);

  if (dt.size() != WAIT_ARGS)
  {
    throw std::logic_error("Invalid amount of arguments for wait");
  }

  const std::string calc_name = dt[0];
  if (data.cm.find(calc_name) == data.cm.end())
  {
    throw std::logic_error("Calculation does not exist");
  }

  const std::string process_name = data.cm[calc_name].process_name;
  if (data.pm.find(process_name) == data.pm.end())
  {
    throw std::logic_error("Process does not exist");
  }

  if (data.cm[calc_name].time == 0.0)
  {
    data.pm[process_name]->waitFor(calc_name, data.cm);
  }

  out << process_name << " ";
  out << static_cast< long >(data.cm[calc_name].time) << " ";
  out << std::fixed << std::setprecision(PRECISION) << data.cm[calc_name].result << "\n";
  out << std::defaultfloat;
}
