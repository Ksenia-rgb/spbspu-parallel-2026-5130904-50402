#include "calculation.hpp"
#include <iomanip>
#include <ios>
#include <ostream>

std::ostream& pozdnyakov::operator<<(std::ostream& out, const calculation_t& calculation)
{
  const int precision = 3;
  out << calculation.process << ' ';
  switch (calculation.status)
  {
  case calculation_status_t::in_progress:
    return out << "<IN PROGRESS>";
  case calculation_status_t::canceled:
    return out << "<CANCELED>";
  case calculation_status_t::done:
    break;
  }
  const std::ios::fmtflags flags = out.flags();
  const std::streamsize old_precision = out.precision();
  out << std::fixed << std::setprecision(precision);
  out << calculation.result.milliseconds << ' ' << calculation.result.area;
  out.flags(flags);
  out.precision(old_precision);
  return out;
}
