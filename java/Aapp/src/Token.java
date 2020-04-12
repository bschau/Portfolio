package com.schau;

/**
 * Tokens as seen by the Tokenizer and used by the Expression parser.
 *
 * BIG FAT WARNING: If you reorder the tokens from ADD and on to (and incl.)
 * EOF you must update the tbl in Expression.java.
 */
public enum Token {
	ADD,
	SUB,
	MUL,
	DIV,
	UMINUS,
	LPARAN,
	RPARAN,
	NOT,
	EQ,
	NE,
	GT,
	GE,
	LT,
	LE,
	LAND,
	LOR,
	AND,
	OR,
	XOR,
	CAT,
	EOF,
	// From this point on you can add/change tokens as you like.
	VAL,
	STRING,
	ID,
	INCLUDE,
	DEFINE,
	REDEF,
	UNDEF,
	EXPAND,
	IF,
	ELIF,
	IFDEF,
	IFNDEF,
	ELSE,
	ENDIF,
	ECHO,
	RUN,
	END,
	REM,
	EMPTYSYSTEMTOKEN,
};
