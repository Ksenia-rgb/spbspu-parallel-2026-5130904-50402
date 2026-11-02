#ifndef MATVEEV_CATALOG_HPP
#define MATVEEV_CATALOG_HPP

#include <map>
#include <string>
#include <vector>
#include "shape.hpp"

namespace matveev
{
  class Catalog final
  {
  public:
    void addShape(const std::string &name, const shape_t &shape);
    void addSet(const std::string &name, const std::vector< std::string > &names);
    const shape_t &getShape(const std::string &name) const;
    std::vector< shape_t > getSet(const std::string &name) const;
    void rotate(const std::string &name, double angle);

  private:
    std::map< std::string, shape_t > shapes_ = {};
    std::map< std::string, std::vector< std::string > > sets_ = {};
  };
}

#endif
