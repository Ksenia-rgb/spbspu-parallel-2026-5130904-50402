#ifndef SHAPES_HPP
#define SHAPES_HPP

#include <memory>
#include <ostream>

namespace chernikov {
  struct point_t {
    double x;
    double y;
  };
  struct frame_t {
    point_t min;
    point_t max;
  };

  struct shape_data_t {
    int type;
    double rx;
    double ry;
    double x;
    double y;
    double angle;
  };

  struct Shape {
    virtual ~Shape() = default;
    virtual bool contains(const point_t &p) const = 0;
    virtual frame_t getFrame() const = 0;
    virtual void print(std::ostream &out) const = 0;
    virtual shape_data_t toData() const = 0;
  };

  struct Circle: Shape {
    Circle(double radius, const point_t &c);
    bool contains(const point_t &p) const override;
    frame_t getFrame() const override;
    void print(std::ostream &out) const override;
    shape_data_t toData() const override;

    double r;
    point_t center;
  };

  std::unique_ptr< Shape > makeShape(const shape_data_t &data);
  frame_t unite(const frame_t &a, const frame_t &b);
  void printFrame(std::ostream &out, const frame_t &frame);
}
#endif
