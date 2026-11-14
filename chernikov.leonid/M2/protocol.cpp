#include "protocol.hpp"
#include <vector>
#include "pipe_io.hpp"
#include "shapes.hpp"

bool chernikov::sendRequest(int fd, const request_header_t &header, const std::vector< shape_data_t > &shapes)
{
  if (!writeAll(fd, &header, sizeof(header))) {
    return false;
  }
  return shapes.empty() || writeAll(fd, shapes.data(), shapes.size() * sizeof(shape_data_t));
}

bool chernikov::receiveRequest(int fd, request_header_t &header, std::vector< shape_data_t > &shapes)
{
  if (!readAll(fd, &header, sizeof(header))) {
    return false;
  }
  shapes.resize(header.count);
  return shapes.empty() || readAll(fd, shapes.data(), shapes.size() * sizeof(shape_data_t));
}

bool chernikov::sendResult(int fd, const result_t &result)
{
  return writeAll(fd, &result, sizeof(result));
}

bool chernikov::receiveResult(int fd, result_t &result)
{
  return readAll(fd, &result, sizeof(result));
}
