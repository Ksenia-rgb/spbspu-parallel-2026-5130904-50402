#include "commands.hpp"
#include "processManager.hpp"
#include <istream>
#include <ostream>
#include <string>
#include <sstream>
#include <vector>

namespace muhamadiarov
{

  namespace
  {

    ProcessManager pm;

    std::vector< std::string > getArgs(std::istream& in)
    {
      std::vector< std::string > result;
      std::string line;
      if (std::getline(in, line))
      {
        std::istringstream iss(line);
        std::string word;
        while (iss >> word)
        {
          result.push_back(word);
        }
      }

      return result;
    }

    bool isValidName(const std::string& name)
    {
      return !name.empty() && ((name[0] >= 'a' && name[0] <= 'z') || (name[0] >= 'A' && name[0] <= 'Z'));
    }

    bool isValidNumber(const std::string& number)
    {
      for (std::size_t i = 0; i < number.size(); ++i)
      {
        if (number[i] < '0' || number[i] > '9')
        {
          return false;
        }
      }

      return number.empty() ? false : true;
    }

    std::size_t getSize(const std::string& str)
    {
      if (isValidNumber(str) && str[0] != '-')
      {
        std::size_t pos = 0;
        unsigned long long val = std::stoll(str, &pos);
        if (val > SIZE_MAX)
        {
          throw std::invalid_argument("the quantity exceeds the permissible values");
        }

        return static_cast< std::size_t >(val);
      }

      throw std::invalid_argument("incorrect argument");
    }

    int getDouble(const std::string& str)
    {
      if (isValidNumber(str))
      {
        return std::stod(str);
      }

      throw std::invalid_argument("incorrect argument");
    }

    std::string getName(const std::string& str)
    {
      if (isValidName(str))
      {
        return str;
      }

      throw std::invalid_argument("incorrect argument");
    }
  }

  void cmdCircle(std::istream& in, std::ostream&, Data& data)
  {
    const std::vector< std::string > args = getArgs(in);
    if (args.size() != 4)
    {
      throw std::invalid_argument("there are no correct count of arguments");
    }

    const std::string name = getName(args[0]);
    if (data.shapes.count(name))
    {
      throw std::invalid_argument("dup");
    }

    const int r = getDouble(args[1]);
    if (r <= 0.0)
    {
      throw std::invalid_argument("incorrect radius");
    }

    const double x = getDouble(args[2]);
    const double y = getDouble(args[3]);
    data.shapes[name] = std::make_unique< Circle >(r, Point{x, y});
  }

  void cmdSet(std::istream& in, std::ostream&, Data& data)
  {
    const std::vector< std::string > args = getArgs(in);
    if (args.size() < 3)
    {
      throw std::invalid_argument("there are no correct count of arguments");
    }

    const std::string name = getName(args[0]);
    if (data.sets.count(name))
    {
      throw std::invalid_argument("dup set");
    }

    const std::size_t size = getSize(args[1]);
    if (size != args.size() - 2)
    {
      throw std::invalid_argument("incorrect count of shapes");
    }

    Set_t s;
    s.reserve(size);
    for (std::size_t i = 2; i < args.size(); ++i)
    {
      const std::string shape_name = getName(args[i]);
      if (!data.shapes.count(shape_name))
      {
        throw std::invalid_argument("incorrect shape-name");
      }

      s.push_back(shape_name);
    }

    data.sets[name] = std::move(s);
  }

  void cmdShow(std::istream& in, std::ostream& out, Data& data)
  {
    const std::vector< std::string > args = getArgs(in);
    if (args.size() != 1)
    {
      throw std::invalid_argument("there are no correct count of arguments");
    }

    const std::string name = getName(args[0]);
    if (!data.shapes.count(name))
    {
      throw std::invalid_argument("not find this shape");
    }

    const Shape& s = *data.shapes.at(name);
    const double r = (s.getMaxX() - s.getMinX()) / 2.0;
    const double x = (s.getMaxX() + s.getMinX()) / 2.0;
    const double y = (s.getMaxY() + s.getMinY()) / 2.0;
    out << r << " (" << x << ' ' << y << ")\n";
  }

