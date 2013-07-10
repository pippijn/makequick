#include "util/timer.h"

#include <algorithm>
#include <cstdio>
#include <numeric>

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

timer::timer (char const *name)
  : name (name)
  , start (now ())
{
}

timer::~timer ()
{
  double time = 0 + (now () - start);
  if (time > 0.1)
    printf ("%%%% %s: %.3gsec\n", name, time);
}


lex_timer::lex_timer (char const *name)
  : timer (name)
  , start (now ())
  , prev (start)
  , bytes (0)
{
}

lex_timer::~lex_timer ()
{
#if LEX_TIMING
  sort (times.begin (), times.end ());
  if (times.empty ())
    return;
  times.pop_back ();

  double average = accumulate (times.begin (),
                               times.end (),
                               0.0) / times.size ();
  double total = 0.0 + (prev - start);

  printf ("%%%% lex/parse: %lu token%s - avg: %fsec (%.1f tokens/sec) %lu bytes (%.3f MiB @ %.1f MiB/sec)\n",
          times.size (),
          times.size () == 1 ? "" : "s",
          average,
          1 / average,
          bytes,
          double (bytes) / 1024 / 1024,
          double (bytes) / total / 1024 / 1024);
#endif
}

void
lex_timer::next ()
{
  timeval curr = now ();
  timeval diff = curr - prev;
  times.push_back (diff);
  prev = curr;
}
