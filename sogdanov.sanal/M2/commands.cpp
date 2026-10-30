#include "commands.hpp"

#include <algorithm>
#include <cerrno>
#include <cctype>
#include <cstring>
#include <iomanip>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <utility>

#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include "monte_carlo.hpp"

namespace sogdanov
{
  namespace
  {
    static void require(const bool condition)
    {
      if (!condition)
      {
        throw std::invalid_argument("Invalid command");
      }
    }

    static bool validName(const std::string& name)
    {
      if (name.empty())
      {
        return false;
      }

      const char first = name[0];
      const bool lowercase = (first >= 'a') && (first <= 'z');
      const bool uppercase = (first >= 'A') && (first <= 'Z');

      return lowercase || uppercase;
    }

    static long long parseInteger(const std::string& token)
    {
      size_t position = 0;
      const long long value = std::stoll(token, &position);

      require(position == token.size());

      return value;
    }

    static size_t parseSize(const std::string& token)
    {
      require(!token.empty());
      require(token[0] != '-');

      size_t position = 0;
      const unsigned long long value = std::stoull(token, &position);

      require(position == token.size());
      require(value <= std::numeric_limits< size_t >::max());

      return static_cast< size_t >(value);
    }

    static void printFigure(std::ostream& output, const Figure& figure)
    {
      output << figure.getHorizontalRadius() << " (" << figure.getCenterX() << ' ' << figure.getCenterY() << ")\n";
    }

    static void printBounds(std::ostream& output, const Bounds& bounds)
    {
      const auto flags = output.flags();
      const auto precision = output.precision();

      output << std::defaultfloat;
      output << std::setprecision(std::numeric_limits< long double >::max_digits10);
      output << '(' << bounds.min_x << ' ' << bounds.min_y << ") (" << bounds.max_x << ' ' << bounds.max_y << ")\n";

      output.flags(flags);
      output.precision(precision);
    }

    static void printStatus(std::ostream& output, const Calc& calculation)
    {
      const auto flags = output.flags();
      const auto precision = output.precision();

      output << calculation.process_name << ' ';

      if (calculation.done_)
      {
        output << std::fixed << std::setprecision(3) << calculation.milliseconds << ' ' << calculation.area << '\n';
      }
      else
      {
        output << "<IN PROGRESS>\n";
      }

      output.flags(flags);
      output.precision(precision);
    }

    static void setNonblocking(const int descriptor)
    {
      const int flags = ::fcntl(descriptor, F_GETFL);

      if (flags == -1)
      {
        throw std::runtime_error("Cannot get pipe flags");
      }

      if (::fcntl(descriptor, F_SETFL, flags | O_NONBLOCK) == -1)
      {
        throw std::runtime_error("Cannot make pipe nonblocking");
      }
    }

    static void receiveResults(Data& data, Worker& worker)
    {
      while (true)
      {
        const size_t remaining = sizeof(worker.reply) - worker.received;
        const ssize_t count = ::read(worker.result_rd, worker.reply + worker.received, remaining);

        if (count > 0)
        {
          worker.received += static_cast< size_t >(count);

          if (worker.received != sizeof(worker.reply))
          {
            continue;
          }

          ResultSlot result = {};

          std::memcpy(&result.calculation_id, worker.reply, sizeof(result.calculation_id));
          std::memcpy(&result.milliseconds, worker.reply + sizeof(result.calculation_id), sizeof(result.milliseconds));
          std::memcpy(&result.area, worker.reply + sizeof(result.calculation_id) + sizeof(result.milliseconds),
              sizeof(result.area));

          const auto found = std::find_if(data.calcs.begin(), data.calcs.end(),
              [&result](const auto& entry)
              {
                return entry.second.id == result.calculation_id;
              });

          if ((found == data.calcs.end()) || found->second.done_)
          {
            throw std::runtime_error("Unexpected calculation result");
          }

          Calc& calculation = found->second;

          calculation.milliseconds = result.milliseconds;
          calculation.area = result.area;
          calculation.done_ = true;
          worker.received = 0;
        }
        else if (count == 0)
        {
          throw std::runtime_error("Worker process disconnected");
        }
        else if (errno == EINTR)
        {
          continue;
        }
        else if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
        {
          return;
        }
        else
        {
          throw std::runtime_error("Cannot receive calculation result");
        }
      }
    }

