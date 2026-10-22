#include "commands.hpp"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <iostream>
#include <unistd.h>

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
    int seed = 0;
    if (!(in >> name >> seed))
    {
      throw std::invalid_argument("invalid");
    }
    if (seed < 0)
    {
      throw std::invalid_argument("invalid");
    }
    if (data.workers.count(name))
    {
      throw std::invalid_argument("invalid");
    }

    int pps[2] = {};
    if (pipe(pps) < 0)
    {
      throw std::invalid_argument("pipe");
    }
    const int rd = pps[0];
    const int wr = pps[1];

    const pid_t pid = fork();
    if (pid < 0)
    {
      close(rd);
      close(wr);
      throw std::invalid_argument("fork");
    }
    if (pid == 0)
    {
      close(wr);
      runWorker(rd, seed);
    }
    close(rd);
    data.workers[name] = Worker{pid, wr};
  }

  void cmdAreaOn(std::istream &, std::ostream &, Data &)
  {}

  void cmdStatus(std::istream &, std::ostream &, Data &)
  {}

  void cmdWait(std::istream &, std::ostream &, Data &)
  {}

}
