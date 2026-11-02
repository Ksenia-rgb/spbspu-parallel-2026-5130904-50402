#include <iostream>
#include <exception>
#include <string>
#include <cerrno>
#include <stdexcept>
#include <csignal>
#include <unistd.h>
#include "commands.hpp"
#include "catalog.hpp"
#include "manager.hpp"

namespace matveev
{
  static void runConsole();
}

void matveev::runConsole()
{
  Catalog catalog = {};
  Manager manager = {};
  std::string pending = {};
  constexpr std::size_t bufferSize = 4096;
  char buffer[bufferSize] = {};
  while (true)
  {
    if (!manager.pump(waitIndefinitely, STDIN_FILENO))
    {
      continue;
    }
    const ssize_t received = ::read(STDIN_FILENO, buffer, bufferSize);
    if ((received < 0) && (errno == EINTR))
    {
      continue;
    }
    if (received < 0)
    {
      throw std::runtime_error("Cannot read input");
    }
    if (received == 0)
    {
      if (!pending.empty())
      {
        executeLine(pending, catalog, manager, std::cout);
      }
      return;
    }
    for (ssize_t i = 0; i < received; ++i)
    {
      if (buffer[i] == '\n')
      {
        executeLine(pending, catalog, manager, std::cout);
        pending.clear();
        manager.pump(0);
      }
      else
      {
        pending += buffer[i];
      }
    }
  }
}

int main(const int argc, char *[])
{
  constexpr int expectedArguments = 1;
  constexpr int argumentError = 1;
  constexpr int internalError = 2;
  if (argc != expectedArguments)
  {
    std::cerr << "Usage: lab\n";
    return argumentError;
  }
  try
  {
    if (std::signal(SIGPIPE, SIG_IGN) == SIG_ERR)
    {
      throw std::runtime_error("Cannot configure output signal");
    }
    matveev::runConsole();
  }
  catch (const std::exception &error)
  {
    std::cerr << error.what() << "\n";
    return internalError;
  }
  return 0;
}
