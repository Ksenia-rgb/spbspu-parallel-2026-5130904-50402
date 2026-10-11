#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include "arguments.hpp"
#include "monte-carlo.hpp"
#include "shapes.hpp"

namespace
{
  pozdnyakov::ellipses_t readEllipses(std::istream& in)
  {
    pozdnyakov::ellipses_t ellipses;
    while (!(in >> std::ws).eof())
    {
      pozdnyakov::ellipse_t ellipse{{0.0, 0.0}, 0.0, 0.0};
      if (!(in >> ellipse))
      {
        throw std::invalid_argument("figure parameters must be four integers");
      }
      ellipses.push_back(ellipse);
    }
    return ellipses;
  }
}

int main(int argc, char** argv)
{
  const int invalid_input_code = 1;
  const int internal_error_code = 2;
  try
  {
    const pozdnyakov::arguments_t args = pozdnyakov::parseArguments(argc, argv);
    const pozdnyakov::ellipses_t ellipses = readEllipses(std::cin);
    const pozdnyakov::areas_t areas = pozdnyakov::computeAreas(ellipses, args);
    std::cout << std::setprecision(std::numeric_limits< double >::max_digits10);
    std::cout << areas.coverage << ' ' << areas.intersection << '\n';
  }
  catch (const std::invalid_argument& error)
  {
    std::cerr << error.what() << '\n';
    return invalid_input_code;
  }
  catch (const std::exception& error)
  {
    std::cerr << error.what() << '\n';
    return internal_error_code;
  }
  return 0;
}
