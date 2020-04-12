package com.schau;

/**
 * Scope ... a scope in Aapp can be of:
 *
 * local:	- inner scope, variables in the local scope can be set, deleted
 * 		  and changed.
 * ant:		- properties defined in Ant build files.  ant: variables are
 * 		  read only.
 * rt:		- the java runtime (such as os.name etc).  rt: variables are
 * 		  read only.
 * env:		- environment variables (such as $PATH/%PATH%).  env: variables
 * 		  are read only.
 *
 * If you try to redefine a ant:, rt: or env: variable it will be set in the
 * local: scope.  So, redefining f.ex. os.name will create a copy of os.name
 * in the local: scope so you will be able to:
 *
 * $=local:os.name
 * $=rt:os.name
 *
 * ... and get two different answers ...
 */
public class Scope {
	/**
	 * Scope.
	 */
	private static String scope;

	/**
	 * Set scope.
	 *
	 * @param s scope.
	 * @exception Exception.
	 */
	public static void setScope(String s) throws Exception {
		scope=s.toLowerCase();
		if ((isLocal()) || (isRT()) || (isEnv()) || (isAnt())) {
			return;
		}

		Tokenizer.error("Unknown variable scope '"+s+"'");
	}

	/**
	 * Is this a local scope?
	 *
	 * @return true if local, false otherwise.
	 */
	public static boolean isLocal() {
		return ((scope.compareTo("local")==0) ? true : false);
	}

	/**
	 * Is this a runtime scope?
	 *
	 * @return true if runtime, false otherwise.
	 */
	public static boolean isRT() {
		return ((scope.compareTo("rt")==0) ? true : false);
	}

	/**
	 * Is this a environment scope?
	 *
	 * @return true if environment, false otherwise.
	 */
	public static boolean isEnv() {
		return ((scope.compareTo("env")==0) ? true : false);
	}

	/**
	 * Is this a Ant property?
	 *
	 * @return true if Ant property, false otherwise.
	 */
	public static boolean isAnt() {
		return ((scope.compareTo("ant")==0) ? true : false);
	}
}
