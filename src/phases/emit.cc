#include "phase.h"

#include "foreach.h"

struct emit
  : visitor
{
  annotation_map &annots;

  void visit (t_toplevel_declarations &n);

  emit (annotation_map &annots)
    : annots (annots)
  {
  }

#if 1
  ~emit ()
  {
    if (!std::uncaught_exception ())
      throw 0;
  }
#endif
};

static phase<emit> thisphase ("emit", "instantiate_rules", "resolve_shortvars");


void
emit::visit (t_toplevel_declarations &n)
{
  phases::run ("emit_SOURCES", &n, annots);
  phases::run ("emit_rules", &n, annots);
}
