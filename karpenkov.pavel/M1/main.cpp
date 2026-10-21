#include <cstdlib>
#include <future>
#include <iostream>
#include <random>
#include <vector>
#include <cctype>
#include <cerrno>
#include <stdexcept>


namespace karpenkov {
constexpr size_t count_of_args = 4;
constexpr size_t count_of_args_without_seed = 3;
constexpr size_t invalid_args_count_error = 1;
constexpr size_t base_of_ten = 10;
constexpr size_t invalid_args = 2;
constexpr size_t invalid_input = 3;
struct Circle {
public:
  Circle(int x, int y, int radius) : x(x), y(y), radius(radius) {}
  int getRadius() const{
    return radius;
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
size_t monteCarloGenerator(const size_t &count, const int &radius,
                           const size_t &seed) {
  std::default_random_engine randomGenerator(seed);
  std::uniform_int_distribution<int> dist(0, 2 * radius);

  size_t N_in = 0;
  int x = 0, y = 0;
  for (size_t i = 0; i < count; ++i) {
    x = dist(randomGenerator);
    y = dist(randomGenerator);

    int dx = x - radius;
    int dy = y - radius;
    if (dx * dx + dy * dy <= radius * radius) {
      N_in++;
    }
  }
  return N_in;
}
double getCircleSquare(Circle a, const size_t &threads, const size_t &tries,
                       const size_t &seed) {
  size_t base = tries / threads;
  size_t rem  = tries % threads;
  int radius = a.getRadius();
  std::vector<std::future<size_t>> results;
  for (size_t i = 0; i < threads; ++i) {
    size_t count = base + (i < rem ? 1 : 0);
    results.push_back(std::async(std::launch::async, monteCarloGenerator,
                                 count, radius, seed + i));
  }
  size_t N_in = 0;
  for(auto it = results.begin(); it != results.end(); ++it){
    N_in += it->get();
  }
  double r = static_cast<double>(radius);
  return 4.0 * r * r * static_cast<double>(N_in) / static_cast<double>(tries);
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
