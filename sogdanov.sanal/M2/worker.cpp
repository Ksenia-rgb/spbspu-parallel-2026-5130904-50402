#include "worker.hpp"

#include <cerrno>
#include <chrono>
#include <cstring>
#include <limits>
#include <random>
#include <stdexcept>

#include <unistd.h>

#include "monte_carlo.hpp"

namespace sogdanov
{
  namespace
  {

    static bool readAll(const int descriptor, void *destination, const size_t size)
    {
      char *const data = static_cast< char * >(destination);
      size_t received = 0;

      while (received < size)
      {
        const ssize_t result = ::read(descriptor, data + received, size - received);

        if (result < 0)
        {
          if (errno == EINTR)
          {
            continue;
          }

          throw std::runtime_error("Cannot read request");
        }

        if (result == 0)
        {
          if (received == 0)
          {
            return false;
          }

          throw std::runtime_error("Incomplete request");
        }

        received += static_cast< size_t >(result);
      }

      return true;
    }

    static void writeAll(const int descriptor, const void *source, const size_t size)
    {
      const char *const data = static_cast< const char * >(source);
      size_t sent = 0;

      while (sent < size)
      {
        const ssize_t result = ::write(descriptor, data + sent, size - sent);

        if ((result < 0) && (errno == EINTR))
        {
          continue;
        }

        if (result <= 0)
        {
          throw std::runtime_error("Cannot write result");
        }

        sent += static_cast< size_t >(result);
      }
    }
  }
}

std::vector< unsigned char > sogdanov::makeRequest(
    const size_t calculation_id, const std::vector< Figure > &figures, const size_t threads, const size_t tries)
{
  if (figures.empty() || (tries == 0))
  {
    throw std::invalid_argument("Invalid calculation parameters");
  }

  const size_t header[4] = {calculation_id, threads, tries, figures.size()};
  const size_t figure_size = 4 * sizeof(long long);
  const size_t maximum_size = std::numeric_limits< size_t >::max();
  const size_t maximum_count = (maximum_size - sizeof(header)) / figure_size;

  if (figures.size() > maximum_count)
  {
    throw std::length_error("Request is too large");
  }

  std::vector< unsigned char > message(sizeof(header) + figures.size() * figure_size);

  std::memcpy(message.data(), header, sizeof(header));

  size_t offset = sizeof(header);

  for (const Figure &figure : figures)
  {
    const long long parameters[4]
        = {figure.getHorizontalRadius(), figure.getVerticalRadius(), figure.getCenterX(), figure.getCenterY()};

    std::memcpy(message.data() + offset, parameters, sizeof(parameters));
    offset += sizeof(parameters);
  }

  return message;
}

void sogdanov::runWorker(const int input, const int output, const size_t seed)
{
  std::mt19937_64 generator(seed);
  size_t header[4] = {};

  while (readAll(input, header, sizeof(header)))
  {
    const size_t calculation_id = header[0];
    const size_t threads = header[1];
    const size_t tries = header[2];
    const size_t figure_count = header[3];
    std::vector< Figure > figures;

    if ((figure_count == 0) || (figure_count > figures.max_size()))
    {
      throw std::runtime_error("Invalid request size");
    }

    figures.reserve(figure_count);

    for (size_t index = 0; index < figure_count; ++index)
    {
      long long parameters[4] = {};

      if (!readAll(input, parameters, sizeof(parameters)))
      {
        throw std::runtime_error("Missing figure parameters");
      }

      figures.emplace_back(parameters[0], parameters[1], parameters[2], parameters[3]);
    }

    const size_t calculation_seed = static_cast< size_t >(generator());
    const auto start = std::chrono::steady_clock::now();
    const Areas areas = estimateAreas(figures, threads, tries, calculation_seed);
    const auto finish = std::chrono::steady_clock::now();

    const double milliseconds = std::chrono::duration< double, std::milli >(finish - start).count();
    const ResultSlot result = {calculation_id, milliseconds, static_cast< double >(areas.coverage)};

    unsigned char message[sizeof(size_t) + 2 * sizeof(double)] = {};

    std::memcpy(message, &result.calculation_id, sizeof(result.calculation_id));
    std::memcpy(message + sizeof(result.calculation_id), &result.milliseconds, sizeof(result.milliseconds));
    std::memcpy(
        message + sizeof(result.calculation_id) + sizeof(result.milliseconds), &result.area, sizeof(result.area));

    writeAll(output, message, sizeof(message));
  }
}
