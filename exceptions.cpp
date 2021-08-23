#include "exceptions.h"
ex_symbol_not_found_t ex_symbol_not_found;
ex_array_as_single_t ex_array_as_single;
ex_arr_dim_mismatch_t ex_arr_dim_mismatch;
ex_exceed_level_limit_t ex_exceed_level_limit;
const char* ex_symbol_not_found_t::what()
{
  return "Symbol Not Found!";
}

const char* ex_array_as_single_t::what()
{
  return "Array is used as variable, but we don't support addressing";
}

const char* ex_arr_dim_mismatch_t::what()
{
  return "Array's definition dimension and referenced dimension does not agree.";
}

const char* ex_exceed_level_limit_t::what()
{
  return "Eval compile exceed max evaluation level.";
}
