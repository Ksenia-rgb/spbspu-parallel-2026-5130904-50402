#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>
#include "areas.hpp"
#include "args.hpp"
#include "circle.hpp"

int main(int argc, char** argv)
{
  try
  {
    const makarov::Parameters params = makarov::parseParameters(argc, argv);
    const std::vector< makarov::Circle > circles = makarov::readCircles(std::cin);
    const makarov::Areas areas = makarov::getAreas(circles, params.threads, params.tries, params.seed);
    std::cout << std::setprecision(std::numeric_limits< double >::max_digits10);
    std::cout << areas.any << " " << areas.all << "\n";
  }
  catch (const std::invalid_argument& e)
  {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }
  catch (const std::exception& e)
  {
    std::cerr << "Error: " << e.what() << "\n";
    return 2;
  }
}
