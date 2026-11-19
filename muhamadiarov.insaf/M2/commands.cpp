#include "commands.hpp"
#include <istream>
#include <ostream>

namespace muhamadiarov
{
  
  namespace
  {
  
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
    data.shapes[name] = std::make_unique< Circle >(r, Point{x, y});
  }
  
  void cmdSet(std::istream&, std::ostream&, Data&)
  {
    std::string name = readWord(in);
    if (data.shapes.count(name))
    {
      throw std::invalid_argument("dup set");
    }

    Set_t s;
    s.reserve(size);
    std::size_t size = readSize(in);
    for (std::size_t i = 0; i < size; ++i)
    {
      std::string shapeName = readWord(in);
      if (data.shapes.count(shapeName))
      {
        throw std::invalid_argument("incorrect shape-name");
      }

      s.push_back(shapeName);
    }

    data.sets[name] = std::move(s);
  }

  void cmdShow(std::istream&, std::ostream&, Data&);
  void cmdShowSet(std::istream&, std::ostream&, Data&);
  void cmdFrame(std::istream&, std::ostream&, Data&);
  void cmdFrameSet(std::istream&, std::ostream&, Data&);
  void cmdSpawn(std::istream&, std::ostream&, Data&);
  void cmdAreaOn(std::istream&, std::ostream&, Data&);
  void cmdStatus(std::istream&, std::ostream&, Data&);
  void cmdWait(std::istream&, std::ostream&, Data&);

}
