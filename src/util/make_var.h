#pragma once

static inline generic_node_ptr
make_var (std::string const &value)
{
  return new t_vardecl_body (location::generated,
           new token (location::generated, TK_CODE, value));
}

