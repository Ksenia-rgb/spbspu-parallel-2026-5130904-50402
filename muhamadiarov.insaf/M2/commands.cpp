#include "commands.hpp"
#include "processManager.hpp"
#include <istream>
#include <ostream>

namespace muhamadiarov
{

  namespace
  {

    ProcessManager pm;

    std::string readWord(std::istream& in)
    {
      std::string s;
      if (!(in >> s))
      {
        throw std::invalid_argument("word");
      }

      return s;
    }

    int readInt(std::istream& in)
    {
      int v = 0;
      if (!(in >> v))
      {
        throw std::invalid_argument("number");
      }

      return v;
    }

    std::size_t readSize(std::istream& in)
    {
      std::size_t v = 0;
      if (!(in >> v))
      {
        throw std::invalid_argument("size");
      }
      return v;
    }

  }

  void cmdCircle(std::istream& in, std::ostream&, Data& data)
  {
    std::string name = readWord(in);
    if (data.shapes.count(name))
    {
      throw std::invalid_argument("dup");
    }

    int r = readInt(in);
    if (r <= 0)
    {
      throw std::invalid_argument("incorrect radius");
    }

    int x = readInt(in);
    int y = readInt(in);
    data.shapes[name] = std::make_unique< Circle >(
        static_cast< double >(r), Point{static_cast< double >(x), static_cast< double >(y)});
  }

  void cmdSet(std::istream& in, std::ostream&, Data& data)
  {
    std::string name = readWord(in);
    if (data.sets.count(name))
    {
      throw std::invalid_argument("dup set");
    }

    std::size_t size = readSize(in);
    Set_t s;
    s.reserve(size);
    for (std::size_t i = 0; i < size; ++i)
    {
      std::string shapeName = readWord(in);
      if (!data.shapes.count(shapeName))
      {
        throw std::invalid_argument("incorrect shape-name");
      }

      s.push_back(shapeName);
    }

    data.sets[name] = std::move(s);
  }

  void cmdShow(std::istream& in, std::ostream& out, Data& data)
  {
    std::string name = readWord(in);
    if (!data.shapes.count(name))
    {
      throw std::invalid_argument("not find this shape");
    }

    const Shape& s = *data.shapes.at(name);
    const double r = (s.getMaxX() - s.getMinX()) / 2.0;
    const double x = (s.getMaxX() + s.getMinX()) / 2.0;
    const double y = (s.getMaxY() + s.getMinY()) / 2.0;
    out << r << " (" << x << ' ' << y << ")\n";
  }

  void cmdShowSet(std::istream& in, std::ostream& out, Data& data)
  {
    std::string name = readWord(in);
    auto it = data.sets.find(name);
    if (it == data.sets.end())
    {
      throw std::invalid_argument("no set");
    }

    for (const auto& sn : it->second)
    {
      const Shape& s = *data.shapes.at(sn);
      const double r = (s.getMaxX() - s.getMinX()) / 2.0;
      const double cx = (s.getMaxX() + s.getMinX()) / 2.0;
      const double cy = (s.getMaxY() + s.getMinY()) / 2.0;
      out << r << " (" << cx << ' ' << cy << ")\n";
    }
  }

  void cmdFrame(std::istream& in, std::ostream& out, Data& data)
  {
    std::string name = readWord(in);
    auto it = data.shapes.find(name);
    if (it == data.shapes.end())
    {
      throw std::invalid_argument("no shape");
    }

    const Shape& s = *it->second;
    out << '(' << s.getMinX() << ' ' << s.getMinY() << ") (" << s.getMaxX() << ' ' << s.getMaxY() << ")\n";
  }

  void cmdFrameSet(std::istream& in, std::ostream& out, Data& data)
  {
    std::string name = readWord(in);
    auto it = data.sets.find(name);
    if (it == data.sets.end())
    {
      throw std::invalid_argument("no set");
    }

    double minX = 0, maxX = 0, minY = 0, maxY = 0;
    bool first = true;
    for (const auto& sn : it->second)
    {
      const Shape& s = *data.shapes.at(sn);
      if (first)
      {
        minX = s.getMinX();
        maxX = s.getMaxX();
        minY = s.getMinY();
        maxY = s.getMaxY();
        first = false;
      }
      else
      {
        if (s.getMinX() < minX)
        {
          minX = s.getMinX();
        }

        if (s.getMaxX() > maxX)
        {
          maxX = s.getMaxX();
        }

        if (s.getMinY() < minY)
        {
          minY = s.getMinY();
        }

        if (s.getMaxY() > maxY)
        {
          maxY = s.getMaxY();
        }
      }
    }

    out << '(' << minX << ' ' << minY << ") (" << maxX << ' ' << maxY << ")\n";
  }

  void cmdSpawn(std::istream& in, std::ostream&, Data&)
  {
    std::string name = readWord(in);
    std::size_t seed = readSize(in);
    pm.spawn(name, seed);
  }

  void cmdAreaOn(std::istream& in, std::ostream&, Data& data)
  {
    std::string procName = readWord(in);
    std::string calcName = readWord(in);
    std::string setName = readWord(in);
    int threads = readInt(in);
    int tries = readInt(in);

    if (!data.sets.count(setName))
    {
      throw std::invalid_argument("no set");
    }
    if (tries <= 0 || threads < 0)
    {
      throw std::invalid_argument("incorrect params");
    }

    Calculation c;
    c.processName = procName;
    c.setName = setName;
    c.threads = threads;
    c.tries = tries;
    c.state = StateProcess::ACTIVE;

    pm.areaOn(calcName, c, data);
  }

  void cmdStatus(std::istream& in, std::ostream& out, Data&)
  {
    std::string calcName = readWord(in);
    pm.status(calcName, out);
  }

  void cmdWait(std::istream& in, std::ostream& out, Data&)
  {
    std::string calcName = readWord(in);
    pm.wait(calcName, out);
  }

}
