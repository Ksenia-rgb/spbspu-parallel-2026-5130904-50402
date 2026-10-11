#include "circle.hpp"
#include <ios>
#include <istream>
#include <stdexcept>
#include <vector>

std::istream& makarov::operator>>(std::istream& in, Circle& circle)
{
  const std::istream::sentry sentry(in);
  if (!sentry)
  {
    return in;
  }
  long long r = 0;
  long long skip = 0;
  long long x = 0;
  long long y = 0;
  in >> r >> skip >> x >> y;
  if (in && r <= 0)
  {
    in.setstate(std::ios::failbit);
  }
  if (in)
  {
    circle = Circle{static_cast< double >(r), static_cast< double >(x), static_cast< double >(y)};
  }
  return in;
}

std::vector< makarov::Circle > makarov::readCircles(std::istream& in)
{
  std::vector< Circle > circles;
  while (!(in >> std::ws).eof())
  {
    Circle c{};
    if (!(in >> c))
    {
      throw std::invalid_argument("unable to parse figure parameters");
    }
    circles.push_back(c);
  }
  return circles;
}
