#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <climits>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <circle.hpp>
#include "commands.hpp"
#include "setmap.hpp"
#include "circledata.hpp"
#include "processmap.hpp"

namespace
{
  std::vector< std::string > split(const std::string& str)
  {
    std::vector< std::string > res;
    std::string elem;
    for (const char& symbol : str)
    {
      if (symbol == ' ' && !elem.empty())
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

    // std::cout << '{';
    // for (std::size_t i = 0; i < res.size(); ++i)
    // {
    //   std::cout << '"' << res[i] << '"';
    //   if (i + 1 < res.size())
    //   {
    //     std::cout << ", ";
    //   }
    // }
    // std::cout << "}\n";

    return res;
  }

  bool isValidName(const std::string& name)
  {
    if (name.empty() || (!std::isalpha(name[0]) && name[0] != '_'))
    {
      return false;
    }
    for (char c : name)
    {
      if (!std::isalnum(c) && c != '_')
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

  std::vector< std::string > dt = split(line);
  
  if (dt.size() != 4)
  {
    throw std::logic_error("Invalid arguments count for circle");
  }

  std::string name = dt[0];
  if (!isValidName(name))
  {
    throw std::logic_error("Unkorrect name");
  }
  if (data.ac.find(name) != data.ac.end())
  {
    throw std::logic_error("Circle already exists");
  }

  int radius = 0, x = 0, y = 0;
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

  data.ac[name] = std::make_shared<lavrentev::Circle>(name, radius, x, y);
}

void lavrentev::set(std::istream &in, std::ostream &, Data &data)
{
  std::string line;
  if (!std::getline(in, line))
  {
    throw std::logic_error("Empty line");
  }

  std::vector< std::string > dt = split(line);
  if (dt.size() < 2)
  {
    throw std::logic_error("Not enough arguments");
  }

  std::string name = dt[0];
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

  if (raw_size < 0 || dt.size() != 2 + static_cast< size_t >(raw_size))
  {
    throw std::logic_error("Arguments count mismatch");
  }

  size_t set_size = static_cast< size_t >(raw_size);

  for (size_t i = 0; i < set_size; ++i)
  {
    std::string shape_name = dt[i + 2];

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
  std::vector< std::string > dt = split(line);

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
  out << data.ac[shape_name]->getRadius() << " (" << data.ac[shape_name]->getX() << " " << data.ac[shape_name]->getY() << ")\n";
}

void lavrentev::showset(std::istream &in, std::ostream &out, Data &data)
{
  std::string line;
  std::getline(in, line);
  std::vector< std::string > dt = split(line);

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
    out << circle_ptr->getRadius() << " (" << circle_ptr->getX() << " " << circle_ptr->getY() << ")\n";
  }
}

void lavrentev::frame(std::istream &in, std::ostream &out, Data &data)
{
  std::string line;
  std::getline(in, line);
  std::vector< std::string > dt = split(line);

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

  int min_x, min_y, max_x, max_y;
  min_x = data.ac[shape_name]->getX() - data.ac[shape_name]->getRadius();
  min_y = data.ac[shape_name]->getY() - data.ac[shape_name]->getRadius();
  max_x = data.ac[shape_name]->getX() + data.ac[shape_name]->getRadius();
  max_y = data.ac[shape_name]->getY() + data.ac[shape_name]->getRadius();

  out << "(" << min_x << " " << min_y << ") (" << max_x << " " << max_y << ")\n";
}

void lavrentev::frameset(std::istream &in, std::ostream &out, Data &data)
{
  std::string line;
  std::getline(in, line);
  std::vector< std::string > dt = split(line);

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

  int min_x = INT_MAX, min_y = INT_MAX, max_x = INT_MIN, max_y = INT_MIN;

  for (const auto &circle_ptr : data.sets[set_name])
  {
    int circle_min_x = circle_ptr->getX() - circle_ptr->getRadius();
    int circle_min_y = circle_ptr->getY() - circle_ptr->getRadius();
    int circle_max_x = circle_ptr->getX() + circle_ptr->getRadius();
    int circle_max_y = circle_ptr->getY() + circle_ptr->getRadius();

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
  std::vector< std::string > dt = split(line);

  if (dt.size() != 2)
  {
    throw std::logic_error("Invalid amount of arguments for spawn");
  }
  std::string process_name = dt[0];
  if (!isValidName(process_name))
  {
    throw std::logic_error("Invalid process name");
  }
  if (data.pm.find(process_name) != data.pm.end())
  {
    throw std::logic_error("Process already exists");
  }

  int seed;
  try
  {
    seed = std::stoi(dt[1]);
  }
  catch (const std::exception &)
  {
    throw std::logic_error("Invalid seed");
  }

  std::unique_ptr<Process> prc = std::make_unique<Process>(process_name);
  prc->set_seed(seed);
  data.pm.emplace(process_name, std::move(prc));
}

void lavrentev::area_on(std::istream &in, std::ostream &, Data &data)
{
  std::string line;
  std::getline(in, line);
  std::vector< std::string > dt = split(line);

  if (dt.size() != 5)
  {
    throw std::logic_error("Invalid amount of arguments for area-on");
  }

  std::string process_name = dt[0];
  if (data.pm.find(process_name) == data.pm.end())
  {
    throw std::logic_error("Process does not exist");
  }

  std::string calc_name = dt[1];
  if (!isValidName(calc_name))
  {
    throw std::logic_error("Invalid calculation name");
  }
  if (data.cm.find(calc_name) != data.cm.end())
  {
    throw std::logic_error("Calculation already exists");
  }

  std::string set_name = dt[2];
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
  size_t threads = static_cast< size_t >(raw_threads);

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
  size_t tries = static_cast< size_t >(raw_tries);

  data.pm[process_name]->start(data.sets[set_name], calc_name, threads, tries);
  data.cm[calc_name] = CalcRes{process_name, 0.0, 0.0};
}

void lavrentev::status(std::istream &in, std::ostream &out, Data &data)
{
  std::string line;
  std::getline(in, line);
  std::vector< std::string > dt = split(line);

  if (dt.size() != 1)
  {
    throw std::logic_error("Invalid amount of arguments for status");
  }

  std::string calc_name = dt[0];
  if (data.cm.find(calc_name) == data.cm.end())
  {
    throw std::logic_error("Calculation does not exist");
  }

  std::string process_name = data.cm[calc_name].process_name;
  if (data.pm.find(process_name) != data.pm.end())
  {
    data.pm[process_name]->poll_results(data.cm);
  }

  out << process_name << " ";
  if (data.cm[calc_name].time == 0.0)
  {
    out << "<IN PROGRESS>\n";
  }
  else
  {
    out << static_cast< long >(data.cm[calc_name].time) << " ";
    out << std::fixed << std::setprecision(3) << data.cm[calc_name].result << "\n";
    out << std::defaultfloat;
  }
}

void lavrentev::wait(std::istream &in, std::ostream &out, Data &data)
{
  std::string line;
  std::getline(in, line);
  std::vector< std::string > dt = split(line);
  if (dt.size() != 1)
  {
    throw std::logic_error("Invalid amount of arguments for wait");
  }

  std::string calc_name = dt[0];
  if (data.cm.find(calc_name) == data.cm.end())
  {
    throw std::logic_error("Calculation does not exist");
  }

  std::string process_name = data.cm[calc_name].process_name;
  if (data.pm.find(process_name) == data.pm.end())
  {
    throw std::logic_error("Process does not exist");
  }

  if (data.cm[calc_name].time == 0.0)
  {
    data.pm[process_name]->wait_for(calc_name, data.cm);
  }

  out << process_name << " ";
  out << static_cast< long >(data.cm[calc_name].time) << " ";
  out << std::fixed << std::setprecision(3) << data.cm[calc_name].result << "\n";
  out << std::defaultfloat;
}
