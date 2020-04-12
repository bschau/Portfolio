package com.schau;

import java.lang.StringBuffer;

/**
 * Handle conditional tokens (if, elif, ifdef, ifndef).
 */
public class CmdConditionals {
	/**
	 * Skip the current branch and find a matching else/endif token.
	 *
	 * @param aapp aapp.
	 * @param ppid start of token id.
	 * @param handleElse true if else can terminate this block, false
	 *                   otherwise.
	 * @exception Exception.
	 */
	public static void skipBranch(Aapp aapp, String ppid, boolean handleElse) throws Exception {
		int ppidLen=ppid.length(), ppidIdx=0, depth=1, c;

		while (depth>0) {
			if ((c=Inputter.getChar())==-1) {
				Tokenizer.error("Sudden EOF in if/ifdef/ifndef block");
			}

			if ((char)c==LineSeparator.ls) {
				Inputter.bumpLine();
				ppidIdx=0;
			} else if (c==ppid.charAt(ppidIdx)) {
				ppidIdx++;
				if (ppidIdx==ppidLen) {
					Tokenizer.reset();
	
					Token t=Tokenizer.getToken();

					if ((handleElse) && (t==Token.ELSE)) {
						if (depth==1) {
							depth=0;
						}
					} else {
						switch (t) {
							case EOF:
								Tokenizer.error("Sudden EOF in if/ifdef/ifndef block");
							case ELIF:
								if (depth==1) {
									if (hvis(aapp, ppid, false)) {
										depth=0;
									}
									break;
								}
							case IF:
							case IFDEF:
							case IFNDEF:
								depth++;
								break;
							case ENDIF:
								depth--;
								break;
							case END:
								Tokenizer.error("Sudden END in if/ifdef/ifndef block");
						}

					}

					ppidIdx=0;
				}
			} else {
				ppidIdx=0;
			}
		}
	}
                 
	/**
	 * Handle if token.
	 *
	 * @param aapp aapp.
	 * @param ppid start of token id.
	 * @return true if expression is true, false otherwise.
	 * @exception Exception.
	 */
	public static boolean hvis(Aapp aapp, String ppid) throws Exception {
		return hvis(aapp, ppid, true);
	}

	/**
	 * Handle if token.
	 *
	 * @param aapp aapp.
	 * @param ppid start of token id.
	 * @param skipB if true, invoke skipBranch if expression evaluates to
	 *              false.
	 * @return true if expression is true, false otherwise.
	 * @exception Exception.
	 */
	public static boolean hvis(Aapp aapp, String ppid, boolean skipB) throws Exception {
		Tokenizer.skipSpaces();
		Tokenizer.reset();

		Expression.parse();

		StkVar sv=Expression.getStkVar();
		if (sv.type==StkVar.Type.STRING) {
			Tokenizer.error("Expression must be an integer");
		}

		if (sv.intVal==0) {
			if (skipB) {
				skipBranch(aapp, ppid, true);
			}
			return false;
		}
		return true;
	}

	/**
	 * Get variable name.
	 *
	 * @param op operation (ifdef/ifndef).
	 * @exception Exception.
	 */
	private static String getVar(String op) throws Exception {
		Tokenizer.skipSpaces();
		Tokenizer.reset();
		Token t=Tokenizer.getToken();
		String var=null;

		if (t==Token.ID) {
			var=Tokenizer.strVal;
		}

		if ((var==null) || (var.length()==0)) {
			Tokenizer.error("No variable name given to "+op);
		}

		return var;
	}

	/**
	 * See if the variable is defined.
	 *
	 * @param aapp aapp.
	 * @param var variable.
	 * @return true if variable is defined in any scope, false otherwise.
	 * @exception Exception.
	 */
	private static boolean isDefined(Aapp aapp, String var) throws Exception {
		int i;

		if ((i=var.indexOf(':'))>-1) {
			String s=var.substring(0, i);
			Scope.setScope(s);
			String name=var.substring(i+1);

			if (Scope.isLocal()) {
				if (aapp.vars.containsKey(name)) {
					return true;
				}
			} else if (Scope.isAnt()) {
				if (aapp.getAntVar(name)!=null) {
					return true;
				}
			} else if (Scope.isRT()) {
				if (System.getProperty(name)!=null) {
					return true;
				}
			} else if (Scope.isEnv()) {
				if (System.getenv(name)!=null) {
					return true;
				}
			} else {
				Tokenizer.error("Scope '"+s+"' of variable '"+name+"' unknown");
			}
		} else {
			if (aapp.vars.containsKey(var)) {
				return true;
			}

			if (aapp.getAntVar(var)!=null) {
				return true;
			}

			if (System.getProperty(var)!=null) {
				return true;
			}

			if (System.getenv(var)!=null) {
				return true;
			}
		}

		return false;
	}

	/**
	 * Handle ifdef token.
	 *
	 * @param aapp aapp.
	 * @param ppid start of token id.
	 * @exception Exception.
	 */
	public static void ifdef(Aapp aapp, String ppid) throws Exception {
		String var=getVar("ifdef");

		if (isDefined(aapp, var)) {
			return;
		}

		skipBranch(aapp, ppid, true);
	}

	/**
	 * Handle ifndef token.
	 *
	 * @param aapp aapp.
	 * @param ppid start of token id.
	 * @exception Exception.
	 */
	public static void ifndef(Aapp aapp, String ppid) throws Exception {
		String var=getVar("ifndef");

		if (isDefined(aapp, var)==false) {
			return;
		}

		skipBranch(aapp, ppid, true);
	}
}
