#include "phase.h"

#include "annotations/symbol_table.h"
#include "util/extract_string.h"
#include "util/foreach.h"
#include "util/grep.h"
#include "util/make_code.h"
#include "util/symbol_visitor.h"

struct expand_filtervars
  : symbol_visitor
{
  bool inherits (std::string const &lib, std::string const &tmpl)
  {
    generic_node_ptr ref = symtab.lookup (T_LIBRARY, T_TEMPLATE, lib);
    assert (ref && "the existence should have been checked, before");

    t_target_definition &target = ref->as<t_target_definition> ();
    if (!target.derive ())
      return false;
    std::string base = id (target.derive ()->as<t_inheritance> ().base ());
    if (base == tmpl)
      return true;
    return inherits (base, tmpl);
  }

  void visit (t_filtervar &n);

  expand_filtervars (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
  {
  }
};

static phase<expand_filtervars> thisphase ("expand_filtervars", "insert_target_syms");


void
expand_filtervars::visit (t_filtervar &n)
{
  generic_node_ptr ref = symtab.lookup (T_PROGRAM, T_LIBRARY, id (n.name ()));
  if (!ref)
    return;

  t_target_definition &target = ref->as<t_target_definition> ();

  if (id (n.member ()) == "link")
    {
      std::string links;
      foreach (t_link &link, grep<t_link> (target.body ()))
        foreach (node_ptr const &lib, link.items ()->as<t_link_body> ().list)
          {
            token &tok = lib->as<token> ();
            if (tok.tok == TK_INT_LIB)
              if (inherits (tok.string, id (n.filter ())))
                {
                  if (!links.empty ())
                    links += ' ';
                  links += tok.string;
                }
          }
      if (n.parent ()->is<t_rule_line> ())
        n.parent ()->replace (n, make_code (n.loc, links));
      else
        assert (!"TODO: add error");
    }
  else
    {
      assert (!"TODO: add error");
    }
}