  void cmdShowSet(std::istream& in, std::ostream& out, Data& data)
  {
    const std::vector< std::string > args = getArgs(in);
    if (args.size() != 1)
    {
      throw std::invalid_argument("there are no correct count of arguments");
    }

    const std::string name = getName(args[0]);
    auto it = data.sets.find(name);
    if (it == data.sets.end())
    {
      throw std::invalid_argument("no set");
    }

    for (const auto& sn : it->second)
    {
      const Shape& s = *data.shapes.at(sn);
      const double r = (s.getMaxX() - s.getMinX()) / 2.0;
      const double cx = (s.getMaxX() + s.getMinX()) / 2.0;
      const double cy = (s.getMaxY() + s.getMinY()) / 2.0;
      out << r << " (" << cx << ' ' << cy << ")\n";
    }
  }

  void cmdFrame(std::istream& in, std::ostream& out, Data& data)
  {
    const std::vector< std::string > args = getArgs(in);
    if (args.size() != 1)
    {
      throw std::invalid_argument("there are no correct count of arguments");
    }

    const std::string name = getName(args[0]);
    auto it = data.shapes.find(name);
    if (it == data.shapes.end())
    {
      throw std::invalid_argument("no shape");
    }

    const Shape& s = *it->second;
    out << '(' << s.getMinX() << ' ' << s.getMinY() << ") (" << s.getMaxX() << ' ' << s.getMaxY() << ")\n";
  }

  void cmdFrameSet(std::istream& in, std::ostream& out, Data& data)
  {
    const std::vector< std::string > args = getArgs(in);
    if (args.size() != 1)
    {
      throw std::invalid_argument("there are no correct count of arguments");
    }

    const std::string name = getName(args[0]);
    auto it = data.sets.find(name);
    if (it == data.sets.end())
    {
      throw std::invalid_argument("no set");
    }

    double min_x = 0, max_x = 0, min_y = 0, max_y = 0;
    bool first = true;
    for (const auto& sn : it->second)
    {
      const Shape& s = *data.shapes.at(sn);
      if (first)
      {
        min_x = s.getMinX();
        max_x = s.getMaxX();
        min_y = s.getMinY();
        max_y = s.getMaxY();
        first = false;
      }
      else
      {
        if (s.getMinX() < min_x)
        {
          min_x = s.getMinX();
        }

        if (s.getMaxX() > max_x)
        {
          max_x = s.getMaxX();
        }

        if (s.getMinY() < min_y)
        {
          min_y = s.getMinY();
        }

        if (s.getMaxY() > max_y)
        {
          max_y = s.getMaxY();
        }
      }
    }

    out << '(' << min_x << ' ' << min_y << ") (" << max_x << ' ' << max_y << ")\n";
  }

  void cmdSpawn(std::istream& in, std::ostream&, Data&)
  {
    const std::vector< std::string > args = getArgs(in);
    if (args.size() != 2)
    {
      throw std::invalid_argument("there are no correct count of arguments");
    }

    const std::string name = getName(args[0]);
    const std::size_t seed = getSize(args[1]);
    pm.spawn(name, seed);
  }

  void cmdAreaOn(std::istream& in, std::ostream&, Data& data)
  {
    const std::vector< std::string > args = getArgs(in);
    if (args.size() != 5)
    {
      throw std::invalid_argument("there are no correct count of arguments");
    }

    const std::string proc_name = getName(args[0]);
    const std::string calc_name = getName(args[1]);

    const std::string set_name = getName(args[2]);
    if (!data.sets.count(set_name))
    {
      throw std::invalid_argument("no set");
    }

    const std::size_t threads = getSize(args[3]);
    const std::size_t tries = getSize(args[4]);

    Calculation c;
    c.process_name = proc_name;
    c.set_name = set_name;
    c.threads = threads;
    c.tries = tries;
    c.state = StateProcess::ACTIVE;

    pm.areaOn(calc_name, c, data);
  }

  void cmdStatus(std::istream& in, std::ostream& out, Data&)
  {
    const std::vector< std::string > args = getArgs(in);
    if (args.size() != 1)
    {
      throw std::invalid_argument("there are no correct count of arguments");
    }

    const std::string calc_name = getName(args[0]);
    pm.status(calc_name, out);
  }

  void cmdWait(std::istream& in, std::ostream& out, Data&)
  {
    const std::vector< std::string > args = getArgs(in);
    if (args.size() != 1)
    {
      throw std::invalid_argument("there are no correct count of arguments");
    }

    const std::string calc_name = getName(args[0]);
    pm.wait(calc_name, out);
  }

}
