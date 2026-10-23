#include <algorithm>
#include <istream>
#include <ostream>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <fcntl.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <utility>
#include <vector>
#include "../common/shape.hpp"
#include "worker.hpp"
#include "commands.hpp"

namespace karpovich
{

  namespace
  {
    std::ostream &operator<<(std::ostream &out, point_t p)
    {
      return out << "(" << p.x << " " << p.y << ")";
    }

    void printShape(std::ostream &out, const Shape &sh)
    {
      if (auto c = dynamic_cast< const Circle * >(&sh))
      {
        out << c->getRadius() << " " << c->getCenter() << "\n";
      }
      else if (auto e = dynamic_cast< const Ellipse * >(&sh))
      {
        out << e->getHorizontalRadius() << " " << e->getVerticalRadius() << " " << e->getCenter() << "\n";
      }
    }

    void pumpAll(karpovich::Data &data)
    {
      using namespace karpovich;
      for (auto &kv : data.workers)
      {
        Worker &w = kv.second;
        while (w.result_rd >= 0)
        {
          ResultSlot s{};
          const size_t n = read(w.result_rd, &s, sizeof s);
          if (n == (sizeof s))
          {
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
            continue;
          }
          if (n == 0)
          {
            close(w.result_rd);
            w.result_rd = -1;
          }
          break;
        }
      }
    }

    void printStatus(std::ostream &out, const Calc &c)
    {
      if (c.state == CalcState::DONE)
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
      if (!(in >> shape) || !data.shapes.count(shape))
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

    int req[2] = {-1, -1};
    int res[2] = {-1, -1};
    if (pipe(req) < 0)
    {
      throw std::invalid_argument("pipe");
    }
    if (pipe(res) < 0)
    {
      close(req[0]);
      close(req[1]);
      throw std::invalid_argument("pipe");
    }

    const int flags = fcntl(res[0], F_GETFL, 0);
    if (flags < 0 || fcntl(res[0], F_SETFL, flags | O_NONBLOCK) < 0)
    {
      close(req[0]);
      close(req[1]);
      close(res[0]);
      close(res[1]);
      throw std::invalid_argument("fcntl");
    }

    const pid_t pid = fork();
    if (pid < 0)
    {
      close(req[0]);
      close(req[1]);
      close(res[0]);
      close(res[1]);
      throw std::invalid_argument("fork");
    }
    if (pid == 0)
    {
      close(req[1]);
      close(res[0]);
      for (const auto &kv : data.workers)
      {
        if (kv.second.wr >= 0)
        {
          close(kv.second.wr);
        }
        if (kv.second.result_rd >= 0)
        {
          close(kv.second.result_rd);
        }
      }
      runWorker(req[0], res[1], seed);
      _exit(0);
    }

    close(req[0]);
    close(res[1]);
    Worker w;
    w.pid = pid;
    w.wr = req[1];
    w.result_rd = res[0];
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
