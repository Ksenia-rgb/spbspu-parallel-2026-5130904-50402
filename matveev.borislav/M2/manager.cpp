#include "manager.hpp"
#include <stdexcept>
#include <utility>
#include <cerrno>
#include <ostream>
#include <iomanip>
#include <poll.h>
#include "parse.hpp"

void matveev::Manager::spawn(const std::string &name, const std::uint64_t seed)
{
  requireName(name);
  if (workers_.count(name) != 0)
  {
    throw std::invalid_argument("Process already exists");
  }
  std::vector< int > inherited = {};
  for (const auto &entry: workers_)
  {
    inherited.push_back(entry.second.process->getDescriptor());
  }
  std::unique_ptr< Process > process = std::make_unique< Process >(seed, inherited);
  workers_[name].process = std::move(process);
}

void matveev::Manager::submit(const std::string &process, const std::string &name,
    const std::vector< shape_t > &shapes, const std::uint64_t threads, const std::uint64_t tries)
{
  requireName(name);
  worker_t &worker = workers_.at(process);
  if (worker.stopping || (calculations_.count(name) != 0))
  {
    throw std::invalid_argument("Cannot submit calculation");
  }
  task_t task = {name, makeRequest(shapes, threads, tries)};
  const calculation_t calculation = {process, State::pending, {}};
  calculations_.emplace(name, calculation);
  try
  {
    worker.queue.push_back(std::move(task));
  }
  catch (const std::exception &)
  {
    calculations_.erase(name);
    throw;
  }
  startNext(worker);
}

void matveev::Manager::startNext(worker_t &worker)
{
  if (!worker.current.empty())
  {
    return;
  }
  if (worker.stopping)
  {
    worker.process->stop(false);
  }
  else if (!worker.queue.empty())
  {
    worker.current.swap(worker.queue.front().name);
    worker.process->start(worker.queue.front().bytes);
    worker.queue.pop_front();
  }
}

void matveev::Manager::cancelQueued(worker_t &worker)
{
  for (const task_t &task: worker.queue)
  {
    calculations_.at(task.name).state = State::canceled;
  }
  worker.queue.clear();
}

void matveev::Manager::disconnect(worker_t &worker)
{
  worker.stopping = true;
  if (!worker.current.empty())
  {
    calculations_.at(worker.current).state = State::canceled;
    worker.current.clear();
  }
  cancelQueued(worker);
  worker.process->stop(true);
}

bool matveev::Manager::pump(const int timeout, const int input)
{
  std::vector< pollfd > descriptors = {};
  std::vector< worker_t * > owners = {};
  if (input >= 0)
  {
    descriptors.push_back({input, POLLIN, 0});
    owners.push_back(nullptr);
  }
  for (auto &entry: workers_)
  {
    const Process &process = *entry.second.process;
    if (process.getDescriptor() >= 0)
    {
      const short events = static_cast< short >(POLLIN | (process.hasOutput() ? POLLOUT : 0));
      descriptors.push_back({process.getDescriptor(), events, 0});
      owners.push_back(&entry.second);
    }
  }
  const int ready = ::poll(descriptors.data(), descriptors.size(), timeout);
  if (ready < 0)
  {
    if (errno == EINTR)
    {
      return false;
    }
    throw std::runtime_error("poll failed");
  }
  bool inputReady = false;
  for (std::size_t i = 0; i < descriptors.size(); ++i)
  {
    const short events = descriptors[i].revents;
    if (events == 0)
    {
      continue;
    }
    if (owners[i] == nullptr)
    {
      inputReady = true;
      continue;
    }
    worker_t &worker = *owners[i];
    try
    {
      if ((events & POLLOUT) != 0)
      {
        worker.process->sendPending();
      }
      if ((events & (POLLIN | POLLHUP | POLLERR | POLLNVAL)) != 0)
      {
        response_t response = {};
        if (worker.process->receive(response))
        {
          calculation_t &calculation = calculations_.at(worker.current);
          calculation.result = response;
          calculation.state = (response.success != 0) ? State::completed : State::canceled;
          worker.current.clear();
          startNext(worker);
        }
      }
    }
    catch (const std::exception &)
    {
      disconnect(worker);
    }
  }
  return inputReady;
}

void matveev::Manager::printStatus(const std::string &name, std::ostream &out) const
{
  const calculation_t &calculation = calculations_.at(name);
  out << calculation.process << " ";
  if (calculation.state == State::pending)
  {
    out << "<IN PROGRESS>\n";
  }
  else if (calculation.state == State::canceled)
  {
    out << "<CANCELED>\n";
  }
  else
  {
    const std::ios_base::fmtflags flags = out.flags();
    const std::streamsize precision = out.precision();
    constexpr int resultPrecision = 3;
    out << std::fixed << std::setprecision(resultPrecision) << calculation.result.milliseconds;
    out << " " << calculation.result.area << "\n";
    out.flags(flags);
    out.precision(precision);
  }
}

void matveev::Manager::wait(const std::string &name)
{
  while (calculations_.at(name).state == State::pending)
  {
    pump(waitIndefinitely);
  }
}
