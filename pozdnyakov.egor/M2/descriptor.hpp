#ifndef DESCRIPTOR_HPP
#define DESCRIPTOR_HPP

#include <cstddef>
#include <utility>

namespace pozdnyakov
{
  class FileDescriptor
  {
  public:
    FileDescriptor() noexcept;
    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor(FileDescriptor&& rhs) noexcept;
    explicit FileDescriptor(int descriptor) noexcept;
    ~FileDescriptor();

    FileDescriptor& operator=(const FileDescriptor&) = delete;
    FileDescriptor& operator=(FileDescriptor&& rhs) noexcept;

    int get() const noexcept;

  private:
    int descriptor_;
  };

  using pipe_ends_t = std::pair< FileDescriptor, FileDescriptor >;

  pipe_ends_t makePipe();
  void writeAll(const FileDescriptor& descriptor, const void* data, std::size_t size);
  bool readAll(const FileDescriptor& descriptor, void* data, std::size_t size);
  bool waitReadable(const FileDescriptor& descriptor, int milliseconds);
}

#endif
