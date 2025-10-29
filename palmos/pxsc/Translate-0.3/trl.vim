" Vim syntax file
" Language:	trl - Brians internal translation format
" Maintainer:	Brian Schau <brian@schau.com>
" Last change:	2003 Apr 19
" Available on:	nowhere

" Note: Add the following two lines filetype.vim:
"
"       " Translation
"       au BufNewFile,BufRead *.trl           setf trl
"
"       Then copy this file to the syntax directory.
"
 
" Remove any old syntax
if version < 600
	syn clear
elseif exists("b:current_syntax")
	finish
endif

syn case ignore

syn match trlKey "\<\h\w*\>"
syn region trlValue start="\t" end="$"
syn region trlComment start="#" end="$"
syn region trlEOF start="#eof" end="$"

syn sync ccomment trlComment

if version >= 508 || !exists("did_trl_syntax_inits")
	if version < 508
		command -nargs=+ HiLink hi link <args>
	else
		command -nargs=+ HiLink hi def link <args>
	endif

	let did_trl_syntax_inits = 1

	" The default methods for highlighting
	HiLink trlComment		Comment
	HiLink trlValue			Type
	HiLink trlKey			Statement
	HiLink trlEOF			SpecialChar

	delcommand HiLink
endif

let b:current_syntax = "trl"
