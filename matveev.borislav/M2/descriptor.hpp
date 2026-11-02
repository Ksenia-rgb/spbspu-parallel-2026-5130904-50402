#ifndef MATVEEV_DESCRIPTOR_HPP
#define MATVEEV_DESCRIPTOR_HPP

namespace matveev
{
  constexpr int invalidDescriptor = -1;
  class Descriptor final
  {
  public:
    Descriptor(const Descriptor &) = delete;
    explicit Descriptor(int value = invalidDescriptor) noexcept;
    ~Descriptor();
    Descriptor &operator=(const Descriptor &) = delete;
    int get() const noexcept;
    int release() noexcept;
    void reset(int value = invalidDescriptor) noexcept;

  private:
    int value_;
  };
  void makeNonblocking(int descriptor);
}

#endif
