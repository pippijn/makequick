install Program ".DEFAULT" [
  (* Target *)
  Name		"makequick";

  Sources [
    "annotations/error_log.cpp";
    "annotations/file_list.cpp";
    "annotations/output_file.cpp";
    "annotations/rule_info.cpp";
    "annotations/symbol_table.cpp";
    "annotations/target_objects.cpp";

    "phases/emit/emit_SOURCES.cpp";
    "phases/emit/emit_SUFFIXES.cpp";
    "phases/emit/emit_built_sources.cpp";
    "phases/emit/emit_dirs.cpp";
    "phases/emit/emit_flags.cpp";
    "phases/emit/emit_headers.cpp";
    "phases/emit/emit_import.cpp";
    "phases/emit/emit_link.cpp";
    "phases/emit/emit_m4.cpp";
    "phases/emit/emit_rules.cpp";
    "phases/emit/emit_targets.cpp";
    "phases/emit/emit_tests.cpp";
    "phases/audit.cpp";
    "phases/clean_rules.cpp";
    "phases/concat_sources.cpp";
    "phases/concat_vardecls.cpp";
    "phases/default_destdir.cpp";
    "phases/default_flags.cpp";
    "phases/default_prereq.cpp";
    "phases/emit.cpp";
    "phases/exclude.cpp";
    "phases/expand_filtervars.cpp";
    "phases/expand_vars.cpp";
    "phases/flatten_filenames.cpp";
    "phases/flatten_rules.cpp";
    "phases/flatten_vars.cpp";
    "phases/imports.cpp";
    "phases/inference.cpp";
    "phases/infer_target_objects.cpp";
    "phases/inheritance.cpp";
    "phases/insert_global_syms.cpp";
    "phases/insert_target_syms.cpp";
    "phases/insert_this_syms.cpp";
    "phases/insert_varadd_syms.cpp";
    "phases/insert_vardecl_syms.cpp";
    "phases/instantiate_rules.cpp";
    "phases/merge_blocks.cpp";
    "phases/multirule.cpp";
    "phases/print.cpp";
    "phases/remove_patrules.cpp";
    "phases/remove_templates.cpp";
    "phases/reparse_vars.cpp";
    "phases/resolve_flagsref.cpp";
    "phases/resolve_shortvars.cpp";
    "phases/resolve_sources.cpp";
    "phases/resolve_sourcesref.cpp";
    "phases/resolve_tools.cpp";
    "phases/resolve_wildcards.cpp";
    "phases/squarevars.cpp";
    "phases/sx.cpp";
    "phases/xml.cpp";

    "util/inference_engine/inference_prereq.cpp";
    "util/extract_string.cpp";
    "util/graph.cpp";
    "util/inference_engine.cpp";
    "util/regex_escape.cpp";
    "util/symbol_visitor.cpp";
    "util/timer.cpp";

    "annotation_map.cpp";
    "colours.cpp";
    "exception.cpp";
    "file_lexer.cpp";
    "lexer.cpp";
    "main.cpp";
    "node.cpp";
    "node_factory.cpp";
    "node_type.cpp";
    "nodes.pb.cpp";
    "parser.cpp";
    "parseutil.cpp";
    "phases.cpp";
    "serialisation.cpp";
    "sighandler.cpp";
    "string_lexer.cpp";
    "visitor.cpp";
    "yyparse.cpp";
    "yylex.cpp";
  ];

  Headers [
    "annotations/error_log.h";
    "annotations/file_list.h";
    "annotations/output_file.h";
    "annotations/rule_info.h";
    "annotations/symbol_table.h";
    "annotations/target_objects.h";

    "boost/filesystem_fwd.hpp";

    "lexer/pimpl.h";
    "lexer/util.h";

    "phases/print.h";

    "stdafx/stdafx.h";

    "util/inference_engine/regex.h";
    "util/inference_engine/string.h";
    "util/inference_engine/wildcard.h";
    "util/ancestor.h";
    "util/canonical.h";
    "util/colours.h";
    "util/extract_string.h";
    "util/foreach.h";
    "util/graph.h";
    "util/grep.h";
    "util/inference_engine.h";
    "util/local.h";
    "util/make_code.h";
    "util/make_filename.h";
    "util/make_var.h";
    "util/move.h";
    "util/object_name.h";
    "util/plus_writer.h";
    "util/regex_escape.h";
    "util/symbol_visitor.h";
    "util/timer.h";
    "util/uc.h";
    "util/unique_visitor.h";
    "util/unlink.h";

    "annotation.h";
    "annotation_map.h";
    "exception.h";
    "file_lexer.h";
    "lexer.h";
    "node_cc.h";
    "node_fwd.h";
    "node.h";
    "nodes.pb.h";
    "node_ptr.h";
    "rule_init.h";
    "node_t.h";
    "node_type.h";
    "object_pool.h";
    "parser.h";
    "parseutil.h";
    "phase.h";
    "phases.h";
    "string_lexer.h";
    "visitor_cc.h";
    "visitor.h";
    "visitor_t.h";
    "ylcode.h";
    "y.tab.h";
    "yyparse.hpp";
    "config.h";
    "yylex.hpp";
    "yystate.h";
  ];

  CRequires [
    "boost_filesystem";
    "boost_regex";
    "boost_system";
    "pthread";
    "protobuf";
  ];

  Var ("OM_YFLAGS", "-d");
  Var ("OM_CPPFLAGS", "-I$(dir .)");

  Code "include ./Generate.om";

  Recurse ("util util/inference_engine annotations phases phases/emit lexer");
]
