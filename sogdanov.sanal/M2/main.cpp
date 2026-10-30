#include <cerrno>
#include <exception>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

#include <signal.h>
#include <unistd.h>

#include "commands.hpp"

int main(int argc, char*[])
{
  if (argc != 1)
  {
    std::cerr << "This program takes no command-line arguments\n";
    return 0;
  }

  try
  {
    struct sigaction action = {};

    action.sa_handler = SIG_IGN;

    if ((::sigemptyset(&action.sa_mask) == -1) || (::sigaction(SIGPIPE, &action, nullptr) == -1))
    {
      throw std::runtime_error("Cannot configure SIGPIPE handling");
    }

    std::cout.exceptions(std::ios::badbit | std::ios::failbit);

    sogdanov::Data data{};

    const std::map< std::string, sogdanov::command_t > commands = {{"circle", sogdanov::cmdCircle},
        {"set", sogdanov::cmdSet}, {"show", sogdanov::cmdShow}, {"showset", sogdanov::cmdShowSet},
        {"frame", sogdanov::cmdFrame}, {"frameset", sogdanov::cmdFrameSet}, {"spawn", sogdanov::cmdSpawn},
        {"area-on", sogdanov::cmdAreaOn}, {"status", sogdanov::cmdStatus}, {"wait", sogdanov::cmdWait}};

    const char invalid_command[] = "<INVALID COMMAND>\n";
    const size_t buffer_size = 4096;
    char buffer[buffer_size] = {};
    std::string pending;
    bool input_finished = false;

    while (true)
    {
      const size_t newline = pending.find('\n');

      if ((newline == std::string::npos) && !input_finished)
      {
        if (!sogdanov::waitForEvents(data, true))
        {
          continue;
        }

        const ssize_t count = ::read(STDIN_FILENO, buffer, sizeof(buffer));

        if (count < 0)
        {
          if (errno == EINTR)
          {
            continue;
          }

          throw std::runtime_error("Cannot read standard input");
        }

        if (count == 0)
        {
          input_finished = true;
        }
        else
        {
          pending.append(buffer, static_cast< size_t >(count));
        }

        continue;
      }

      if (input_finished && pending.empty())
      {
        break;
      }

      const size_t length = (newline == std::string::npos) ? pending.size() : newline;
      const std::string line = pending.substr(0, length);
      const size_t consumed = (newline == std::string::npos) ? length : length + 1;

      pending.erase(0, consumed);

      sogdanov::Arguments arguments = sogdanov::tokenize(line);

      if (arguments.empty())
      {
        continue;
      }

      const auto command = commands.find(arguments.front());

      if (command == commands.end())
      {
        std::cout << "<UNKNOWN COMMAND>\n";
        std::cout.flush();
        continue;
      }

      arguments.erase(arguments.begin());

      try
      {
        command->second(arguments, std::cout, data);
      }
      catch (const std::invalid_argument&)
      {
        std::cout << invalid_command;
      }
      catch (const std::out_of_range&)
      {
        std::cout << invalid_command;
      }

      std::cout.flush();
    }

    sogdanov::waitForAll(data);
  }
  catch (const std::exception& error)
  {
    std::cerr << error.what() << '\n';
  }
}
