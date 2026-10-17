#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>
#include "figure.hpp"
#include "monte_carlo.hpp"
#include "options.hpp"

int main(const int argc, char** const argv)
{
  constexpr int invalid_input = 1;
  constexpr int internal_error = 2;
  try
  {
    const matveev::options_t options = matveev::parseArguments(argc, argv);
    const std::vector< matveev::figure_t > figures = matveev::readFigures(std::cin);
    const matveev::areas_t areas = matveev::calculateAreas(figures, options);
    std::cout << std::setprecision(std::numeric_limits< long double >::max_digits10);
    std::cout << areas.coverage << " " << areas.intersection << "\n";
    std::cout.flush();
    if (!std::cout)
    {
      throw std::runtime_error("Failed to write the result");
    }
  }
  catch (const std::invalid_argument& error)
  {
    std::cerr << error.what() << "\n";
    return invalid_input;
  }
  catch (const std::out_of_range& error)
  {
    std::cerr << error.what() << "\n";
    return invalid_input;
  }
  catch (const std::exception& error)
  {
    std::cerr << error.what() << "\n";
    return internal_error;
  }
  return 0;
}
