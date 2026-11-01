#include "commands.hpp"

#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <thread>

namespace afanasev
{
  namespace
  {
    bool isValidName(const std::string &s)
    {
      if (s.empty())
      {
        return false;
      }
      const char c = s.front();
      return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    bool hasExtra(std::istream &in)
    {
      std::string extra;
      return static_cast< bool >(in >> extra);
    }

    void printShape(std::ostream &out, const Shape &sh)
    {
      out << sh.radius() << " " << sh.center() << "\n";
    }

    bool pumpAll(Data &data)
    {
      bool all_ok = true;
      for (auto &thr : data.workers)
      {
        const Worker &w = thr.second;
        while (hasData(w.result_rd))
        {
          ResultSlot r{};
          if (!recvAll(w.result_rd, &r, sizeof r))
          {
            all_ok = false;
            break;
          }
          auto it = data.calc_name_by_id.find(r.calc_id);
          if (it == data.calc_name_by_id.end())
          {
            continue;
          }
          Calc &c = data.calcs[it->second];
          c.done = true;
          c.ms = r.ms;
          c.area = r.area;
        }
      }
      return all_ok;
    }
  }

  bool cmdCircle(std::istream &in, std::ostream &, Data &data)
  {
    std::string name;
    long long r = 0, x = 0, y = 0;
    if (!(in >> name >> r >> x >> y) || r <= 0 || !isValidName(name) || data.shapes.count(name) > 0)
    {
      return false;
    }
    if (hasExtra(in))
    {
      return false;
    }

    data.shapes.emplace(name, Shape::makeCircle(r, x, y));
    return true;
  }

  bool cmdSet(std::istream &in, std::ostream &, Data &data)
  {
    std::string name;
    long long n = 0;

    if (!(in >> name >> n) || (n < 0) || (!isValidName(name)) || (data.sets.count(name) > 0))
    {
      return false;
    }

    std::vector< std::string > names;
    names.reserve(static_cast< std::size_t >(n));
    for (long long i = 0; i < n; ++i)
    {
      std::string s;
      if (!(in >> s) || (data.shapes.count(s) == 0))
      {
        return false;
      }
      names.push_back(s);
    }

    if (hasExtra(in))
    {
      return false;
    }

    data.sets.emplace(name, std::move(names));
    return true;
  }

  bool cmdShow(std::istream &in, std::ostream &out, Data &data)
  {
    std::string name;
    if (!(in >> name))
    {
      return false;
    }
    if (hasExtra(in))
    {
      return false;
    }

    auto it = data.shapes.find(name);
    if (it == data.shapes.end())
    {
      return false;
    }

    printShape(out, it->second);
    return true;
  }

  bool cmdShowSet(std::istream &in, std::ostream &out, Data &data)
  {
    std::string name;
    if (!(in >> name))
    {
      return false;
    }
    if (hasExtra(in))
    {
      return false;
    }

    auto it = data.sets.find(name);
    if (it == data.sets.end())
    {
      return false;
    }

    for (const auto &shape_name : it->second)
    {
      auto sit = data.shapes.find(shape_name);
      if (sit == data.shapes.end())
      {
        return false;
      }
      printShape(out, sit->second);
    }
    return true;
  }

  bool cmdFrame(std::istream &in, std::ostream &out, Data &data)
  {
    std::string name;
    if (!(in >> name))
    {
      return false;
    }
    if (hasExtra(in))
    {
      return false;
    }

    auto it = data.shapes.find(name);
    if (it == data.shapes.end())
    {
      return false;
    }

    const bbox_t b = it->second.bbox();
    out << "(" << b.min_x << " " << b.min_y << ") (" << b.max_x << " " << b.max_y << ")\n";
    return true;
  }

  bool cmdFrameSet(std::istream &in, std::ostream &out, Data &data)
  {
    std::string name;
    if (!(in >> name))
    {
      return false;
    }
    if (hasExtra(in))
    {
      return false;
    }

    auto it = data.sets.find(name);
    if (it == data.sets.end())
    {
      return false;
    }

    const std::vector< std::string > &names = it->second;
    if (names.empty())
    {
      return false;
    }

    bbox_t result{};
    bool first = true;
    for (const auto &shape_name : names)
    {
      auto sit = data.shapes.find(shape_name);
      if (sit == data.shapes.end())
      {
        return false;
      }
      const bbox_t b = sit->second.bbox();
      if (first)
      {
        result = b;
        first = false;
      }
      else
      {
        result.min_x = std::min(result.min_x, b.min_x);
        result.min_y = std::min(result.min_y, b.min_y);
        result.max_x = std::max(result.max_x, b.max_x);
        result.max_y = std::max(result.max_y, b.max_y);
      }
    }

    out << "(" << result.min_x << " " << result.min_y << ") (" << result.max_x << " " << result.max_y << ")\n";
    return true;
  }

