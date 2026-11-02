#ifndef MATVEEV_COMMANDS_HPP
#define MATVEEV_COMMANDS_HPP

#include <iosfwd>
#include <string>

namespace matveev
{
  class Catalog;
  class Manager;
  void executeLine(const std::string &line, Catalog &catalog, Manager &manager, std::ostream &out);
}

#endif
