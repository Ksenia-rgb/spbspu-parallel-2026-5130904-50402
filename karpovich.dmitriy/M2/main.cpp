#include <iostream>
#include <limits>
#include <pthread.h>
#include <signal.h>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <unordered_map>
#include "commands.hpp"
#include "worker.hpp"

namespace
{
  using cmd_t = void (*)(std::istream &, std::ostream &, karpovich::Data &);
}

int main()
{
  struct sigaction act = {};
  act.sa_handler = SIG_IGN;
  sigaction(SIGPIPE, &act, nullptr);

  karpovich::Data data;
  std::unordered_map< std::string, cmd_t > cmds;
  cmds["circle"] = karpovich::cmdCircle;
  cmds["set"] = karpovich::cmdSet;
  cmds["show"] = karpovich::cmdShow;
  cmds["showset"] = karpovich::cmdShowSet;
  cmds["frame"] = karpovich::cmdFrame;
  cmds["frameset"] = karpovich::cmdFrameSet;
  cmds["spawn"] = karpovich::cmdSpawn;
  cmds["area-on"] = karpovich::cmdAreaOn;
  cmds["status"] = karpovich::cmdStatus;
  cmds["wait"] = karpovich::cmdWait;

  std::string cmd;
  while (std::cin >> cmd)
  {
    try
    {
      cmds.at(cmd)(std::cin, std::cout, data);
    }
    catch (const std::out_of_range &)
    {
      std::cout << "<UNKNOWN COMMAND>\n";
      std::cin.clear();
      std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
    }
    catch (const std::invalid_argument &)
    {
      std::cout << "<INVALID COMMAND>\n";
      std::cin.clear();
      std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
    }
  }

  for (auto &kv : data.workers)
  {
    const karpovich::Worker &w = kv.second;
    if (w.wr >= 0)
    {
      close(w.wr);
    }
    if (w.pid > 0)
    {
      waitpid(w.pid, nullptr, 0);
    }
    if (w.sh)
    {
      pthread_mutex_destroy(&w.sh->mtx);
      munmap(w.sh, sizeof(karpovich::WorkerShared));
    }
    if (w.shm_fd >= 0)
    {
      close(w.shm_fd);
    }
    if (!w.shm_name.empty())
    {
      shm_unlink(w.shm_name.c_str());
    }
  }
}
