#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include <map>
#include <string>
#include <vector>
#include "calculation.hpp"
#include "shapes.hpp"

namespace pozdnyakov
{

  class Registry
  {
  public:
    void addShape();
    void addSet();
    void addCalculation();

    const ellipse_t& getShape(const std::string& name) const;
    ellipses_t getSet(const std::string& name) const;
    calculation_t& getCalculation(const std::string& name);
    const calculation_t& getCalculation(const std::string& name) const;

  private:
  };

  bool isValidName(const std::string& name);
}

#endif
