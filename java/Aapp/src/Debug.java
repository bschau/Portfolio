package com.schau;

import java.util.Iterator;
import java.util.Set;
import java.util.Stack;

/**
 * Debugging routines.
 */
public class Debug {
	/**
	 * Dump the variables table to stdout.
	 *
	 * @param aapp aapp.
	 */
	public static void dumpVars(Aapp aapp) {
		Set allKeys=aapp.vars.keySet();
		String key;
		StkVar sv;

		System.out.println("Defined variables:");
		for (Iterator it=allKeys.iterator(); it.hasNext(); ) {
			key=(String)it.next();
			sv=aapp.vars.get(key);

			if (sv.type==StkVar.Type.INT) {
				System.out.println(key+" = "+Integer.toString(sv.intVal)+" (INT)");
			} else {
				System.out.println(key+" = "+sv.strVal+" (STRING)");
			}
		}
	}

	/**
	 * Dump valStk to stdout.
	 *
	 * @param stk valStk.
	 */
	public static void dumpValStk(Stack<StkVar> stk) {
		System.out.println("valStk trace:");

		while (!stk.empty()) {
			StkVar v=stk.pop();
			if (v.type==StkVar.Type.INT) {
				System.out.println(v+" = "+Integer.toString(v.intVal)+" (INT)");
			} else {
				System.out.println(v+" = "+v.strVal+" (STRING)");
			}
		}
	}
}