    static void sendRequests(Worker& worker)
    {
      while (!worker.outgoing.empty())
      {
        const std::vector< unsigned char >& message = worker.outgoing.front();
        const size_t remaining = message.size() - worker.sent;
        const ssize_t count = ::write(worker.wr, message.data() + worker.sent, remaining);

        if (count > 0)
        {
          worker.sent += static_cast< size_t >(count);

          if (worker.sent == message.size())
          {
            worker.outgoing.pop_front();
            worker.sent = 0;
          }
        }
        else if ((count < 0) && (errno == EINTR))
        {
          continue;
        }
        else if ((count < 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        {
          return;
        }
        else
        {
          throw std::runtime_error("Cannot send calculation request");
        }
      }
    }

    static void pumpAll(Data& data)
    {
      for (auto& entry : data.workers)
      {
        receiveResults(data, entry.second);
        sendRequests(entry.second);
      }
    }
  }
}

sogdanov::Arguments sogdanov::tokenize(const std::string& line)
{
  Arguments arguments;
  size_t position = 0;

  while (position < line.size())
  {
    const unsigned char current = static_cast< unsigned char >(line[position]);

    if (std::isspace(current))
    {
      ++position;
      continue;
    }

    const size_t start = position;

    while (position < line.size())
    {
      const unsigned char symbol = static_cast< unsigned char >(line[position]);

      if (std::isspace(symbol))
      {
        break;
      }

      ++position;
    }

    arguments.push_back(line.substr(start, position - start));
  }

  return arguments;
}

void sogdanov::cmdCircle(const Arguments& arguments, std::ostream&, Data& data)
{
  require(arguments.size() == 4);

  const std::string& name = arguments[0];
  const long long radius = parseInteger(arguments[1]);
  const long long x = parseInteger(arguments[2]);
  const long long y = parseInteger(arguments[3]);

  require(validName(name));
  require(radius > 0);
  require(data.figures.find(name) == data.figures.end());

  data.figures.emplace(name, Figure(radius, radius, x, y));
}

void sogdanov::cmdSet(const Arguments& arguments, std::ostream&, Data& data)
{
  require(arguments.size() >= 2);

  const std::string& name = arguments[0];
  const size_t count = parseSize(arguments[1]);

  require(validName(name));
  require(count > 0);
  require(count == arguments.size() - 2);
  require(data.sets.find(name) == data.sets.end());

  std::vector< Figure > figures;

  figures.reserve(count);

  for (size_t index = 0; index < count; ++index)
  {
    const auto found = data.figures.find(arguments[index + 2]);

    require(found != data.figures.end());
    figures.push_back(found->second);
  }

  data.sets.emplace(name, std::move(figures));
}

void sogdanov::cmdShow(const Arguments& arguments, std::ostream& output, Data& data)
{
  require(arguments.size() == 1);

  const auto found = data.figures.find(arguments[0]);

  require(found != data.figures.end());
  printFigure(output, found->second);
}

void sogdanov::cmdShowSet(const Arguments& arguments, std::ostream& output, Data& data)
{
  require(arguments.size() == 1);

  const auto found = data.sets.find(arguments[0]);

  require(found != data.sets.end());

  for (const Figure& figure : found->second)
  {
    printFigure(output, figure);
  }
}

void sogdanov::cmdFrame(const Arguments& arguments, std::ostream& output, Data& data)
{
  require(arguments.size() == 1);

  const auto found = data.figures.find(arguments[0]);

  require(found != data.figures.end());
  printBounds(output, found->second.getBounds());
}

void sogdanov::cmdFrameSet(const Arguments& arguments, std::ostream& output, Data& data)
{
  require(arguments.size() == 1);

  const auto found = data.sets.find(arguments[0]);

  require(found != data.sets.end());
  printBounds(output, findGeneralBounds(found->second));
}

void sogdanov::cmdSpawn(const Arguments& arguments, std::ostream&, Data& data)
{
  require(arguments.size() == 2);

  const std::string& name = arguments[0];
  const size_t seed = parseSize(arguments[1]);

  require(validName(name));
  require(data.workers.find(name) == data.workers.end());

  const auto inserted = data.workers.emplace(name, Worker{});
  Worker& worker = inserted.first->second;
  int request_pipe[2] = {-1, -1};
  int result_pipe[2] = {-1, -1};

  try
  {
    if ((::pipe(request_pipe) == -1) || (::pipe(result_pipe) == -1))
    {
      throw std::runtime_error("Cannot create pipe");
    }

    setNonblocking(request_pipe[1]);
    setNonblocking(result_pipe[0]);

    const pid_t pid = ::fork();

    if (pid == -1)
    {
      throw std::runtime_error("Cannot create process");
    }

    if (pid == 0)
    {
      ::close(request_pipe[1]);
      ::close(result_pipe[0]);

      for (const auto& entry : data.workers)
      {
        if (entry.second.wr != -1)
        {
          ::close(entry.second.wr);
        }

        if (entry.second.result_rd != -1)
        {
          ::close(entry.second.result_rd);
        }
      }

      int exit_code = 0;

      try
      {
        runWorker(request_pipe[0], result_pipe[1], seed);
      }
      catch (...)
      {
        exit_code = 1;
      }

      ::close(request_pipe[0]);
      ::close(result_pipe[1]);
      ::_exit(exit_code);
    }

    worker.pid = pid;
    worker.wr = request_pipe[1];
    worker.result_rd = result_pipe[0];

    ::close(request_pipe[0]);
    ::close(result_pipe[1]);
  }
  catch (...)
  {
    for (const int descriptor : request_pipe)
    {
      if (descriptor != -1)
      {
        ::close(descriptor);
      }
    }

    for (const int descriptor : result_pipe)
    {
      if (descriptor != -1)
      {
        ::close(descriptor);
      }
    }

    data.workers.erase(inserted.first);
    throw;
  }
}

void sogdanov::cmdAreaOn(const Arguments& arguments, std::ostream&, Data& data)
{
  const size_t argument_count = 5;
  require(arguments.size() == argument_count);

  const std::string& process_name = arguments[0];
  const std::string& calculation_name = arguments[1];
  const std::string& set_name = arguments[2];
  const size_t threads = parseSize(arguments[3]);
  const size_t tries = parseSize(arguments[4]);

  require(validName(calculation_name));
  require(tries > 0);
  require(data.calcs.find(calculation_name) == data.calcs.end());

  const auto process = data.workers.find(process_name);
  const auto set = data.sets.find(set_name);

  require(process != data.workers.end());
  require(set != data.sets.end());

  if (data.next_calculation_id == std::numeric_limits< size_t >::max())
  {
    throw std::overflow_error("Too many calculations");
  }

  const size_t id = data.next_calculation_id;
  std::vector< unsigned char > request = makeRequest(id, set->second, threads, tries);
  const Calc calculation = {id, process_name, false, 0.0, 0.0};
  const auto inserted = data.calcs.emplace(calculation_name, calculation);

  try
  {
    process->second.outgoing.push_back(std::move(request));
  }
  catch (...)
  {
    data.calcs.erase(inserted.first);
    throw;
  }

  ++data.next_calculation_id;
}

void sogdanov::cmdStatus(const Arguments& arguments, std::ostream& output, Data& data)
{
  require(arguments.size() == 1);

  const auto found = data.calcs.find(arguments[0]);

  require(found != data.calcs.end());

  pumpAll(data);
  printStatus(output, found->second);
}

void sogdanov::cmdWait(const Arguments& arguments, std::ostream& output, Data& data)
{
  require(arguments.size() == 1);

  const auto found = data.calcs.find(arguments[0]);

  require(found != data.calcs.end());

  pumpAll(data);

  while (!found->second.done_)
  {
    waitForEvents(data, false);
  }

  printStatus(output, found->second);
}

sogdanov::Data::~Data()
{
  for (const auto& entry : workers)
  {
    const Worker& worker = entry.second;

    if (worker.wr != -1)
    {
      ::close(worker.wr);
    }

    if (worker.result_rd != -1)
    {
      ::close(worker.result_rd);
    }

    if (worker.pid > 0)
    {
      ::kill(worker.pid, SIGTERM);

      while ((::waitpid(worker.pid, nullptr, 0) == -1) && (errno == EINTR))
      {
      }
    }
  }
}

bool sogdanov::waitForEvents(Data& data, const bool read_input)
{
  std::vector< pollfd > descriptors;

  descriptors.push_back(pollfd{read_input ? STDIN_FILENO : -1, POLLIN, 0});

  for (const auto& entry : data.workers)
  {
    const Worker& worker = entry.second;

    descriptors.push_back(pollfd{worker.result_rd, POLLIN, 0});

    if (!worker.outgoing.empty())
    {
      descriptors.push_back(pollfd{worker.wr, POLLOUT, 0});
    }
  }

  int result = -1;

  do
  {
    result = ::poll(descriptors.data(), static_cast< nfds_t >(descriptors.size()), -1);
  }
  while ((result == -1) && (errno == EINTR));

  if (result == -1)
  {
    throw std::runtime_error("Cannot wait for events");
  }

  pumpAll(data);

  const short events = descriptors.front().revents;

  return (events & (POLLIN | POLLHUP | POLLERR | POLLNVAL)) != 0;
}

void sogdanov::waitForAll(Data& data)
{
  while (true)
  {
    bool completed = true;

    for (const auto& entry : data.calcs)
    {
      if (!entry.second.done_)
      {
        completed = false;
        break;
      }
    }

    if (completed)
    {
      return;
    }

    waitForEvents(data, false);
  }
}
