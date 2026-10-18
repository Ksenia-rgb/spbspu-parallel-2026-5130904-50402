#include <cstdlib>
#include <future>
#include <iostream>
#include <random>

namespace karpenkov {
constexpr size_t count_of_args = 4;
constexpr size_t invalid_args_count_error = 1;
constexpr size_t base_of_ten = 10;
constexpr size_t invalid_args = 2;
struct Circle {
public:
  Circle(int x, int y, int radius) : x(x), y(y), radius(radius) {}
  int getRadius(){
    return radius;
  }
private:
  int x;
  int y;
  int radius;
};
size_t stringToSizeT(const char *lms) {
  char *end = nullptr;
  const unsigned long long rms = std::strtoull(lms, &end, base_of_ten);
  return static_cast<size_t>(rms);
}
size_t monteCarloGenerator(const size_t &count, const int &radius,
                           const size_t &seed) {
  std::default_random_engine randomGenerator(seed);
  std::uniform_int_distribution<int> dist_x(0, 2 * radius);
  std::uniform_int_distribution<int> dist_y(0, 2 * radius);

  size_t N_in = 0;
  size_t x = 0, y = 0;
  for (size_t i = 0; i < count; ++i) {
    x = dist_x(randomGenerator);
    y = dist_y(randomGenerator);

    if (x * x + y * y <= radius * radius) {
      N_in++;
    }
  }
  return N_in;
}
double getCircleSquare(Circle a, const size_t &threads, const size_t &tries,
                       const size_t &seed) {
  size_t count = tries / threads;
  std::vector<std::future<size_t>> results;
  for (size_t i = 0; i < threads; ++i) {
    results.push_back(std::async(std::launch::async, monteCarloGenerator, count,
                                 a.getRadius(), seed));
  }
  size_t N_in = 0;
  for(auto it = results.begin(); it != results.end(); ++it){
    N_in += it->get();
  }
  return 4*a.getRadius()*a.getRadius()*N_in/tries;
}
}; 

int main(int argc, const char *argv[]) {
  if (argc < karpenkov::count_of_args) {
    std::cerr << "Invalid count of arguments";
    return karpenkov::invalid_args_count_error;
  }
  size_t threads = 0, tries = 0, seed = 0;
  try {
    threads = karpenkov::stringToSizeT(argv[1]);
    tries = karpenkov::stringToSizeT(argv[2]);
    seed = karpenkov::stringToSizeT(argv[3]);
  } catch (...) {
    std::cerr << "Invalid argument";
    return karpenkov::invalid_args;
  } 
}
