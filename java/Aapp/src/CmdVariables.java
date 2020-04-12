package com.schau;

/**
 * Handle variable manipulation tokens (define, redef, undef) and expansion
 * tokens (=).
 */
public class CmdVariables {
	/**
	 * Check to see if the variable referenced is in local scope.
	 *
	 * @param v variable.
	 * @param opr operation (define/redef/undef).
	 * @exception Exception.
	 */
	private static void checkScope(String v, String opr) throws Exception {
		int i;

		if ((i=v.indexOf(':'))>-1) {
			Scope.setScope(v.substring(0, i));

			if (!Scope.isLocal()) {
				Tokenizer.error("Attempt to "+opr+" '"+v.substring(i+1)+"' outside local scope");
			}
		}
	}

	/**
	 * Define/redefine a variable.
	 *
	 * @param aapp aapp.
	 * @param defOnce set to true to define, false to redefine.
	 * @param op operation.
	 * @exception Exception.
	 */
	private static void varDefine(Aapp aapp, boolean defOnce, String op) throws Exception {
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

		checkScope(var, "define");

		int i;
		if ((i=var.indexOf(':'))>-1) {
			var=var.substring(i+1);
		}

		if (defOnce) {
			if ((aapp.vars.get(var)!=null) ||
			    (System.getProperty(var)!=null) ||
			    (System.getenv(var)!=null)) {
				Tokenizer.error("Cannot redefine "+var);
			}
		}

		Expression.parse();

		StkVar v=Expression.getStkVar();
		if (v==null) {
			Tokenizer.error("No value to variable '"+var+"'");
		}

		aapp.vars.put(var, v);
	}

	/**
	 * Handle the define token.
	 *
	 * @param aapp aapp.
	 * @exception Exception.
	 */
	public static void define(Aapp a) throws Exception {
		varDefine(a, true, "define");
	}

	/**
	 * Handle the redef token.
	 *
	 * @param aapp aapp.
	 * @exception Exception.
	 */
	public static void redef(Aapp a) throws Exception {
		varDefine(a, false, "redef");
	}

	/**
	 * Handle the undef token.
	 *
	 * @param aapp aapp.
	 * @exception Exception.
	 */
	public static void undef(Aapp aapp) throws Exception {
		Tokenizer.skipSpaces();
		Tokenizer.reset();
		Token t=Tokenizer.getToken();
		String var=null;

		if (t==Token.ID) {
			var=Tokenizer.strVal;
		}

		if ((var==null) || (var.length()==0)) {
			Tokenizer.error("No variable name to undefine");
		}

		checkScope(var, "undef");

		aapp.vars.remove(var);
	}

	/**
	 * Get the StkVar value for the named variable.
	 *
	 * @param aapp aapp.
	 * @param var variable.
	 * @return StkVar.
	 * @exception Exception.
	 */
	public static StkVar getStkVar(Aapp aapp, String var) throws Exception {
		StkVar val=null;
		int i;

		if ((i=var.indexOf(':'))>-1) {
			String s=var.substring(0, i);

			Scope.setScope(s);
			String name=var.substring(i+1);

			if (Scope.isLocal()) {
				val=aapp.vars.get(name);
			} else if (Scope.isAnt()) {
				if ((s=aapp.getAntVar(name))!=null) {
					val=new StkVar(s);
				}
			} else if (Scope.isRT()) {
				if ((s=System.getProperty(name))==null) {
					val=null;
				} else {
					val=new StkVar(s);
				}
			} else if (Scope.isEnv()) {
				if ((s=System.getenv(name))==null) {
					val=null;
				} else {
					val=new StkVar(s);
				}
			} else {
				Tokenizer.error("Scope '"+s+"' of variable '"+name+"' unknown");
			}
		} else {
			if ((val=aapp.vars.get(var))==null) {
				String s;

				if ((s=aapp.getAntVar(var))==null) {
					if ((s=System.getProperty(var))==null) {
						if ((s=System.getenv(var))!=null) {
							val=new StkVar(s);
						}
					} else {
						val=new StkVar(s);
					}
				} else {
					val=new StkVar(s);
				}
			}
		}

		if (val==null) {
			Tokenizer.error("Variable '"+var+"' not found");
		}

		return val;
	}

	/**
	 * Expand the content of a variable into the string buffer.
	 *
	 * @param aapp aapp.
	 * @param dBuf string buffer to receive variable content.
	 * @exception Exception.
	 */
	public static void expand(Aapp aapp, StringBuffer dBuf) throws Exception {
		Tokenizer.skipSpaces();
		Tokenizer.reset();
		Token t=Tokenizer.getToken();
		String var=null;

		if (t==Token.ID) {
			var=Tokenizer.strVal;
		} else if (t==Token.LPARAN) {
			if ((t=Tokenizer.getToken())==Token.ID) {
				var=Tokenizer.strVal;
			}

			if ((t=Tokenizer.getToken())!=Token.RPARAN) {
				Tokenizer.error("Invalid expand sequence");
			}
		} else if (t==Token.VAL) {
			dBuf.append(Integer.toString(Tokenizer.intVal));
			return;
		}

		if ((var==null) || (var.length()==0)) {
			Tokenizer.error("No variable name to expand");
		}

		StkVar sv=getStkVar(aapp, var);
		if (sv.type==StkVar.Type.INT) {
			dBuf.append(Integer.toString(sv.intVal));
		} else {
			Inputter.addFile(sv.strVal, "Expression on line "+Inputter.getSrcLine()+" in "+Inputter.getSrcFile());
		}
	}
}
