#include <algorithm>
#include <istream>
#include <ostream>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <memory>
#include <sys/types.h>
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdexcept>
#include <string>
#include <sys/mman.h>
#include <thread>
#include <unistd.h>
#include <utility>
#include <vector>
#include <fcntl.h>
#include "../common/shape.hpp"
#include "worker.hpp"
#include "commands.hpp"

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
      const char *const p = static_cast< const char * >(buf);
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

    void pumpAll(karpovich::Data &data)
    {
      using namespace karpovich;
      for (auto &kv : data.workers)
      {
        Worker &w = kv.second;
        pthread_mutex_lock(&w.sh->mtx);
        const int c = w.sh->completed;
        for (int i = w.seen; i < c; ++i)
        {
          const ResultSlot &s = w.sh->slots[i % WORKER_MAX_SLOTS];
          for (auto &cc : data.calcs)
          {
            Calc &calc = cc.second;
            if (calc.calc_id == s.calc_id && calc.state == CalcState::IN_PROGRESS)
            {
              calc.state = CalcState::DONE;
              calc.ms = s.ms;
              calc.area = s.area;
            }
          }
        }
        w.seen = c;
        pthread_mutex_unlock(&w.sh->mtx);
      }
    }

    void printStatus(std::ostream &out, const karpovich::Calc &c)
    {
      if (c.state == karpovich::CalcState::DONE)
      {
        out << c.proc << " " << static_cast< int >(c.ms) << " " << c.area << "\n";
      }
      else
      {
        out << c.proc << " <IN PROGRESS>\n";
      }
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

    const std::string shm_name = "/lab2-" + std::to_string(getpid()) + "-" + name;
    const int shm_fd = shm_open(shm_name.c_str(), O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (shm_fd < 0)
    {
      throw std::invalid_argument("shm_open");
    }
    if (ftruncate(shm_fd, static_cast< off_t >(sizeof(WorkerShared))) < 0)
    {
      close(shm_fd);
      shm_unlink(shm_name.c_str());
      throw std::invalid_argument("ftruncate");
    }
    void *const addr = mmap(nullptr, sizeof(WorkerShared), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (addr == MAP_FAILED)
    {
      close(shm_fd);
      shm_unlink(shm_name.c_str());
      throw std::invalid_argument("mmap");
    }
    WorkerShared *const sh = static_cast< WorkerShared * >(addr);

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

  void cmdAreaOn(std::istream &in, std::ostream &, Data &data)
  {
    std::string proc, calc, setname;
    size_t threads = 0, tries = 0;
    if (!(in >> proc >> calc >> setname >> threads >> tries))
    {
      throw std::invalid_argument("invalid");
    }
    if (threads == 0 || tries == 0)
    {
      throw std::invalid_argument("invalid");
    }

    auto wit = data.workers.find(proc);
    if (wit == data.workers.end())
    {
      throw std::invalid_argument("invalid");
    }
    auto sit = data.sets.find(setname);
    if (sit == data.sets.end())
    {
      throw std::invalid_argument("invalid");
    }
    if (data.calcs.count(calc))
    {
      throw std::invalid_argument("invalid");
    }

    std::vector< ShapeData > raw;
    raw.reserve(sit->second.size());
    for (const auto &sname : sit->second)
    {
      const Shape &sh = *data.shapes.at(sname);
      if (auto c = dynamic_cast< const Circle * >(&sh))
      {
        raw.push_back(ShapeData{ShapeType::Circle, c->getRadius(), 0.0, c->getCenter().x, c->getCenter().y});
      }
      else if (auto e = dynamic_cast< const Ellipse * >(&sh))
      {
        const double hr = e->getHorizontalRadius();
        const double vr = e->getVerticalRadius();
        raw.push_back(ShapeData{ShapeType::Ellipse, hr, vr, e->getCenter().x, e->getCenter().y});
      }
    }

    const size_t calc_id = data.next_calc_id++;
    size_t hdr[PROTO_HEADER_WORDS] = {1, calc_id, threads, tries, raw.size()};

    int err = 0;
    send(err, wit->second.wr, hdr, sizeof hdr);
    if (err <= 0)
    {
      throw std::invalid_argument("write");
    }
    if (!raw.empty())
    {
      send(err, wit->second.wr, raw.data(), raw.size() * sizeof(ShapeData));
      if (err <= 0)
      {
        throw std::invalid_argument("write");
      }
    }

    Calc c;
    c.proc = proc;
    c.calc_id = calc_id;
    c.state = CalcState::IN_PROGRESS;
    data.calcs[calc] = c;
  }

  void cmdStatus(std::istream &in, std::ostream &out, Data &data)
  {
    std::string name;
    if (!(in >> name))
    {
      throw std::invalid_argument("invalid");
    }
    auto it = data.calcs.find(name);
    if (it == data.calcs.end())
    {
      throw std::invalid_argument("invalid");
    }
    pumpAll(data);
    printStatus(out, it->second);
  }

  void cmdWait(std::istream &in, std::ostream &out, Data &data)
  {
    std::string name;
    if (!(in >> name))
    {
      throw std::invalid_argument("invalid");
    }
    auto it = data.calcs.find(name);
    if (it == data.calcs.end())
    {
      throw std::invalid_argument("invalid");
    }
    while (it->second.state != CalcState::DONE)
    {
      pumpAll(data);
      if (it->second.state == CalcState::DONE)
      {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    printStatus(out, it->second);
  }

}
