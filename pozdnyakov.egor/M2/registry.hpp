#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include <map>
#include <string>
#include <vector>
#include "calculation.hpp"
#include "shapes.hpp"

namespace pozdnyakov
{
  using names_t = std::vector< std::string >;
  using shapes_t = std::map< std::string, ellipse_t >;
  using sets_t = std::map< std::string, names_t >;
  using calculations_t = std::map< std::string, calculation_t >;

  class Registry
  {
  public:
    void addShape(const std::string& name, const ellipse_t& shape);
    void addSet(const std::string& name, const names_t& shapes);
    void addCalculation(const std::string& name, const std::string& process);

    bool hasCalculation(const std::string& name) const;
    const ellipse_t& getShape(const std::string& name) const;
    ellipses_t getSet(const std::string& name) const;
    calculation_t& getCalculation(const std::string& name);
    const calculation_t& getCalculation(const std::string& name) const;

  private:
    shapes_t shapes_;
    sets_t sets_;
    calculations_t calculations_;
  };

  bool isValidName(const std::string& name);
}

#endif
