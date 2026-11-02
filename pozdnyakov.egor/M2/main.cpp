#include <csignal>
#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <signal.h>
#include "interpreter.hpp"
#include "parse.hpp"

int main()
{
  const int internal_error_code = 2;
  try
  {
    std::signal(SIGPIPE, SIG_IGN);
    std::cout << std::setprecision(std::numeric_limits< double >::digits10);
    pozdnyakov::Interpreter interpreter;
    std::string line;
    while (std::getline(std::cin, line))
    {
      try
      {
        if (!interpreter.execute(pozdnyakov::splitWords(line), std::cout))
        {
          std::cout << "<UNKNOWN COMMAND>\n";
        }
      }
      catch (const std::logic_error&)
      {
        std::cout << "<INVALID COMMAND>\n";
      }
      catch (const std::runtime_error& error)
      {
        std::cerr << error.what() << '\n';
        std::cout << "<INVALID COMMAND>\n";
      }
    }
  }
  catch (const std::exception& error)
  {
    std::cerr << error.what() << '\n';
    return internal_error_code;
  }
  return 0;
}
