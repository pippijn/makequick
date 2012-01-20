#include "phase.h"

#include "annotations/symbol_table.h"
#include "util/extract_string.h"
#include "util/foreach.h"
#include "util/make_var.h"
#include "util/symbol_visitor.h"
#include "util/unique_visitor.h"

#include <boost/filesystem/path.hpp>

struct expand_vars
  : symbol_visitor
  , unique_visitor
{
  void visit (t_callvar &n);
  void visit (t_roundvar &n);
  bool override (generic_node &n);

  generic_node_ptr sym;

  expand_vars (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
  {
  }
};

static phase<expand_vars> thisphase ("expand_vars", "insert_vardecl_syms", "insert_varadd_syms");


static struct function_calls
{
  typedef generic_node_ptr (*function_type) (generic_node &n);
  typedef std::map<std::string, function_type> function_map;

  // path query functions
  template<bool (fs::path::*func) () const>
  static generic_node_ptr path_function (generic_node &n)
  {
    if (t_vardecl_body_ptr body = n.is<t_vardecl_body> ())
      {
        // TODO: add semantic_error
        assert (body->size () == 1);
        return make_var ((fs::path (extract_string (*body)).*func) () ? "1" : "0");
      }
    return NULL;
  }

  // path decomposition functions
  template<fs::path (fs::path::*func) () const>
  static generic_node_ptr path_function (generic_node &n)
  {
    if (t_vardecl_body_ptr body = n.is<t_vardecl_body> ())
      {
        // TODO: add semantic_error
        assert (body->size () == 1);
        return make_var ((fs::path (extract_string (*body)).*func) ().native ());
      }
    return NULL;
  }

#define FS_FUNC(name) static generic_node_ptr name (generic_node &n) { return path_function<&fs::path::name> (n); }
  // decomposition
  FS_FUNC (root_name)
  FS_FUNC (root_directory)
  FS_FUNC (root_path)
  FS_FUNC (relative_path)
  FS_FUNC (parent_path)
  FS_FUNC (filename)
  FS_FUNC (stem)
  FS_FUNC (extension)

  // query
  FS_FUNC (empty)
  FS_FUNC (has_root_name)
  FS_FUNC (has_root_directory)
  FS_FUNC (has_root_path)
  FS_FUNC (has_relative_path)
  FS_FUNC (has_parent_path)
  FS_FUNC (has_filename)
  FS_FUNC (has_stem)
  FS_FUNC (has_extension)
  FS_FUNC (is_absolute)
  FS_FUNC (is_relative)
#undef FS_FUNC

  // GNU make compatibility names
  static generic_node_ptr dir    (generic_node &n) { return parent_path (n); }
  static generic_node_ptr notdir (generic_node &n) { return filename    (n); }


  function_calls ()
  {
#define CALL(name) calls[#name] = name
    // decomposition
    CALL (root_name);
    CALL (root_directory);
    CALL (root_path);
    CALL (relative_path);
    CALL (parent_path);
    CALL (filename);
    CALL (stem);
    CALL (extension);

    // query
    CALL (empty);
    CALL (has_root_name);
    CALL (has_root_directory);
    CALL (has_root_path);
    CALL (has_relative_path);
    CALL (has_parent_path);
    CALL (has_filename);
    CALL (has_stem);
    CALL (has_extension);
    CALL (is_absolute);
    CALL (is_relative);

    // GNU make compatibility names
    CALL (dir);
    CALL (notdir);
#undef CALL
  }

  generic_node_ptr operator () (std::string const &name, generic_node &n) const
  {
    function_map::const_iterator found = calls.find (name);
    // TODO: add semantic_error
    assert (found != calls.end ());
    return found->second (n);
  }

  function_map calls;
} const call;


void
expand_vars::visit (t_callvar &n)
{
  visitor::visit (n);
  sym = call (id (n.name ()), n.arg ()->as<generic_node> ());
}

void
expand_vars::visit (t_roundvar &n)
{
  std::string const &name = id (n.name ());
  sym = symtab.lookup (T_VARIABLE, name);
}


bool
expand_vars::override (generic_node &n)
{
  if (done (n))
    return true;
  mark (n);

  local (sym);
  foreach (node_ptr const &p, n.list)
    {
      if (!p)
        continue;

      sym = NULL;
      p->accept (*this);

      if (sym)
        {
          if (!done (*sym))
            sym->accept (*this);
          node_ptr body = sym->as<t_vardecl_body> ().clone ();
          n.set (p->parent_index (), body);
        }
    }

  return true;
}
