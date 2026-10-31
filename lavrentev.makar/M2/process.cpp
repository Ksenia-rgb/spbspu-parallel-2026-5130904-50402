#include "process.hpp"

lavrentev::Process::Process(std::string process_name):
  process_name_(process_name)
{}

double lavrentev::Process::get_result() const
{
  return result_;
}

void lavrentev::Process::set_seed(int seed)
{
  seed_ = seed;
}
