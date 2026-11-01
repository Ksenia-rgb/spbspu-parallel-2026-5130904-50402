#include <csignal>
#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include "commands.hpp"

namespace
{
  using Handler = bool (*)(std::istream &, std::ostream &, afanasev::Data &);

  const std::unordered_map< std::string, Handler > &commandTable()
  {
    static const std::unordered_map< std::string, Handler > t = {
        {"circle", afanasev::cmdCircle},
        {"set", afanasev::cmdSet},
        {"show", afanasev::cmdShow},
        {"showset", afanasev::cmdShowSet},
        {"frame", afanasev::cmdFrame},
        {"frameset", afanasev::cmdFrameSet},
        {"spawn", afanasev::cmdSpawn},
        {"area-on", afanasev::cmdAreaOn},
        {"status", afanasev::cmdStatus},
        {"wait", afanasev::cmdWait},
    };
    return t;
  }
}

int main()
{
  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
  {
    std::cerr << "failed to ignore SIGPIPE\n";
    return 1;
  }

  afanasev::Data data;
  std::string line;

  while (std::getline(std::cin, line))
  {
    std::istringstream in(line);
    std::string cmd;

    if (!(in >> cmd))
    {
      continue;
    }

    const auto &table = commandTable();
    auto it = table.find(cmd);

    if (it == table.end())
    {
      std::cout << "<UNKNOWN COMMAND>\n";
      continue;
    }

    if (!it->second(in, std::cout, data))
    {
      std::cout << "<INVALID COMMAND>\n";
    }
  }

  for (auto &thr : data.workers)
  {
    afanasev::stopWorker(thr.second);
  }

  return 0;
}
