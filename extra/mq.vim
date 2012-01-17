if !exists("main_syntax")
	if version < 600
		syntax clear
	elseif exists("b:current_syntax")
		finish
	endif
	let main_syntax = 'mq'
endif

set iskeyword=@,45,48-57,_,192-255

syn keyword	mqCommentTodo		TODO FIXME XXX TBD contained
syn match	mqLineComment		"#.*" contains=@Spell,mqCommentTodo
syn match	mqSpecial		"\\\d\d\d\|\\."
syn region	mqString		start=+"+  skip=+\\\\\|\\"+  end=+"+  contains=mqSpecial,mqVariable
syn region	mqString		start=+'+  skip=+\\\\\|\\'+  end=+'+  contains=mqSpecial,mqVariable
syn region	mqString		start=+`+  skip=+\\\\\|\\`+  end=+`+  contains=mqSpecial,mqVariable

syn match	mqVariable		"\$[$*@0-9^<]"
syn match	mqVarDecl		"\v^\s*[A-Z_]+(\s*\=)@="
syn region	mqVariable		start=+\$(+  end=+)+  contains=mqVariable
syn region	mqVariable		start=+\$\[+  end=+]+  contains=mqVariable

syn match	mqElementNode		"[a-z]\+flags"
syn keyword	mqElementNode		program library sources link build custom ex li
syn keyword	mqElementNode		extra_dist nodist_sources rule project section headers mans data
syn keyword	mqElementNode		functions arg_enable arg_with options define c_bigendian c_typeof
syn keyword	mqElementNode		c_charset c_enum_fwdecl c_late_expansion c_token_paste c_float_format
syn keyword	mqElementNode		c_stdint_h alignof c_stmt_exprs output exclude pkg-config template
syn keyword	mqElementNode		verbatim extend global
syn keyword	mqOperator		if
syn match	mqOperator		"->\|=>\|=\|:\|+="

syn match	mqBraces		"[{}]"
syn match	mqParens		"[()]"

syn sync fromstart
syn sync maxlines=100

if main_syntax == "mq"
	syn sync ccomment mqComment
endif

hi def link mqComment		Comment
hi def link mqLineComment	Comment
hi def link mqCommentTodo	Todo
hi def link mqSpecial		Special
hi def link mqCharacter		Character
hi def link mqTarget		Constant
hi def link mqString		String
hi def link mqOperator		Operator
hi def link mqElementNode	Type
hi def link mqBraces		Function
hi def link mqVariable		Function
hi def link mqVarDecl		Function

hi def link mqBindingProperty	Label


let b:current_syntax = "mq"
if main_syntax == 'mq'
	unlet main_syntax
endif

" vim: ts=8
