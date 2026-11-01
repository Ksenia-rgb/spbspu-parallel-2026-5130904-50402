#ifndef POLYGON_HPP
#define POLYGON_HPP

namespace lavrentev
{
  class Polygon
  {
  public:
    Polygon(int x1, int y1, int x2, int y2);

    int getMaxX() const;
    int getMaxY() const;
    int getMinX() const;
    int getMinY() const;

    void setMaxX(int x);
    void setMaxY(int y);
    void setMinX(int x);
    void setMinY(int y);

  private:
    int min_x_;
    int min_y_;
    int max_x_;
    int max_y_;
  };
}

#endif
