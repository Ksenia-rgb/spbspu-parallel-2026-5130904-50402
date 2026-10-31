#include "commands.hpp"
#include <cmath>
#include <iostream>
#include <istream>
#include <limits>
#include <stdexcept>
#include <string>
#include "../common/ellipse.hpp"

namespace
{
  void outFigure(std::ostream& out, const shirokov::Ellipse& e)
  {
    out << e.hr << ' ';

    if (std::fabs(e.hr - e.vr) > std::numeric_limits< double >::epsilon())
    {
      out << e.vr << ' ';
    }

    out << '(' << e.x << ' ' << e.y << ')' << '\n';
  }

  std::vector< std::string > split(const std::string& str, char delimiter)
  {
    std::vector< std::string > res;
    std::string elem;
    for (const char& symbol : str)
    {
      if (symbol == delimiter && !elem.empty())
      {
        res.push_back(elem);
        elem.clear();
      }
      else if (symbol != delimiter)
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

  size_t parseSizeT(const std::string& line)
  {
    const auto first_non_space = line.find_first_not_of(" \t\n");
    if (first_non_space != std::string::npos && line[first_non_space] == '-')
    {
      throw std::invalid_argument("negative value");
    }

    const unsigned long long val = std::stoull(line);

    if (val > std::numeric_limits< size_t >::max())
    {
      throw std::out_of_range("value exceeds size_t maximum");
    }

    return static_cast< size_t >(val);
  }

  bool isValidName(const std::string& name)
  {
    if (name.empty() || (!std::isalpha(name[0]) && name[0] != '_'))
    {
      return false;
    }
    for (const char symbol : name)
    {
      if (!std::isalnum(symbol) && symbol != '_')
      {
        return false;
      }
    }
    return true;
  }
}

void shirokov::circle(std::istream& in, std::ostream&, Data& data)
{
  std::string args;
  std::getline(in, args);

  std::vector< std::string > tokens = split(args, ' ');
  std::string name;
  double r = 0.0;
  double x = 0.0;
  double y = 0.0;

  try
  {
    name = tokens.at(0);
    r = std::stod(tokens.at(1));
    x = std::stod(tokens.at(2));
    y = std::stod(tokens.at(3));
  }
  catch (const std::out_of_range& e)
  {
    throw std::range_error(e.what());
  }

  ellipse_map& ellipses = data.ellipses;

  if (!isValidName(name))
  {
    throw std::invalid_argument("invalid figure name");
  }
  if (ellipses.find(name) != ellipses.end())
  {
    throw std::invalid_argument("circle with this name already exist");
  }
  if (r <= 0)
  {
    throw std::invalid_argument("radius must be positive");
  }

  ellipses.insert({name, {x, y, r, r}});
}

void shirokov::ellipse(std::istream& in, std::ostream&, Data& data)
{
  std::string args;
  std::getline(in, args);

  std::vector< std::string > tokens = split(args, ' ');

  std::string name;
  double hr = 0.0;
  double vr = 0.0;
  double x = 0.0;
  double y = 0.0;

  try
  {
    name = tokens.at(0);
    hr = std::stod(tokens.at(1));
    vr = std::stod(tokens.at(2));
    x = std::stod(tokens.at(3));
    y = std::stod(tokens.at(4));
  }
  catch (const std::out_of_range& e)
  {
    throw std::range_error(e.what());
  }

  ellipse_map& ellipses = data.ellipses;

  if (!isValidName(name))
  {
    throw std::invalid_argument("invalid figure name");
  }
  if (ellipses.find(name) != ellipses.end())
  {
    throw std::invalid_argument("circle with this name already exist");
  }
  if (hr <= 0 || vr <= 0)
  {
    throw std::invalid_argument("radiuses must be positive");
  }

  ellipses.insert({name, {x, y, hr, vr}});
}

void shirokov::set(std::istream& in, std::ostream&, Data& data)
{
  std::string args;
  std::getline(in, args);

  std::vector< std::string > tokens = split(args, ' ');

  std::string set_name;
  size_t set_size = 0;

  try
  {
    set_name = tokens.at(0);
    set_size = parseSizeT(tokens.at(1));
  }
  catch (const std::out_of_range& e)
  {
    throw std::range_error(e.what());
  }

  set_map& sets = data.sets;
  ellipse_map& ellipses = data.ellipses;

  if (sets.find(set_name) != sets.end())
  {
    throw std::invalid_argument("set with this name already exist");
  }
  if (!set_size)
  {
    throw std::invalid_argument("incorrect size of set");
  }
  if (tokens.size() != set_size + 2)
  {
    throw std::invalid_argument("arguments count mismatch");
  }

  std::string figure_name;
  std::vector< Ellipse > s;
  s.reserve(set_size);
  try
  {
    for (size_t i = 0; i < set_size; ++i)
    {
      figure_name = tokens.at(i + 2);
      if (ellipses.find(figure_name) == ellipses.end())
      {
        throw std::invalid_argument("figure with this name does't exist: " + figure_name);
      }
      s.push_back(ellipses[figure_name]);
    }
  }
  catch (const std::out_of_range& e)
  {
    throw std::range_error(e.what());
  }

  sets.insert({set_name, std::move(s)});
}

void shirokov::show(std::istream& in, std::ostream& out, Data& data)
{
  std::string args;
  std::getline(in, args);

  std::vector< std::string > tokens = split(args, ' ');

  std::string name;

  try
  {
    name = tokens.at(0);
  }
  catch (const std::out_of_range& e)
  {
    throw std::range_error(e.what());
  }

  ellipse_map& ellipses = data.ellipses;

  if (ellipses.find(name) == ellipses.end())
  {
    throw std::invalid_argument("figure with this name does't exist");
  }

  const Ellipse& e = ellipses[name];
  outFigure(out, e);
}

void shirokov::showset(std::istream& in, std::ostream& out, Data& data)
{
  std::string args;
  std::getline(in, args);

  std::vector< std::string > tokens = split(args, ' ');

  std::string name;

  try
  {
    name = tokens.at(0);
  }
  catch (const std::out_of_range& e)
  {
    throw std::range_error(e.what());
  }

  set_map& sets = data.sets;

  if (sets.find(name) == sets.end())
  {
    throw std::invalid_argument("set with this name does't exist");
  }

  const std::vector< Ellipse >& s = sets[name];
  for (const Ellipse& e : s)
  {
    outFigure(out, e);
  }
}

void shirokov::frame(std::istream& in, std::ostream& out, Data& data)
{
  std::string args;
  std::getline(in, args);

  std::vector< std::string > tokens = split(args, ' ');

  std::string name;

  try
  {
    name = tokens.at(0);
  }
  catch (const std::out_of_range& e)
  {
    throw std::range_error(e.what());
  }

  ellipse_map& ellipses = data.ellipses;

  if (ellipses.find(name) == ellipses.end())
  {
    throw std::invalid_argument("figure with this name does't exist");
  }

  const MinMaxes min_maxes = ellipses[name].getMinMaxes();
  out << '(' << min_maxes.min_x << ' ' << min_maxes.min_y << ')' << ' ';
  out << '(' << min_maxes.max_x << ' ' << min_maxes.max_y << ')' << '\n';
}

void shirokov::frameset(std::istream& in, std::ostream& out, Data& data)
{
  std::string args;
  std::getline(in, args);

  std::vector< std::string > tokens = split(args, ' ');

  std::string name;

  try
  {
    name = tokens.at(0);
  }
  catch (const std::out_of_range& e)
  {
    throw std::range_error(e.what());
  }

  set_map& sets = data.sets;

  if (sets.find(name) == sets.end())
  {
    throw std::invalid_argument("set with this name does't exist");
  }

  const MinMaxes min_maxes = getAllMinMaxes(sets[name]);
  out << '(' << min_maxes.min_x << ' ' << min_maxes.min_y << ')' << ' ';
  out << '(' << min_maxes.max_x << ' ' << min_maxes.max_y << ')' << '\n';
}