  bool cmdSpawn(std::istream &in, std::ostream &, Data &data)
  {
    std::string name;
    long long seed = 0;
    if (!(in >> name >> seed) || (seed < 0) || (!isValidName(name)) || (data.workers.count(name) > 0))
    {
      return false;
    }
    if (hasExtra(in))
    {
      return false;
    }

    Worker w = spawnWorker(static_cast< unsigned >(seed));
    if (w.pid <= 0)
    {
      return false;
    }

    data.workers.emplace(name, w);
    return true;
  }

  bool cmdAreaOn(std::istream &in, std::ostream &, Data &data)
  {
    std::string proc, calc, set;
    long long threads = 0, tries = 0;
    if (!(in >> proc >> calc >> set >> threads >> tries) || threads <= 0 || tries <= 0 || !isValidName(calc))
    {
      return false;
    }
    if (hasExtra(in))
    {
      return false;
    }
    if (threads > tries)
    {
      threads = tries;
    }

    auto wit = data.workers.find(proc);
    if (wit == data.workers.end() || (data.calcs.count(calc) > 0))
    {
      return false;
    }
    auto sit = data.sets.find(set);
    if (sit == data.sets.end())
    {
      return false;
    }

    std::vector< ShapeWire > wires;
    wires.reserve(sit->second.size());
    for (const auto &shape_name : sit->second)
    {
      auto sh_it = data.shapes.find(shape_name);
      if (sh_it == data.shapes.end())
      {
        return false;
      }
      const Shape &sh = sh_it->second;
      ShapeWire sw{};
      sw.radius = static_cast< double >(sh.radius());
      const point_t c = sh.center();
      sw.cx = static_cast< double >(c.x);
      sw.cy = static_cast< double >(c.y);
      wires.push_back(sw);
    }

    const std::size_t calc_id = data.next_calc_id++;

    RequestHeader hdr{};
    hdr.calc_id = calc_id;
    hdr.threads = static_cast< std::size_t >(threads);
    hdr.tries = static_cast< std::size_t >(tries);
    hdr.num_shapes = wires.size();

    Worker &w = wit->second;

    if (!sendAll(w.req_wr, &hdr, sizeof hdr))
    {
      stopWorker(w);
      return false;
    }
    if (!wires.empty() && !sendAll(w.req_wr, wires.data(), wires.size() * sizeof(ShapeWire)))
    {
      stopWorker(w);
      return false;
    }

    Calc c{};
    c.proc = proc;
    c.calc_id = calc_id;
    c.done = false;
    data.calcs.emplace(calc, c);
    data.calc_name_by_id.emplace(calc_id, calc);

    return true;
  }

  bool cmdStatus(std::istream &in, std::ostream &out, Data &data)
  {
    std::string name;
    if (!(in >> name) || (hasExtra(in)))
    {
      return false;
    }

    auto it = data.calcs.find(name);
    if (it == data.calcs.end())
    {
      return false;
    }

    (void)pumpAll(data);
    const Calc &c = it->second;

    if (!c.done)
    {
      out << c.proc << " <IN PROGRESS>\n";
      return true;
    }

    const auto saved_flags = out.flags();
    const auto saved_prec = out.precision();

    out << c.proc << " " << std::fixed << std::setprecision(3) << c.ms << " " << c.area << "\n";

    out.flags(saved_flags);
    out.precision(saved_prec);

    return true;
  }

  bool cmdWait(std::istream &in, std::ostream &out, Data &data)
  {
    std::string name;
    if (!(in >> name) || (hasExtra(in)))
    {
      return false;
    }

    auto it = data.calcs.find(name);
    if (it == data.calcs.end())
    {
      return false;
    }

    (void)pumpAll(data);
    const Calc &c = it->second;
    while (!c.done)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      if (!pumpAll(data))
      {
        break;
      }
    }

    if (!c.done)
    {
      out << c.proc << " <IN PROGRESS>\n";
      return true;
    }

    const auto saved_flags = out.flags();
    const auto saved_prec = out.precision();

    out << c.proc << " " << std::fixed << std::setprecision(3) << c.ms << " " << c.area << "\n";

    out.flags(saved_flags);
    out.precision(saved_prec);

    return true;
  }
}
