#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include <string>

namespace lavrentev
{
  class Circle
  {
  public:
    Circle(int r, int x, int y);
    Circle(std::string name, int r, int x, int y);

    int getX() const;
    int getY() const;
    int getRadius() const;

  private:
    std::string name_;
    int radius_;
    int x_;
    int y_;
  };
}

#endif
