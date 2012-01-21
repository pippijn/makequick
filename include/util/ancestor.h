#pragma once

#include "node.h"

template<typename T>
boost::intrusive_ptr<T>
ancestor (node_ptr n)
{
  while ((n = n->parent ()) && !n->is<T> ())
    ;
  return n ? n->is<T> () : NULL;
}

template<typename T>
boost::intrusive_ptr<T>
ancestor (node &n)
{
  return ancestor<T> (&n);
}
