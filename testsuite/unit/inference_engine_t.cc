#include "util/inference_engine.h"

#include <boost/lexical_cast.hpp>

typedef std::string S;
typedef boost::regex R;

int
main ()
{
  inference_engine dag;

  dag.add_file ("yacc/cow.y.in");
  dag.add_rule ("%", R ("(.+)\\.in"));
  dag.add_rule ("src/%.c", R ("yacc/(.+).y"));
  dag.add_rule ("inc/%.h", R ("yacc/(.+).y"));

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
