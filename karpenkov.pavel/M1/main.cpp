#include <cstdlib>
#include <future>
#include <iostream>
#include <random>
#include <vector>
#include <cctype>
#include <cerrno>
#include <stdexcept>
#include <algorithm>


namespace karpenkov {
constexpr size_t count_of_args = 4;
constexpr size_t count_of_args_without_seed = 3;
constexpr size_t invalid_args_count_error = 1;
constexpr size_t base_of_ten = 10;
constexpr size_t invalid_args = 2;
constexpr size_t invalid_input = 3;
struct Circle {
public:
  Circle(int x, int y, int radius) : x(x), y(y), radius(radius) {
    if (radius <= 0){
      throw std::invalid_argument("Radius must be positive");
    }
  }
  int getRadius() const{
    return radius;
  }
  double left() const {
    return static_cast<double>(x) - radius;
  }
  double right() const {
    return static_cast<double>(x) + radius;
  }
  double bottom() const {
    return static_cast<double>(y) - radius;
  }
  double top() const {
    return static_cast<double>(y) + radius;
  }
  bool contains(double px, double py) const {
    const double dx = px - x;
    const double dy = py - y;
    const double r = radius;
    return dx * dx + dy * dy <= r * r;
  }
private:
  int x;
  int y;
  int radius;
};
size_t stringToSizeT(const char *lms) {
  if(*lms == '-'){
    throw std::invalid_argument("Negative argument is not allowed");
  }
  char *end = nullptr;
  errno = 0;
  const unsigned long long rms = std::strtoull(lms, &end, base_of_ten);
  if(*end != '\0' || errno == ERANGE){
    throw std::invalid_argument("Bad number format");
  }
  return static_cast<size_t>(rms);
}
struct Box {
  double minX, maxX, minY, maxY;
  double area() const {
    return (maxX - minX) * (maxY - minY);
  }
};
Box boundingBox(const std::vector<Circle> &circles) {
  Box box{circles[0].left(), circles[0].right(), circles[0].bottom(),
          circles[0].top()};
  for (const Circle &c : circles) {
    box.minX = std::min(box.minX, c.left());
    box.maxX = std::max(box.maxX, c.right());
    box.minY = std::min(box.minY, c.bottom());
    box.maxY = std::max(box.maxY, c.top());
  }
  return box;
}
std::pair<size_t, size_t> monteCarloGenerator(const std::vector<Circle> &circles,
                                              const Box &box, size_t count,
                                              size_t seed) {
  std::default_random_engine randomGenerator(seed);
  std::uniform_real_distribution<double> distX(box.minX, box.maxX);
  std::uniform_real_distribution<double> distY(box.minY, box.maxY);

  size_t inUnion = 0;
  size_t inIntersection = 0;
  for (size_t i = 0; i < count; ++i) {
    const double x = distX(randomGenerator);
    const double y = distY(randomGenerator);

    size_t inside = 0;
    for (const Circle &c : circles) {
      if (c.contains(x, y)) {
        ++inside;
      }
    }
    if (inside > 0) {
      ++inUnion;
    }
    if (inside == circles.size()) {
      ++inIntersection;
    }
  }
  return {inUnion, inIntersection};
}
struct Areas {
  double unionArea;
  double intersectionArea;
};
Areas getAreas(const std::vector<Circle> &circles, size_t threads,
               size_t tries, size_t seed) {
  const Box box = boundingBox(circles);
  const size_t base = tries / threads;
  const size_t rem = tries % threads;

  std::vector<std::future<std::pair<size_t, size_t>>> results;
  for (size_t i = 0; i < threads; ++i) {
    const size_t count = base + (i < rem ? 1 : 0);
    results.push_back(std::async(std::launch::async, monteCarloGenerator,
                                 std::cref(circles), std::cref(box), count,
                                 seed + i));
  }

  size_t inUnion = 0;
  size_t inIntersection = 0;
  for (auto &f : results) {
    const std::pair<size_t, size_t> part = f.get();
    inUnion += part.first;
    inIntersection += part.second;
  }

  const double boxArea = box.area();
  const double total = static_cast<double>(tries);
  return {boxArea * static_cast<double>(inUnion) / total,
          boxArea * static_cast<double>(inIntersection) / total};
}
bool readCircles(std::istream& in, std::vector<Circle>& out){
  int radius = 0, x = 0, y = 0;
  while (in >> radius){
    if (!(in >> x >> y)){
      return false;
    }
    out.emplace_back(x,y,radius);
  }
  return in.eof();
}
}

int main(int argc, const char *argv[]) {
  if (argc != karpenkov::count_of_args_without_seed && argc != karpenkov::count_of_args) {
    std::cerr << "Invalid count of arguments\n";
    return karpenkov::invalid_args_count_error;
  }
  size_t threads = 0, tries = 0, seed = 0;
  try {
    threads = karpenkov::stringToSizeT(argv[1]);
    tries = karpenkov::stringToSizeT(argv[2]);
    if(argc == karpenkov::count_of_args){
      seed = karpenkov::stringToSizeT(argv[3]);
    }
  } catch (const std::invalid_argument& e) {
    std::cerr << "Invalid argument: " << e.what() << '\n';
    return karpenkov::invalid_args;
  }
  if (tries == 0){
    std::cerr << "Tries must be postivie number";
    return karpenkov::invalid_args;
  }
  if (threads == 0){
    threads = 1;
  }
  std::vector<karpenkov::Circle> circles;
  if(!karpenkov::readCircles(std::cin, circles)){
    std::cerr << "Invalid input\n";
    return karpenkov::invalid_input;
  }


}
