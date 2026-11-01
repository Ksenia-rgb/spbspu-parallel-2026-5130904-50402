#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <cstddef>

namespace shirokov
{
  constexpr int status_ok = 0;
  constexpr int status_error_invalid_args = 1;
  constexpr int status_error_invalid_input = 2;

  constexpr int count_of_args_without_seed = 3;
  constexpr int count_of_args_with_seed = 4;

  constexpr size_t threads_arg_pos = 1;
  constexpr size_t tries_arg_pos = 2;
  constexpr int seed_arg_pos = 3;
}

#endif
