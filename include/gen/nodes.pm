{
   extra_dist => ['cond', 'sources'],
   nodist_sources => ['cond', 'sources'],
   built_sources => ['sources'],
   sources => ['cond', 'sources'],
   test => ['type', 'cond', 'sources'],

   headers => ['cond', 'dest', 'sources'],
   mans => ['cond', 'dest', 'sources'],
   data => ['cond', 'dest', 'sources'],
   scripts => ['cond', 'dest', 'sources'],

   flag_import => ['tool', 'target'],

   dir => ['name', 'dir'],
   ac_check => ['checks'],
   arg_enable => ['id', 'dflt', 'desc', 'body'],
   arg_enable_choice => ['id', 'body'],
   arg_enable_choices => ['+'],
   check_alignof => ['type'],
   check_flags => ['name', 'var', 'flags'],
   check_functions => ['funcs'],
   check_headers => ['names'],
   check_library => ['lib', 'sym', 'hdr', 'notfound', 'desc'],
   check_sizeof => ['type'],
   define => ['id', 'string'],
   destination => ['dir'],
   document => ['decls'],
   error => ['msg'],
   exclude => ['sources'],
   log_compilers => ['+'],
   log_compiler => ['ext', 'rule'],
   filename => ['+'],
   filenames_0 => ['*'],
   filenames_1 => ['+'],
   flags => ['*'],
   identifiers => ['+'],
   if => ['cond'],
   extern => ['import'],
   inheritance => ['base'],
   library => ['defn'],
   link => ['cond', 'items'],
   link_body => ['+'],
   program => ['defn'],
   project => ['name', 'defn'],
   project_version => ['version'],
   project_contact => ['contact'],
   project_header => ['header'],
   project_members => ['+'],
   rule => ['target', 'prereq', 'code'],
   rule_line => ['+'],
   rule_lines => ['+'],
   section => ['name', 'body'],
   section_members => ['+'],
   sources_members => ['+'],
   sourcesref => ['ref'],
   target_definition => ['name', 'derive', 'cond', 'dest', 'body'],
   target_members => ['*'],
   template => ['defn'],
   toplevel_declarations => ['+'],

   tool_flags => ['keyword', 'cond', 'flags'],
   flags => ['*'],
   flag => ['+'],

   vardecl => ['var', 'body'],
   varadd => ['var', 'body'],
   vardecl_body => ['+'],

   shortvar => ['var'],
   intvar => ['num'],
   roundvar => ['name'],
   squarevar => ['name'],
   callvar => ['name', 'arg'],
   accessvar => ['name', 'member'],
   filtervar => ['name', 'member', 'filter'],
}
