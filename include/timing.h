#pragma once

#include <algorithm>
#include <numeric>

#include <sys/time.h>

static inline timeval
now ()
{
  timeval tv;
  gettimeofday (&tv, NULL);
  return tv;
}

static inline timeval
operator - (timeval const &a, timeval const &b)
{
  timeval res;
  timersub (&a, &b, &res);
  return res;
}

static inline bool
operator < (timeval const &a, timeval const &b)
{
  return a.tv_sec < b.tv_sec
      || a.tv_usec < b.tv_usec;
}

static inline double
operator + (double d, timeval const &t)
{
  return d + t.tv_sec + double (t.tv_usec) / 1000000;
}

struct timer
{
  typedef std::vector<timeval> timervec;

  ~timer ()
  {
    std::sort (times.begin (), times.end ());
    if (times.empty ())
      return;
    times.pop_back ();

    double average = std::accumulate (times.begin (), times.end (), 0.0) / times.size ();

    printf ("%d tokens - avg: %.3gsec (%.1f tokens/sec)\n", times.size (), average, 1 / average);
  }

  void next ()
  {
    timeval curr = now ();
    timeval diff = curr - prev;
    times.push_back (diff);
    prev = curr;
  }

  timervec times;
  timeval prev;
};
