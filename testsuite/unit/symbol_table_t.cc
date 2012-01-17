#include "annotations/symbol_table.h"
#include "node.h"

using namespace nodes;

struct test
{
  symbol_table symtab;

  generic_node_ptr prog, lib, tmpl, var;

  test ()
    : prog (make_node<n_program> (location::generated, NULL)->is<generic_node> ())
    , lib (make_node<n_library> (location::generated, NULL)->is<generic_node> ())
    , tmpl (make_node<n_template> (location::generated, NULL)->is<generic_node> ())
    , var (make_node<n_vardecl> (location::generated, NULL, NULL)->is<generic_node> ())
  {
    assert (prog);
    assert (lib);
    assert (tmpl);
    assert (var);
  }

  void test_insert ()
  {
    // insert the first time
    assert (symtab.insert (T_PROGRAM, "prog", prog));
    assert (symtab.insert (T_LIBRARY, "lib", lib));
    assert (symtab.insert (T_TEMPLATE, "tmpl", tmpl));
    assert (symtab.insert (T_VARIABLE, "var", var));

    // fail to insert the second time
    assert (!symtab.insert (T_PROGRAM, "prog", prog));
    assert (!symtab.insert (T_LIBRARY, "lib", lib));
    assert (!symtab.insert (T_TEMPLATE, "tmpl", tmpl));
    assert (!symtab.insert (T_VARIABLE, "var", var));
  }

  void test_lookup ()
  {
    assert (symtab.lookup (T_PROGRAM, "prog") == prog);
    assert (symtab.lookup (T_LIBRARY, "lib") == lib);
    assert (symtab.lookup (T_TEMPLATE, "tmpl") == tmpl);
    assert (symtab.lookup (T_VARIABLE, "var") == var);
  }
};

int
main ()
{
  test t;

  assert (t.symtab.begin () == t.symtab.end ());

  // global insert
  t.test_insert ();
  t.test_lookup ();

  // scoped lookup and insert
  t.symtab.enter_scope (t.prog);
  t.test_lookup ();
  t.test_insert ();
  t.test_lookup ();
  t.symtab.leave_scope ();

  // another lookup
  t.test_lookup ();

  symbol_table::iterator it = t.symtab.begin ();

  std::cout << *it << "\n"; assert (it->name == "var"); ++it;
  std::cout << *it << "\n"; assert (it->name == "prog"); ++it;
  std::cout << *it << "\n"; assert (it->name == "lib"); ++it;
  std::cout << *it << "\n"; assert (it->name == "tmpl"); ++it;
  std::cout << *it << "\n"; assert (it->name == "var"); ++it;
  std::cout << *it << "\n"; assert (it->name == "prog"); ++it;
  std::cout << *it << "\n"; assert (it->name == "lib"); ++it;
  std::cout << *it << "\n"; assert (it->name == "tmpl"); ++it;

  assert (it == t.symtab.end ());

  --it; std::cout << *it << "\n"; assert (it->name == "tmpl");
  --it; std::cout << *it << "\n"; assert (it->name == "lib");
  --it; std::cout << *it << "\n"; assert (it->name == "prog");
  --it; std::cout << *it << "\n"; assert (it->name == "var");
  --it; std::cout << *it << "\n"; assert (it->name == "tmpl");
  --it; std::cout << *it << "\n"; assert (it->name == "lib");
  --it; std::cout << *it << "\n"; assert (it->name == "prog");
  --it; std::cout << *it << "\n"; assert (it->name == "var");

  assert (it == t.symtab.begin ());

  while (it != t.symtab.end ())
    {
      ++it;
    }

  std::cout.put ('\n');

  symbol_table::filtering_iterator<T_PROGRAM> fit = t.symtab.begin<T_PROGRAM> ();
  symbol_table::filtering_iterator<T_PROGRAM> fet = t.symtab.end<T_PROGRAM> ();

  assert (fit != fet);

  std::cout << *fit << "\n"; assert (fit->type == T_PROGRAM); ++fit;
  std::cout << *fit << "\n"; assert (fit->type == T_PROGRAM); ++fit;

  assert (fit == fet);
}
