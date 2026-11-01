#include <iostream>
#include <limits>
#include <signal.h>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_map>
#include "cmds.hpp"
#include "worker.hpp"

int main()
{
  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
    return 1;
  }
  using cmd_t = void (*)(std::istream &, std::ostream &, dirko::Data &);

  dirko::Data data;
  std::unordered_map< std::string, cmd_t > cmds;
  cmds["circle"] = dirko::cmdCircle;
  cmds["set"] = dirko::cmdSet;
  cmds["show"] = dirko::cmdShow;
  cmds["showset"] = dirko::cmdShowSet;
  cmds["frame"] = dirko::cmdFrame;
  cmds["frameset"] = dirko::cmdFrameSet;
  cmds["spawn"] = dirko::cmdSpawn;
  cmds["area-on"] = dirko::cmdAreaOn;
  cmds["status"] = dirko::cmdStatus;
  cmds["wait"] = dirko::cmdWait;

  std::string cmd;
  while (std::cin >> cmd) {
    try {
      auto func = cmds.at(cmd);
      func(std::cin, std::cout, data);
    } catch (const std::out_of_range &) {
      std::cout << "<UNKNOWN COMMAND>\n";
      std::cin.clear();
      std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
    } catch (const std::invalid_argument &) {
      std::cout << "<INVALID COMMAND>\n";
    }
  }

  for (auto &worker_iter : data.workers) {
    const dirko::Worker &worker = worker_iter.second;
    if (worker.input >= 0) {
      close(worker.input);
    }
    if (worker.output >= 0) {
      close(worker.output);
    }
    if (worker.pid > 0) {
      waitpid(worker.pid, nullptr, 0);
    }
  }
}
