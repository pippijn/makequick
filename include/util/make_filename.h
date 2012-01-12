#pragma once

static t_filename_ptr
make_filename (location const &loc, token_vec const &instance)
{
  t_filename_ptr fn = new t_filename (loc, 0);
  fn->list.assign (instance.begin (), instance.end ());
  return fn;
}

static t_filename_ptr
make_filename (location const &loc, std::string const &name)
{
  return new t_filename (loc, new token (loc, TK_FILENAME, name));
}
