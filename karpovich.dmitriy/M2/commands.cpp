#include "commands.hpp"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace karpovich
{

  namespace
  {
    std::ostream &operator<<(std::ostream &out, karpovich::point_t p)
    {
      return out << "(" << p.x << " " << p.y << ")";
    }

    void printShape(std::ostream &out, const karpovich::Shape &sh)
    {
      using namespace karpovich;
      if (auto c = dynamic_cast< const Circle * >(&sh))
      {
        out << c->getRadius() << " " << c->getCenter() << "\n";
      }
      else if (auto e = dynamic_cast< const Ellipse * >(&sh))
      {
        out << e->getHorizontalRadius() << " " << e->getVerticalRadius() << " " << e->getCenter() << "\n";
      }
    }

    size_t send(int &err, int wr, const void *buf, size_t k)
    {
      const char *p = static_cast< const char * >(buf);
      size_t r = 0;
      while (r < k)
      {
        err = write(wr, p + r, k - r);
        if (err <= 0)
        {
          break;
        }
        r += err;
      }
      return r;
    }

  }

  void cmdCircle(std::istream &in, std::ostream &, Data &data)
  {
    std::string name;
    double r = 0.0, x = 0.0, y = 0.0;
    if (!(in >> name >> r >> x >> y))
    {
      throw std::invalid_argument("invalid");
    }
    if (data.shapes.count(name))
    {
      throw std::invalid_argument("invalid");
    }
    if (r <= 0)
    {
      throw std::invalid_argument("circle");
    }
    data.shapes[name] = std::make_shared< Circle >(r, point_t{x, y});
  }

  void cmdSet(std::istream &in, std::ostream &, Data &data)
  {
    std::string name;
    size_t n = 0;
    if (!(in >> name >> n))
    {
      throw std::invalid_argument("invalid");
    }
    if (n == 0)
    {
      throw std::invalid_argument("invalid");
    }
    if (data.sets.count(name))
    {
      throw std::invalid_argument("invalid");
    }
    Set s;
    s.reserve(n);
    for (size_t i = 0; i < n; ++i)
    {
      std::string shape;
      if (!(in >> shape))
      {
        throw std::invalid_argument("invalid");
      }
      if (!data.shapes.count(shape))
      {
        throw std::invalid_argument("invalid");
      }
      s.push_back(std::move(shape));
    }
    data.sets[name] = std::move(s);
  }

  void cmdShow(std::istream &in, std::ostream &out, Data &data)
  {
    std::string name;
    if (!(in >> name))
    {
      throw std::invalid_argument("invalid");
    }
    auto it = data.shapes.find(name);
    if (it == data.shapes.end())
    {
      throw std::invalid_argument("invalid");
    }
    printShape(out, *it->second);
  }

  void cmdShowSet(std::istream &in, std::ostream &out, Data &data)
  {
    std::string name;
    if (!(in >> name))
    {
      throw std::invalid_argument("invalid");
    }
    auto it = data.sets.find(name);
    if (it == data.sets.end())
    {
      throw std::invalid_argument("invalid");
    }
    for (const auto &s : it->second)
    {
      printShape(out, *data.shapes.at(s));
    }
  }

  void cmdFrame(std::istream &in, std::ostream &out, Data &data)
  {
    std::string name;
    if (!(in >> name))
    {
      throw std::invalid_argument("invalid");
    }
    auto it = data.shapes.find(name);
    if (it == data.shapes.end())
    {
      throw std::invalid_argument("invalid");
    }
    const point_t mn = it->second->getMinCorner();
    const point_t mx = it->second->getMaxCorner();
    out << mn << " " << mx << "\n";
  }

  void cmdFrameSet(std::istream &in, std::ostream &out, Data &data)
  {
    std::string name;
    if (!(in >> name))
    {
      throw std::invalid_argument("invalid");
    }
    auto it = data.sets.find(name);
    if (it == data.sets.end())
    {
      throw std::invalid_argument("invalid");
    }

    point_t mn = data.shapes.at(it->second.front())->getMinCorner();
    point_t mx = data.shapes.at(it->second.front())->getMaxCorner();
    for (const auto &s : it->second)
    {
      const Shape &sh = *data.shapes.at(s);
      const point_t a = sh.getMinCorner();
      const point_t b = sh.getMaxCorner();
      mn.x = std::min(mn.x, a.x);
      mn.y = std::min(mn.y, a.y);
      mx.x = std::max(mx.x, b.x);
      mx.y = std::max(mx.y, b.y);
    }
    out << mn << " " << mx << "\n";
  }

  void cmdSpawn(std::istream &in, std::ostream &, Data &data)
  {
    std::string name;
    size_t seed = 0;
    if (!(in >> name >> seed))
    {
      throw std::invalid_argument("invalid");
    }
    if (data.workers.count(name))
    {
      throw std::invalid_argument("invalid");
    }

    const std::string shm_name = "/M2-" + std::to_string(getpid()) + "-" + name;
    const int shm_fd = shm_open(shm_name.c_str(), O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (shm_fd < 0)
    {
      throw std::invalid_argument("shm_open");
    }
    if ((ftruncate(shm_fd, 4096)) < 0)
    {
      close(shm_fd);
      shm_unlink(shm_name.c_str());
      throw std::invalid_argument("ftruncate");
    }
    void *addr = mmap(nullptr, sizeof(WorkerShared), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (addr == MAP_FAILED)
    {
      close(shm_fd);
      shm_unlink(shm_name.c_str());
      throw std::invalid_argument("mmap");
    }
    WorkerShared *sh = static_cast< WorkerShared * >(addr);

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&sh->mtx, &attr);
    pthread_mutexattr_destroy(&attr);
    sh->completed = 0;

    int pps[2] = {};
    if (pipe(pps) < 0)
    {
      pthread_mutex_destroy(&sh->mtx);
      munmap(sh, sizeof(WorkerShared));
      close(shm_fd);
      shm_unlink(shm_name.c_str());
      throw std::invalid_argument("pipe");
    }
    const int rd = pps[0];
    const int wr = pps[1];

    const pid_t pid = fork();
    if (pid < 0)
    {
      close(rd);
      close(wr);
      pthread_mutex_destroy(&sh->mtx);
      munmap(sh, sizeof(WorkerShared));
      close(shm_fd);
      shm_unlink(shm_name.c_str());
      throw std::invalid_argument("fork");
    }
    if (pid == 0)
    {
      close(wr);
      runWorker(rd, seed, sh);
      _exit(0);
    }
    close(rd);

    Worker w;
    w.pid = pid;
    w.wr = wr;
    w.shm_fd = shm_fd;
    w.shm_name = shm_name;
    w.sh = sh;
    w.seen = 0;
    data.workers[name] = w;
  }

  void cmdAreaOn(std::istream &, std::ostream &, Data &)
  {}

  void cmdStatus(std::istream &, std::ostream &, Data &)
  {}

  void cmdWait(std::istream &, std::ostream &, Data &)
  {}

}
