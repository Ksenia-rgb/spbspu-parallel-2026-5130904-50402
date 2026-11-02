#include "commands.hpp"
#include <exception>
#include <ostream>
#include <stdexcept>
#include "parse.hpp"
#include "catalog.hpp"
#include "manager.hpp"

namespace matveev
{
  namespace detail
  {
    static bool executeCommand(const Tokens &tokens, Catalog &catalog, Manager &manager, std::ostream &out);
  }
}

bool matveev::detail::executeCommand(const Tokens &tokens, Catalog &catalog, Manager &manager, std::ostream &out)
{
  constexpr std::size_t nameIndex = 1;
  constexpr std::size_t firstValue = 2;
  constexpr std::size_t secondValue = 3;
  constexpr std::size_t thirdValue = 4;
  constexpr std::size_t fourthValue = 5;
  constexpr std::size_t unaryCount = 2;
  constexpr std::size_t binaryCount = 3;
  constexpr std::size_t circleCount = 5;
  constexpr std::size_t extendedCount = 6;
  const std::string &command = tokens.front();
  if (command == "circle")
  {
    requireCount(tokens, circleCount);
    const long long radius = parseInteger(tokens[firstValue]);
    const long long x = parseInteger(tokens[secondValue]);
    const long long y = parseInteger(tokens[thirdValue]);
    catalog.addShape(tokens[nameIndex], {radius, radius, x, y, 0.0, false});
  }
  else if (command == "set")
  {
    if (tokens.size() < binaryCount)
    {
      throw std::invalid_argument("Incomplete set");
    }
    const std::uint64_t count = parseUnsigned(tokens[firstValue]);
    if ((count == 0) || (count != tokens.size() - binaryCount))
    {
      throw std::invalid_argument("Invalid set size");
    }
    const std::vector< std::string > names(tokens.begin() + binaryCount, tokens.end());
    catalog.addSet(tokens[nameIndex], names);
  }
  else if (command == "show")
  {
    requireCount(tokens, unaryCount);
    printShape(out, catalog.getShape(tokens[nameIndex]));
  }
  else if (command == "showset")
  {
    requireCount(tokens, unaryCount);
    for (const shape_t &shape: catalog.getSet(tokens[nameIndex]))
    {
      printShape(out, shape);
    }
  }
  else if (command == "frame")
  {
    requireCount(tokens, unaryCount);
    printFrame(out, getFrame(catalog.getShape(tokens[nameIndex])));
  }
  else if (command == "frameset")
  {
    requireCount(tokens, unaryCount);
    printFrame(out, getFrame(catalog.getSet(tokens[nameIndex])));
  }
  else if (command == "spawn")
  {
    requireCount(tokens, binaryCount);
    manager.spawn(tokens[nameIndex], parseUnsigned(tokens[firstValue]));
  }
  else if (command == "area-on")
  {
    requireCount(tokens, extendedCount);
    const std::vector< shape_t > shapes = catalog.getSet(tokens[secondValue]);
    const std::uint64_t threads = parseUnsigned(tokens[thirdValue]);
    const std::uint64_t tries = parseUnsigned(tokens[fourthValue]);
    manager.submit(tokens[nameIndex], tokens[firstValue], shapes, threads, tries);
  }
  else if ((command == "status") || (command == "wait"))
  {
    requireCount(tokens, unaryCount);
    if (command == "wait")
    {
      manager.wait(tokens[nameIndex]);
    }
    else
    {
      manager.pump(0);
    }
    manager.printStatus(tokens[nameIndex], out);
  }
  else
  {
    return false;
  }
  return true;
}

void matveev::executeLine(const std::string &line, Catalog &catalog, Manager &manager, std::ostream &out)
{
  try
  {
    const Tokens tokens = splitLine(line);
    if (!tokens.empty() && !detail::executeCommand(tokens, catalog, manager, out))
    {
      out << "<UNKNOWN COMMAND>\n";
    }
  }
  catch (const std::exception &)
  {
    out << "<INVALID COMMAND>\n";
  }
  out.flush();
  if (!out)
  {
    throw std::runtime_error("Cannot write output");
  }
}
