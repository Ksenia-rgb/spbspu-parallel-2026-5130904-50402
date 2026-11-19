#include "process.hpp"
#include <unistd.h>
#include <cstddef>

namespace muhamadiarov
{

  std::size_t send(int& err, int wr, const void* buffer, std::size_t k) const noexcept
  {
    char* const p = static_cast< char* >(buffer);
    std::size_t r = 0;
    while (r < k)
    {
      err = write(wr, p + r, k - r);
      if (err < 0)
      {
        break;
      }
      r += err;
    }
    return r;
  }

  std::size_t take(int& err, int rd, void* buffer, std::size_t k) const noexcept
  {
    char* const p = static_cast< char* >(buffer);
    std::size_t r = 0;
    while (r < k)
    {
      err = read(rd, p + r, k - r);
      if (err < 0)
      {
        break;
      }
      r += err;
    }
    return r;
  }

}
