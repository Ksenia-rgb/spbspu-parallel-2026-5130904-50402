#ifndef CIRCLE_HPP
#define CIRCLE_HPP

namespace lavrentev
{
  class Circle
  {
  public:
    Circle(int r, int x, int y);

    int getX() const;
    int getY() const;
    int getRadius() const;

  private:
    int radius_;
    int x_;
    int y_;
  };
}

#endif
