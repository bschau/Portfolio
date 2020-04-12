package com.schau;

import java.util.Stack;

/**
 * The tokenizer. The ppid token has been recognize now parse the rest.
 */
public class Tokenizer {
	/**
	 * Token buffer.
	 */
	private static StringBuffer tBuf=new StringBuffer();

	/**
	 * Aapp.
	 */
	private static Aapp aapp;

	/**
	 * Current token.
	 */
	private static Token token;

	/**
	 * If the token is denoting an INT here is the value.
	 */
	public static int intVal;

	/**
	 * If the token is denoting an STRING here is the value.
	 */
	public static String strVal;

	/**
	 * Initialize the tokenizer.
	 */
	public static void init(Aapp a) {
		aapp=a;
		reset();
	}

	/**
	 * Reset the tokenizer to a known state.
	 */
	public static void reset() {
		token=Token.EOF;
	}

	/**
	 * Handle error. Is used by other classes as well.
	 *
	 * @param e error string.
	 * @exception Exception.
	 */
	public static void error(String e) throws Exception {
		StringBuffer errBuf=new StringBuffer(e);

		errBuf.append(" (");
		errBuf.append(Inputter.getSrcFile());
		errBuf.append(" on line ");
		errBuf.append(Integer.toString(Inputter.getSrcLine()));
		errBuf.append(")");
		errBuf.append(LineSeparator.all);
		Stack fS=Inputter.cloneStack();
		fS.pop();
		while (!fS.empty()) {
			SrcFile s=(SrcFile)fS.pop();
	
			errBuf.append("included from ");
			errBuf.append(s.getName());
			errBuf.append(" on line ");
			errBuf.append(Integer.toString(s.getLine()));
			errBuf.append(LineSeparator.all);
		}

		throw new Exception(errBuf.toString());
	}

	/**
	 * Skip leading whitespaces.
	 *
	 * @exception Exception.
	 */
	public static void skipSpaces() throws Exception {
		int c;

		while ((c=Inputter.getChar())!=-1) {
			if ((char)c==LineSeparator.ls) {
				Inputter.putBack(c);
				break;
			}

			if (!Character.isSpaceChar(c)) {
				Inputter.putBack(c);
				break;
			}
		}
	}

	/**
	 * Get a quoted string. The string can be in single as well as double
	 * quotes.
	 *
	 * @return string.
	 * @exception Exception.
	 */
	private static String getQuotedString() throws Exception {
		int i;
		char c;

		if ((i=Inputter.getChar())==-1) {
			return null;
		}

		c=(char)i;
		if ((c=='\'') || (c=='"')) {
			return getQuotedString(c);
		}

		return getQuotedString((char)0);
	}

	/**
	 * Get a delimetted string.
	 *
	 * @param delimitter.
	 * @return string.
	 * @exception Exception.
	 */
	private static String getQuotedString(char delim) throws Exception {
		int i;
		char c;

		tBuf.delete(0, tBuf.length());
		while (true) {
			if (((i=Inputter.getChar())==-1) || ((char)i==LineSeparator.ls)) {
				error("Unterminated string");
			}


			c=(char)i;
			if (delim==0) {
				if (Character.isSpaceChar(c)) {
					break;
				}
			} else if (c==delim) {
				break;
			}

			tBuf.append(c);
		}

		return tBuf.toString();
	}

	/**
	 * Get an identifier. An identifer can start with a letter, underscore
	 * or $ and then contain letter, digits, underscore, $ and '.'.
	 *
	 * @param c first character of identifier.
	 * @return identifier.
	 * @exception Exception.
	 */
	private static String getIdentifier(char c) throws Exception {
		tBuf.delete(0, tBuf.length());

		if ((Character.isLetter(c)) || (c=='_') || (c=='$')) {
			int i;

			while (true) {
				tBuf.append(c);
				if ((i=Inputter.getChar())==-1) {
					break;
				}

				c=(char)i;
				if (!((Character.isLetterOrDigit(c)) || (c=='_') || (c==':') || (c=='.') || (c=='$'))) {
					Inputter.putBack(i);
					break;
				}
			}

			return tBuf.toString();
		}

		return null;
	}

