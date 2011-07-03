#include "annotations/build_dag.h"

#include <boost/lexical_cast.hpp>

using annotations::build_dag;

typedef std::string S;
typedef boost::regex R;

int
main ()
{
  build_dag dag;

  dag.add_file ("/usr/bin/gcc");
  dag.add_file ("foo.c");
  dag.add_file ("foo.h");
#if 0
  for (int i = 0; i < 1000; i++)
    {
      dag.add_file ("foo" + boost::lexical_cast<std::string> (i) + ".c");
      dag.add_file ("foo" + boost::lexical_cast<std::string> (i) + ".h");
    }
#endif

  dag.add_rule ("%.o", S ("/usr/bin/gcc"), R ("(.+)\\.c"), R ("(.+)\\.h"));
  dag.add_rule ("%", R ("(.+)\\.o"));

  puts ("-- before inference --");
  dag.print ();
  puts ("\n-- inference --");
  dag.infer ();
  puts ("\n-- after inference --");
  dag.print ();
}
