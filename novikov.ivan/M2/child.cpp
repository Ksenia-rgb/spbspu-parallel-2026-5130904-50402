#include "child.hpp"
#include <unistd.h>
#include "area.hpp"
#include "ellipse.hpp"
#include "timer.hpp"

void novikov::childProcess(DuplexChannel channel, size_t seed)
{
  try
  {
    while (true)
    {
      size_t size = 0;
      channel.read(&size, sizeof(size));

      ellipse_list ellipses(size);
      channel.read(ellipses.data(), static_cast< ssize_t >(size * sizeof(Ellipse)));

      size_t threads = 0, tries = 0;
      channel.read(&threads, sizeof(threads));
      channel.read(&tries, sizeof(tries));

      double init{0}, total{0};
      const novikov::Clicker cl;

      init = cl.millisec();
      const double res = novikov::area(ellipses, threads, tries, seed).first;
      total = cl.millisec();

      double time = total - init;
      channel.write(&res, sizeof(res));
      channel.write(&time, sizeof(time));
    }
  }
  catch (...)
  {
    _exit(0);
  }
}
