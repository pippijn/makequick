#include "sighandler.h"

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

bool should_terminate = false;

static void
maybe_exit (int signum)
{
  if (should_terminate)
    {
      puts ("second interrupt caught - exiting");
      exit (1);
    }
  puts ("interrupt caught - terminating");
  should_terminate = true;
}

static void
throw_signal (int signum)
{
  throw std::runtime_error (strsignal (signum));
}

static bool
init ()
{
  signal (SIGINT, maybe_exit);
  signal (SIGSEGV, throw_signal);
  return true;
}

static bool const inited = init ();
