#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

static void
throw_signal (int signum)
{
  throw std::runtime_error (strsignal (signum));
}

static void
try_exit (int signum)
{
  puts ("\ninterrupt caught - terminating");
  signal (signum, SIG_DFL);
  exit (EXIT_FAILURE);
}

static bool
init ()
{
  signal (SIGINT, try_exit);
  signal (SIGSEGV, throw_signal);
  return true;
}

static bool const inited = init ();
