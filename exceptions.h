////////////////////////////////
/////E X C E P T I O N S////////
////////////////////////////////
#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H
#include<exception>

class ex_symbol_not_found_t : public std::exception
{
public:
  virtual const char* what();
};

class ex_array_as_single_t : public std::exception
{
public:
  virtual const char* what();
};

class ex_arr_dim_mismatch_t : public std::exception
{
public:
  virtual const char* what();
};

class ex_exceed_level_limit_t : public std::exception
{
public:
  virtual const char* what();
};

extern ex_symbol_not_found_t ex_symbol_not_found;
extern ex_array_as_single_t ex_array_as_single;
extern ex_arr_dim_mismatch_t ex_arr_dim_mismatch;
extern ex_exceed_level_limit_t ex_exceed_level_limit;
#endif
