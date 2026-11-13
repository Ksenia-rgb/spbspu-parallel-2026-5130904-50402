#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <cstddef>
#include <vector>
#include "shapes.hpp"

namespace chernikov {
  struct request_header_t {
    size_t calc_id;
    size_t threads;
    size_t tries;
    size_t count; // за заголовком идут count штук shape_data_t
  };
  struct result_t {
    size_t calc_id;
    double ms;
    double area;
  };

  // TODO: protocol.cpp через writeAll / readAll
  bool sendRequest(int fd, const request_header_t &header, const std::vector< shape_data_t > &shapes);
  bool receiveRequest(int fd, request_header_t &header, std::vector< shape_data_t > &shapes);
  bool sendResult(int fd, const result_t &result);
  bool receiveResult(int fd, result_t &result);
}
#endif