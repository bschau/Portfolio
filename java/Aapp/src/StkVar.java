package com.schau;

/**
 * Stack variable.  The public variables (type, intVal and strVal) should
 * really have accessor methods - that may come later ...
 */
public class StkVar {
	/**
	 * A StkVar can be either a integer or a string.
	 */
	public enum Type { INT, STRING };

	/**
	 * Type of StkVar.
	 */
	public Type type;

	/**
	 * If INT, this is the value.
	 */
	public int intVal;

	/**
	 * If STRING, this is the value.
	 */
	public String strVal;

	/**
	 * Create a StkVar of type Integer.
	 *
	 * @param i intVal.
	 */
	public StkVar(int i) {
		intVal=i;
		type=Type.INT;
	}

	/**
	 * Create a StkVar of type String.
	 *
	 * @param s strVal.
	 */
	public StkVar(String s) {
		strVal=s;
		type=Type.STRING;
	}
}
