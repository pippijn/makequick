#include "sighandler.h"

bool should_terminate = false;

static void
throw_signal (int signum)
{
  if (should_terminate)
    {
      puts ("second interrupt caught - exiting");
      exit (1);
    }
  puts ("interrupt caught - terminating");
  should_terminate = true;
}

static bool
init ()
{
  signal (SIGINT, throw_signal);
  return true;
}

static bool const inited = init ();
