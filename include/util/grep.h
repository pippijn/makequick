#pragma once

#include <boost/range.hpp>

#include "node.h"

template<typename T>
struct type_filter
{
  typedef node_vec base;

  typedef typename base::difference_type difference_type;

  type_filter (base::iterator begin,
               base::iterator end)
    : bt (begin)
    , et (end)
  {
  }

  struct iterator
  {
    typedef typename node_vec::iterator		base;
    typedef typename base::iterator_category	iterator_category;
    typedef typename base::difference_type	difference_type;
    typedef T					value_type;
    typedef T &					reference;
    typedef T *					pointer;

    bool ok () const
    {
      assert (cur >= container.bt);
      assert (cur <= container.et);
      return cur >= container.et || *cur && (*cur)->template is<T> ();
    }

    iterator (type_filter const &container, base it)
      : container (container)
      , cur (it)
    {
      while (!ok ())
        ++cur;
    }

    iterator &operator ++ ()
    {
      do
        ++cur;
      while (!ok ());
      return *this;
    }

    bool operator == (iterator const &rhs) const
    {
      return cur == rhs.cur;
    }

    T &operator * ()
    {
      return (*cur)->as<T> ();
    }

    type_filter const &container;
    base cur;
  };

  typedef iterator const_iterator;

  iterator begin () const
  {
    return iterator (*this, bt);
  }

  iterator end () const
  {
    return iterator (*this, et);
  }

  typename base::iterator const bt;
  typename base::iterator const et;
};

template<typename T>
type_filter<T>
grep (node_ptr const &p)
{
  generic_node &n = p->as<generic_node> ();
  return type_filter<T> (n.list.begin (),
                         n.list.end ());
}

template<typename T>
type_filter<T>
grep (node &p)
{
  return grep<T> (&p);
}
