#pragma once

#include "util/foreach.h"

static inline t_filename_ptr
make_filename (location const &loc, std::string const &name)
{
  return new t_filename (loc, new token (loc, TK_FILENAME, name));
}

static inline t_filename_ptr
make_filename (std::string const &name)
{
  return make_filename (location::generated, name);
}


static inline t_filename_ptr
make_filename (location const &loc, node_vec const &nodes)
{
  t_filename_ptr fn = new t_filename;
  fn->loc = loc;
  foreach (node_ptr const &p, nodes)
    fn->add (p);
  return fn;
}

static inline t_filename_ptr
make_filename (node_vec const &nodes)
{
  return make_filename (location::generated, nodes);
}


static inline t_filename_ptr
make_filename (location const &loc, token_vec const &tokens)
{
  t_filename_ptr fn = new t_filename;
  fn->loc = loc;
  foreach (token_ptr const &p, tokens)
    fn->add (p);
  return fn;
}

static inline t_filename_ptr
make_filename (token_vec const &tokens)
{
  return make_filename (location::generated, tokens);
}
