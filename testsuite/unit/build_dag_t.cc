#include "annotations/build_dag.h"

#include <boost/lexical_cast.hpp>

using annotations::build_dag;

typedef std::string S;
typedef boost::regex R;

int
main ()
{
  build_dag dag;

  dag.add_file ("yacc/cow.y.in");
  dag.add_rule ("yacc/cow.y", S ("yacc/cow.y.in"));
  dag.add_rule ("src/cow.c", S ("yacc/cow.y"));
  dag.add_rule ("inc/cow.h", S ("yacc/cow.y"));

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
