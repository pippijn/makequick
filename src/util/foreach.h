#if __GXX_EXPERIMENTAL_CXX0X__
#  define foreach(a, b) for (a : b)
#else
#  include <boost/foreach.hpp>
#  define foreach BOOST_FOREACH
#endif
