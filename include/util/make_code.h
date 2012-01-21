#pragma once

static inline node_ptr
make_code (location const &loc, std::string const &value)
{
  return new token (loc, TK_CODE, value);
}

static inline node_ptr
make_code (std::string const &value)
{
  return make_code (location::generated, value);
}
