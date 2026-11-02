#include <iostream>

int main(int argc, char* argv[])
{
  if (argc != 3 && argc != 4)
  {
    std::cerr << "err in parametrs\n";
    return 1;
  }

  int seed = 0;
  size_t pos = 0;
  try
  {
    if (argc == 4)
    {
      seed = std::stoi(argv[3], &pos);
      if (pos != std::string(argv[3]).length() || seed < 0)
      {
        throw std::invalid_argument (" wrong parametr");
      }
    }
  } catch (const std::exception& err)
  {
    std::cerr << err.what() << "\n";
    return 1;
  }
}
