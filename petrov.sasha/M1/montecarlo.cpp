#include "montecarlo.hpp"

#include <algorithm>
#include <istream>
#include <stdexcept>

std::istream &petrov::operator>>(std::istream &in, p_t &point)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return in;
  }
  return in >> point.x >> point.y;
}

std::istream &petrov::operator>>(std::istream &in, circle_t &circle)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return in;
  }
  double ignoredParameter = 0.0;
  return in >> circle.radius >> ignoredParameter >> circle.center;
}
