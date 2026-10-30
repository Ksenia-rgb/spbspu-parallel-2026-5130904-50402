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

      output << calculation.process_name_ << ' ';

      if (calculation.done_)
      {
        output << std::fixed << std::setprecision(3) << calculation.milliseconds_ << ' ' << calculation.area_ << '\n';
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
        const size_t remaining = sizeof(worker.reply_) - worker.received_;
        const ssize_t count = ::read(worker.result_rd_, worker.reply_ + worker.received_, remaining);

        if (count > 0)
        {
          worker.received_ += static_cast< size_t >(count);

          if (worker.received_ != sizeof(worker.reply_))
          {
            continue;
          }

          ResultSlot result = {};

          std::memcpy(&result.calculation_id_, worker.reply_, sizeof(result.calculation_id_));
          std::memcpy(
              &result.milliseconds_, worker.reply_ + sizeof(result.calculation_id_), sizeof(result.milliseconds_));
          std::memcpy(&result.area_, worker.reply_ + sizeof(result.calculation_id_) + sizeof(result.milliseconds_),
              sizeof(result.area_));

          const auto found = std::find_if(data.calcs_.begin(), data.calcs_.end(),
              [&result](const auto& entry)
              {
                return entry.second.id_ == result.calculation_id_;
              });

          if ((found == data.calcs_.end()) || found->second.done_)
          {
            throw std::runtime_error("Unexpected calculation result");
          }

          Calc& calculation = found->second;

          calculation.milliseconds_ = result.milliseconds_;
          calculation.area_ = result.area_;
          calculation.done_ = true;
          worker.received_ = 0;
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
      while (!worker.outgoing_.empty())
      {
        const std::vector< unsigned char >& message = worker.outgoing_.front();
        const size_t remaining = message.size() - worker.sent_;
        const ssize_t count = ::write(worker.wr_, message.data() + worker.sent_, remaining);

        if (count > 0)
        {
          worker.sent_ += static_cast< size_t >(count);

          if (worker.sent_ == message.size())
          {
            worker.outgoing_.pop_front();
            worker.sent_ = 0;
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
      for (auto& entry : data.workers_)
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
  require(data.figures_.find(name) == data.figures_.end());

  data.figures_.emplace(name, Figure(radius, radius, x, y));
}

void sogdanov::cmdSet(const Arguments& arguments, std::ostream&, Data& data)
{
  require(arguments.size() >= 2);

  const std::string& name = arguments[0];
  const size_t count = parseSize(arguments[1]);

  require(validName(name));
  require(count > 0);
  require(count == arguments.size() - 2);
  require(data.sets_.find(name) == data.sets_.end());

  std::vector< Figure > figures;

  figures.reserve(count);

  for (size_t index = 0; index < count; ++index)
  {
    const auto found = data.figures_.find(arguments[index + 2]);

    require(found != data.figures_.end());
    figures.push_back(found->second);
  }

  data.sets_.emplace(name, std::move(figures));
}

void sogdanov::cmdShow(const Arguments& arguments, std::ostream& output, Data& data)
{
  require(arguments.size() == 1);

  const auto found = data.figures_.find(arguments[0]);

  require(found != data.figures_.end());
  printFigure(output, found->second);
}

void sogdanov::cmdShowSet(const Arguments& arguments, std::ostream& output, Data& data)
{
  require(arguments.size() == 1);

  const auto found = data.sets_.find(arguments[0]);

  require(found != data.sets_.end());

  for (const Figure& figure : found->second)
  {
    printFigure(output, figure);
  }
}

void sogdanov::cmdFrame(const Arguments& arguments, std::ostream& output, Data& data)
{
  require(arguments.size() == 1);

  const auto found = data.figures_.find(arguments[0]);

  require(found != data.figures_.end());
  printBounds(output, found->second.getBounds());
}

void sogdanov::cmdFrameSet(const Arguments& arguments, std::ostream& output, Data& data)
{
  require(arguments.size() == 1);

  const auto found = data.sets_.find(arguments[0]);

  require(found != data.sets_.end());
  printBounds(output, findGeneralBounds(found->second));
}

void sogdanov::cmdSpawn(const Arguments& arguments, std::ostream&, Data& data)
{
  require(arguments.size() == 2);

  const std::string& name = arguments[0];
  const size_t seed = parseSize(arguments[1]);

  require(validName(name));
  require(data.workers_.find(name) == data.workers_.end());

  const auto inserted = data.workers_.emplace(name, Worker{});
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

      for (const auto& entry : data.workers_)
      {
        if (entry.second.wr_ != -1)
        {
          ::close(entry.second.wr_);
        }

        if (entry.second.result_rd_ != -1)
        {
          ::close(entry.second.result_rd_);
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

    worker.pid_ = pid;
    worker.wr_ = request_pipe[1];
    worker.result_rd_ = result_pipe[0];

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

    data.workers_.erase(inserted.first);
    throw;
  }
}

void sogdanov::cmdAreaOn(const Arguments& arguments, std::ostream&, Data& data)
{
  require(arguments.size() == 5);

  const std::string& process_name = arguments[0];
  const std::string& calculation_name = arguments[1];
  const std::string& set_name = arguments[2];
  const size_t threads = parseSize(arguments[3]);
  const size_t tries = parseSize(arguments[4]);

  require(validName(calculation_name));
  require(tries > 0);
  require(data.calcs_.find(calculation_name) == data.calcs_.end());

  const auto process = data.workers_.find(process_name);
  const auto set = data.sets_.find(set_name);

  require(process != data.workers_.end());
  require(set != data.sets_.end());

  if (data.next_calculation_id_ == std::numeric_limits< size_t >::max())
  {
    throw std::overflow_error("Too many calculations");
  }

  const size_t id = data.next_calculation_id_;
  std::vector< unsigned char > request = makeRequest(id, set->second, threads, tries);
  const Calc calculation = {id, process_name, false, 0.0, 0.0};
  const auto inserted = data.calcs_.emplace(calculation_name, calculation);

  try
  {
    process->second.outgoing_.push_back(std::move(request));
  }
  catch (...)
  {
    data.calcs_.erase(inserted.first);
    throw;
  }

  ++data.next_calculation_id_;
}

void sogdanov::cmdStatus(const Arguments& arguments, std::ostream& output, Data& data)
{
  require(arguments.size() == 1);

  const auto found = data.calcs_.find(arguments[0]);

  require(found != data.calcs_.end());

  pumpAll(data);
  printStatus(output, found->second);
}

void sogdanov::cmdWait(const Arguments& arguments, std::ostream& output, Data& data)
{
  require(arguments.size() == 1);

  const auto found = data.calcs_.find(arguments[0]);

  require(found != data.calcs_.end());

  pumpAll(data);

  while (!found->second.done_)
  {
    waitForEvents(data, false);
  }

  printStatus(output, found->second);
}

sogdanov::Data::~Data()
{
  for (auto& entry : workers_)
  {
    Worker& worker = entry.second;

    if (worker.wr_ != -1)
    {
      ::close(worker.wr_);
    }

    if (worker.result_rd_ != -1)
    {
      ::close(worker.result_rd_);
    }

    if (worker.pid_ > 0)
    {
      ::kill(worker.pid_, SIGTERM);

      while ((::waitpid(worker.pid_, nullptr, 0) == -1) && (errno == EINTR))
      {
      }
    }
  }
}

bool sogdanov::waitForEvents(Data& data, const bool read_input)
{
  std::vector< pollfd > descriptors;

  descriptors.push_back(pollfd{read_input ? STDIN_FILENO : -1, POLLIN, 0});

  for (const auto& entry : data.workers_)
  {
    const Worker& worker = entry.second;

    descriptors.push_back(pollfd{worker.result_rd_, POLLIN, 0});

    if (!worker.outgoing_.empty())
    {
      descriptors.push_back(pollfd{worker.wr_, POLLOUT, 0});
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

    for (const auto& entry : data.calcs_)
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
