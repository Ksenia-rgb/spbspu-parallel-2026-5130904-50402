#include "worker.hpp"

#include <algorithm>
#include <chrono>
#include <limits>
#include <numeric>
#include <random>
#include <thread>
#include <vector>
#include <cerrno>
#include <csignal>
#include <cstddef>
#include <poll.h>
#include <sys/wait.h>
#include <unistd.h>

namespace afanasev
{
  namespace
  {
    [[noreturn]] void runChild(unsigned seed, int req_rd, int result_wr)
    {
      signal(SIGPIPE, SIG_IGN);

      for (;;)
      {
        RequestHeader hdr{};
        if (!recvAll(req_rd, &hdr, sizeof hdr))
        {
          _exit(0);
        }

        std::vector< ShapeWire > shapes(hdr.num_shapes);
        if (!shapes.empty())
        {
          if (!recvAll(req_rd, shapes.data(), shapes.size() * sizeof(ShapeWire)))
          {
            _exit(0);
          }
        }

        const auto start = std::chrono::steady_clock::now();

        double area = 0.0;

        if (!shapes.empty())
        {
          double min_x = std::numeric_limits< double >::infinity();
          double max_x = -std::numeric_limits< double >::infinity();
          double min_y = std::numeric_limits< double >::infinity();
          double max_y = -std::numeric_limits< double >::infinity();
          for (const auto &s : shapes)
          {
            min_x = std::min(min_x, s.cx - s.radius);
            max_x = std::max(max_x, s.cx + s.radius);
            min_y = std::min(min_y, s.cy - s.radius);
            max_y = std::max(max_y, s.cy + s.radius);
          }

          const double bbox_area = (max_x - min_x) * (max_y - min_y);

          constexpr std::size_t MAX_THREADS = 12;

          std::size_t nthreads = hdr.threads;
          if (nthreads > MAX_THREADS)
          {
            nthreads = MAX_THREADS;
          }
          if (nthreads == 0)
          {
            nthreads = 1;
          }

          const double gen_min = static_cast< double >(std::default_random_engine::min());
          const double gen_range = static_cast< double >(std::default_random_engine::max()) - gen_min;
          const double scale_x = (max_x - min_x) / gen_range;
          const double scale_y = (max_y - min_y) / gen_range;

          const long long total_tries = static_cast< long long >(hdr.tries);
          const long long base = total_tries / static_cast< long long >(nthreads);
          const long long rem = total_tries % static_cast< long long >(nthreads);
          const unsigned base_seed = seed + static_cast< unsigned >(hdr.calc_id);

          std::vector< long long > counts(nthreads, 0);

          if (nthreads == 1)
          {
            std::default_random_engine gen(base_seed);
            long long in_union = 0;
            for (long long i = 0; i < total_tries; ++i)
            {
              const double px = min_x + (static_cast< double >(gen()) - gen_min) * scale_x;
              const double py = min_y + (static_cast< double >(gen()) - gen_min) * scale_y;
              for (const auto &s : shapes)
              {
                const double dx = px - s.cx;
                const double dy = py - s.cy;
                if (dx * dx + dy * dy <= s.radius * s.radius)
                {
                  ++in_union;
                  break;
                }
              }
            }
            counts[0] = in_union;
          }
          else
          {
            std::vector< std::thread > workers;
            workers.reserve(nthreads);

            for (std::size_t t = 0; t < nthreads; ++t)
            {
              const long long cnt = base + (static_cast< long long >(t) < rem ? 1LL : 0LL);
              const unsigned thread_seed = base_seed + static_cast< unsigned >(t);

              workers.emplace_back(
                  [&, t, cnt, thread_seed]()
                  {
                    std::default_random_engine gen(thread_seed);
                    long long in_union = 0;
                    for (long long i = 0; i < cnt; ++i)
                    {
                      const double px = min_x + (static_cast< double >(gen()) - gen_min) * scale_x;
                      const double py = min_y + (static_cast< double >(gen()) - gen_min) * scale_y;
                      for (const auto &s : shapes)
                      {
                        const double dx = px - s.cx;
                        const double dy = py - s.cy;
                        if (dx * dx + dy * dy <= s.radius * s.radius)
                        {
                          ++in_union;
                          break;
                        }
                      }
                    }
                    counts[t] = in_union;
                  });
            }

            for (std::thread &w : workers)
            {
              w.join();
            }
          }

          const long long total_in = std::accumulate(counts.begin(), counts.end(), 0LL);
          area = bbox_area * static_cast< double >(total_in) / static_cast< double >(hdr.tries);
        }

        const auto end = std::chrono::steady_clock::now();
        const double ms = std::chrono::duration< double, std::milli >(end - start).count();

        ResultSlot r{};
        r.calc_id = hdr.calc_id;
        r.ms = ms;
        r.area = area;

        if (!sendAll(result_wr, &r, sizeof r))
        {
          _exit(0);
        }
      }
    }
  }

  bool sendAll(int fd, const void *buf, std::size_t n)
  {
    const char *const p = static_cast< const char * >(buf);
    std::size_t put = 0;
    while (put < n)
    {
      const ssize_t r = write(fd, p + put, n - put);
      if (r < 0)
      {
        if (errno == EINTR)
        {
          continue;
        }
        return false;
      }
      if (r == 0)
      {
        return false;
      }
      put += static_cast< std::size_t >(r);
    }
    return true;
  }

  bool recvAll(int fd, void *buf, std::size_t n)
  {
    char *const p = static_cast< char * >(buf);
    std::size_t got = 0;
    while (got < n)
    {
      const ssize_t r = read(fd, p + got, n - got);
      if (r < 0)
      {
        if (errno == EINTR)
        {
          continue;
        }
        return false;
      }
      if (r == 0)
      {
        return false;
      }
      got += static_cast< std::size_t >(r);
    }
    return true;
  }

  bool hasData(int fd)
  {
    pollfd pfd{};
    pfd.fd = fd;
    pfd.events = POLLIN;
    return poll(&pfd, 1, 0) > 0;
  }

  Worker spawnWorker(unsigned seed)
  {
    Worker w;

    int req_pipe[2] = {-1, -1};
    int result_pipe[2] = {-1, -1};

    if (pipe(req_pipe) < 0)
    {
      return w;
    }
    if (pipe(result_pipe) < 0)
    {
      close(req_pipe[0]);
      close(req_pipe[1]);
      return w;
    }

    const pid_t pid = fork();
    if (pid < 0)
    {
      close(req_pipe[0]);
      close(req_pipe[1]);
      close(result_pipe[0]);
      close(result_pipe[1]);
      return w;
    }

    if (pid == 0)
    {
      close(req_pipe[1]);
      close(result_pipe[0]);
      runChild(seed, req_pipe[0], result_pipe[1]);
    }

    close(req_pipe[0]);
    close(result_pipe[1]);

    w.pid = pid;
    w.req_wr = req_pipe[1];
    w.result_rd = result_pipe[0];
    return w;
  }

  void stopWorker(Worker &w)
  {
    if (w.req_wr >= 0)
    {
      close(w.req_wr);
      w.req_wr = -1;
    }
    if (w.result_rd >= 0)
    {
      close(w.result_rd);
      w.result_rd = -1;
    }
    if (w.pid > 0)
    {
      kill(w.pid, SIGTERM);
      waitpid(w.pid, nullptr, 0);
      w.pid = -1;
    }
  }
}
