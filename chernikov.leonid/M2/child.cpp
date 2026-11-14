#include "child.hpp"
#include <chrono>
#include <cstddef>
#include <ratio>
#include <vector>
#include "monte_carlo.hpp"
#include "protocol.hpp"
#include "shapes.hpp"

void chernikov::runChild(int request_fd, int result_fd, size_t seed)
{
  request_header_t header = {};
  std::vector< shape_data_t > data;
  while (receiveRequest(request_fd, header, data)) {
    const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    shapes_t shapes;
    shapes.reserve(data.size());
    for (const shape_data_t &d : data) {
      shapes.push_back(makeShape(d));
    }
    const double area = coverArea(shapes, header.threads, header.tries, seed);
    const std::chrono::duration< double, std::milli > ms = std::chrono::steady_clock::now() - start;
    const result_t result = {header.calc_id, ms.count(), area};
    if (!sendResult(result_fd, result)) {
      return;
    }
  }
}
