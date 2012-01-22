#include "util/inference_engine.h"

#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

typedef std::string S;
typedef boost::regex R;
typedef inference_engine::wildcard W;

int
main ()
{
  inference_engine dag;

  dag.add_file ("yacc/cow.y.in");
  dag.add_rule ("%", W ("%.in"));
  dag.add_rule ("src/%.c", W ("yacc/%.y"));
  dag.add_rule ("inc/%.h", W ("yacc/%.y"));
  dag.add_rule ("inc/moo.h", R ("yacc/cow.y"));

  dag.add_rule ("%.c", R ("(.+).as"));
  dag.add_rule ("testsuite/%.c", R ("testsuite/(.+).as"));

  dag.add_file ("aldor.as");
  dag.add_file ("testsuite/aldor-test.as");

  dag.add_file ("/usr/bin/gcc");
#if 1
  int const FILES = 2;
  for (int i = 0; i < FILES; i++)
    dag.add_file ("src/foo" + boost::lexical_cast<std::string> (i) + ".c");
  for (int i = 0; i < FILES; i++)
    dag.add_file ("inc/foo" + boost::lexical_cast<std::string> (i) + ".h");
#endif

  dag.add_rule ("%.o", S ("/usr/bin/gcc"), R ("src/(.+)\\.c"), R ("inc/(.+)\\.h"));
#if 0
  dag.add_rule ("obj/%.o", R ("src/(.+)\\.c"), R ("inc/(.+)\\.h"));
  for (int i = 0; i < 13; i++)
    dag.add_rule ("nop/%.o", R ("(.+)\\.c"), R ("inc/(.+)\\.h"));
#endif

  puts ("-- before inference --");
  if (FILES < 10)
    dag.print ();
  puts ("\n-- inference --");
  dag.infer ();
  puts ("\n-- after inference --");
  if (FILES < 10)
    dag.print ();
}
