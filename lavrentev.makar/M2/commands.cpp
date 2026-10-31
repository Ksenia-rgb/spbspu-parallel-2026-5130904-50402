#include <iostream>
#include <stdexcept>
#include <string>
#include <climits>
#include <vector>
#include <algorithm>
#include <circle.hpp>
#include "commands.hpp"

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

void lavrentev::circle(std::istream &in, std::ostream &, set_map &, circle_data &ac, process_map &)
{
  std::string line;
  if (!std::getline(in, line))
  {
    throw std::logic_error("Empty line");
  }

  std::vector< std::string > data = split(line);
  
  if (data.size() != 4)
  {
    throw std::logic_error("Invalid arguments count for circle");
  }

  std::string name = data[0];
  if (!isValidName(name))
  {
    throw std::logic_error("Unkorrect name");
  }
  if (ac.find(name) != ac.end())
  {
    throw std::logic_error("Circle already exists");
  }

  int radius = 0, x = 0, y = 0;
  try
  {
    radius = std::stoi(data[1]);
    x = std::stoi(data[2]);
    y = std::stoi(data[3]);
  }
  catch (const std::exception &)
  {
    throw std::logic_error("Invalid circle parameters");
  }

  if (radius <= 0)
  {
    throw std::logic_error("Radius must be positive");
  }

  ac[name] = std::make_shared<lavrentev::Circle>(name, radius, x, y);
}

void lavrentev::set(std::istream &in, std::ostream &, set_map &sets, circle_data &ac, process_map &)
{
  std::string line;
  if (!std::getline(in, line))
  {
    throw std::logic_error("Empty line");
  }

  std::vector< std::string > data = split(line);
  if (data.size() < 2)
  {
    throw std::logic_error("Not enough arguments");
  }

  std::string name = data[0];
  if (sets.find(name) != sets.end())
  {
    throw std::logic_error("Set already exists");
  }

  int raw_size = 0;
  try
  {
    raw_size = std::stoi(data[1]);
  }
  catch (const std::exception &)
  {
    throw std::logic_error("Invalid set size");
  }

  if (raw_size < 0 || data.size() != 2 + static_cast< size_t >(raw_size))
  {
    throw std::logic_error("Arguments count mismatch");
  }

  size_t set_size = static_cast< size_t >(raw_size);

  for (size_t i = 0; i < set_size; ++i)
  {
    std::string shape_name = data[i + 2];

    if (ac.find(shape_name) == ac.end())
    {
      throw std::logic_error("Circle does not exist");
    }
    sets[name].push_back(ac[shape_name]);
  }
}

void lavrentev::show(std::istream &in, std::ostream &out, set_map &, circle_data &ac, process_map &)
{
  std::string line;
  std::getline(in, line);
  std::vector< std::string > data = split(line);

  std::string shape_name;
  try
  {
    shape_name = data.at(0);
  }
  catch (const std::out_of_range &e)
  {
    throw std::logic_error(e.what());
  }

  if (ac.find(shape_name) == ac.end())
  {
    throw std::logic_error("Circle does not exist");
  }
  out << ac[shape_name]->getRadius() << " (" << ac[shape_name]->getX() << " " << ac[shape_name]->getY() << ")\n";
}

void lavrentev::showset(std::istream &in, std::ostream &out, set_map &sets, circle_data &, process_map &)
{
  std::string line;
  std::getline(in, line);
  std::vector< std::string > data = split(line);

  std::string set_name;
  try
  {
    set_name = data.at(0);
  }
  catch (const std::out_of_range &e)
  {
    throw std::logic_error(e.what());
  }

  if (sets.find(set_name) == sets.end())
  {
    throw std::logic_error("Set does not exist");
  }
  for (const auto &circle_ptr : sets[set_name])
  {
    out << circle_ptr->getRadius() << " (" << circle_ptr->getX() << " " << circle_ptr->getY() << ")\n";
  }
}

void lavrentev::frame(std::istream &in, std::ostream &out, set_map &, circle_data &ac, process_map &)
{
  std::string line;
  std::getline(in, line);
  std::vector< std::string > data = split(line);

  std::string shape_name;
  try
  {
    shape_name = data.at(0);
  }
  catch (const std::out_of_range &e)
  {
    throw std::logic_error(e.what());
  }

  if (ac.find(shape_name) == ac.end())
  {
    throw std::logic_error("Circle does not exist");
  }

  int min_x, min_y, max_x, max_y;
  min_x = ac[shape_name]->getX() - ac[shape_name]->getRadius();
  min_y = ac[shape_name]->getY() - ac[shape_name]->getRadius();
  max_x = ac[shape_name]->getX() + ac[shape_name]->getRadius();
  max_y = ac[shape_name]->getY() + ac[shape_name]->getRadius();

  out << "(" << min_x << " " << min_y << ") (" << max_x << " " << max_y << ")\n";
}

void lavrentev::frameset(std::istream &in, std::ostream &out, set_map &sets, circle_data &, process_map &)
{
  std::string line;
  std::getline(in, line);
  std::vector< std::string > data = split(line);

  std::string set_name;
  try
  {
    set_name = data.at(0);
  }
  catch (const std::out_of_range &e)
  {
    throw std::logic_error(e.what());
  }

  if (sets.find(set_name) == sets.end())
  {
    throw std::logic_error("Set does not exist");
  }

  int min_x = INT_MAX, min_y = INT_MAX, max_x = INT_MIN, max_y = INT_MIN;

  for (const auto &circle_ptr : sets[set_name])
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

// void lavrentev::spawn(std::istream &in, std::ostream &, set_map &sets, circle_data &ac, process_map &pm)
// {
//   std::string line;
//   std::getline(in, line);
//   std::vector< std::string > data = split(line);

//   std::string process_name;
//   try
//   {
//     process_name = data.at(0);
//   }
//   catch (const std::out_of_range &e)
//   {
//     throw std::logic_error(e.what());
//   }

//   if (pm.find(process_name) != pm.end())
//   {
//     throw std::logic_error("Process already exists");
//   }

//   Process prc = Process(process_name);
//   int seed;
//   in >> seed;
//   prc.set_seed(seed);

  
// }