	/**
	 * Get the next word.   A word is a token or possible the value of an
	 * identifer.
	 *
	 * @param lookupId true - look up the identifier value, false - return
	 *                 identifier.
	 * @return token.
	 * @exception Exception.
	 */
	private static Token getNextWord(boolean lookupId) throws Exception {
		String s;
		char c=' ', d;
		int i, j;

		if ((i=Inputter.getChar())==-1) {
			return Token.EOF;
		}

		c=(char)i;
		if (c==LineSeparator.ls) {
			Inputter.bumpLine();
			return Token.EOF;
		}

		switch (c) {
			case '+':
				return Token.ADD;
			case '-':
				if ((token!=Token.VAL) && (token!=Token.RPARAN) && (token!=Token.ID)) {
					return Token.UMINUS;
				}
				return Token.SUB;
			case '*':
				return Token.MUL;
			case '/':
				return Token.DIV;
			case '(':
				return Token.LPARAN;
			case ')':
				return Token.RPARAN;
			case '\'':
			case '"':
				strVal=getQuotedString(c);
				return Token.STRING;
			case '^':
				return Token.XOR;
		}

		if ((j=Inputter.getChar())==-1) {
			return Token.EOF;
		}

		d=(char)j;
		switch (c) {
			case '&':
				if (d=='&') {
					return Token.LAND;
				}
				Inputter.putBack(j);
				return Token.AND;
			case '|':
				if (d=='|') {
					return Token.LOR;
				}
				Inputter.putBack(j);
				return Token.OR;
			case '=':
				if (d=='=') {
					return Token.EQ;
				}
				Inputter.putBack(j);
				return Token.EXPAND;
			case '!':
				if (d=='=') {
					return Token.NE;
				}
				Inputter.putBack(d);
				return Token.NOT;
			case '>':
				if (d=='=') {
					return Token.GE;
				}
				Inputter.putBack(d);
				return Token.GT;
			case '<':
				if (d=='=') {
					return Token.LE;
				}
				Inputter.putBack(d);
				return Token.LT;
			case '#':
				if (d=='#') {
					return Token.CAT;
				}
				break;
		}

		Inputter.putBack(j);

		tBuf.delete(0, tBuf.length());
		if (Character.isDigit(c)) {
			while (true) {
				tBuf.append(c);
				if ((i=Inputter.getChar())==-1) {
					break;
				}

				c=(char)i;
				if (!Character.isDigit(c)) {
					Inputter.putBack(i);
					break;
				}
			}

			intVal=Integer.parseInt(tBuf.toString());
			return Token.VAL;
		}

		if ((s=getIdentifier(c))!=null) {
			if (s.compareTo("include")==0) {
				return Token.INCLUDE;
			} else if (s.compareTo("define")==0) {
				return Token.DEFINE;
			} else if (s.compareTo("undef")==0) {
				return Token.UNDEF;
			} else if (s.compareTo("redef")==0) {
				return Token.REDEF;
			} else if (s.compareTo("if")==0) {
				return Token.IF;
			} else if (s.compareTo("elif")==0) {
				return Token.ELIF;
			} else if (s.compareTo("ifdef")==0) {
				return Token.IFDEF;
			} else if (s.compareTo("ifndef")==0) {
				return Token.IFNDEF;
			} else if (s.compareTo("else")==0) {
				return Token.ELSE;
			} else if (s.compareTo("endif")==0) {
				return Token.ENDIF;
			} else if (s.compareTo("echo")==0) {
				return Token.ECHO;
			} else if (s.compareTo("run")==0) {
				return Token.RUN;
			} else if (s.compareTo("end")==0) {
				return Token.END;
			} else if (s.compareTo("rem")==0) {
				return Token.REM;
			}

			if (lookupId) {
				StkVar sv=aapp.vars.get(s);

				if (sv==null) {
					error("Undefined identifier '"+s+"'");
				}

				if (sv.type==StkVar.Type.INT) {
					intVal=sv.intVal;
				} else {
					error("Identifier '"+s+"' is not a number");
				}

				return Token.VAL;
			}

			strVal=s;
			return Token.ID;
		}

		return Token.EOF;
	}

	/**
	 * Get a token in default mode (no lookup on identifers).
	 *
	 * @return token.
	 * @exception Exception.
	 */
	public static Token getToken() throws Exception {
		return getToken(false);
	}

	/**
	 * Get a token.
	 *
	 * @param lookupId look up on identifiers?
	 * @return token.
	 * @exception Exception.
	 */
	public static Token getToken(boolean lookupId) throws Exception {
		skipSpaces();
		token=getNextWord(lookupId);
		return token;
	}
}
