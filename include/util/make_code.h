#pragma once

static inline node_ptr
make_code (std::string const &value)
{
  return new token (location::generated, TK_CODE, value);
}

