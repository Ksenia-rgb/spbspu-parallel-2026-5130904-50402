#include "commands.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../common/shapes.hpp"

namespace karpenkov
{
  namespace
  {
    constexpr int noError = 0;
    constexpr int errInvalidCommand = 1;

    using Args = std::vector< std::string >;
    using Handler = int (*)(const Args &);
    using HandlerTable = std::unordered_map< std::string, Handler >;
    using CircleMap = std::unordered_map< std::string, Circle >;
    using SetMap = std::unordered_map< std::string, std::vector< Circle > >;

    CircleMap circles;
    SetMap sets;

    Args split(const std::string &line)
    {
      const char *const separators = " \t\r";
      Args words;
      size_t pos = 0;
      while (pos < line.size())
      {
        pos = line.find_first_not_of(separators, pos);
        if (pos == std::string::npos)
        {
          break;
        }
        size_t end = line.find_first_of(separators, pos);
        if (end == std::string::npos)
        {
          end = line.size();
        }
        words.push_back(line.substr(pos, end - pos));
        pos = end;
      }
      return words;
    }

    bool isValidName(const std::string &name)
    {
      if (name.empty())
      {
        return false;
      }
      const char first = name.front();
      return (first >= 'a' && first <= 'z') || (first >= 'A' && first <= 'Z');
    }

    bool parseInt(const std::string &text, int &value)
    {
      try
      {
        size_t parsed = 0;
        value = std::stoi(text, &parsed);
        return parsed == text.size();
      }
      catch (const std::invalid_argument &)
      {
        return false;
      }
      catch (const std::out_of_range &)
      {
        return false;
      }
    }

    int circle(const Args &args)
    {
      if (args.size() != 4 || !isValidName(args[0]) || circles.count(args[0]) != 0)
      {
        return errInvalidCommand;
      }
      int radius = 0;
      int x = 0;
      int y = 0;
      if (!parseInt(args[1], radius) || !parseInt(args[2], x) || !parseInt(args[3], y))
      {
        return errInvalidCommand;
      }
      if (radius <= 0)
      {
        return errInvalidCommand;
      }
      circles.emplace(args[0], Circle(x, y, radius));
      return noError;
    }

    int set(const Args &args)
    {
      if (args.size() < 2 || !isValidName(args[0]) || sets.count(args[0]) != 0)
      {
        return errInvalidCommand;
      }
      int size = 0;
      if (!parseInt(args[1], size))
      {
        return errInvalidCommand;
      }
      if (size <= 0 || args.size() != static_cast< size_t >(size) + 2)
      {
        return errInvalidCommand;
      }
      std::vector< Circle > members;
      members.reserve(static_cast< size_t >(size));
      for (size_t i = 2; i < args.size(); ++i)
      {
        CircleMap::const_iterator found = circles.find(args[i]);
        if (found == circles.end())
        {
          return errInvalidCommand;
        }
        members.push_back(found->second);
      }
      sets.emplace(args[0], std::move(members));
      return noError;
    }

    const HandlerTable &handlers()
    {
      static const HandlerTable table = {
          {"circle", circle},
          {"set", set},
      };
      return table;
    }
  }

  void execute(const std::string &line)
  {
    const Args words = split(line);
    if (words.empty())
    {
      return;
    }
    const HandlerTable &table = handlers();
    HandlerTable::const_iterator found = table.find(words.front());
    if (found == table.end())
    {
      std::cout << "<UNKNOWN COMMAND>\n";
      return;
    }
    const Args args(words.begin() + 1, words.end());
    const int code = found->second(args);
    if (code == errInvalidCommand)
    {
      std::cout << "<INVALID COMMAND>\n";
    }
  }
}