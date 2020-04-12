package com.schau;

import java.io.File;
import java.lang.StringBuffer;
import java.util.ArrayList;
import java.util.Iterator;

/**
 * Handle file tokens (include).
 */
public class CmdFiles {
	/**
	 * Locate an include file. Search order:
	 *
	 *   (1) Current parent directory + inc
	 *   (2) Each of the components in suffixes + inc
	 *
	 * @param aapp aapp.
	 * @param inc include file.
	 * @return full path to file.
	 */
	private static String locate(Aapp aapp, String inc) {
		String p=aapp.curParent+File.separator+inc;
		File f=new File(p);

		if (f.exists()) {
			return p;
		}

		for (Iterator it=aapp.includes.iterator(); it.hasNext(); ) {
			p=((String)it.next())+File.separator+inc;
			f=new File(p);
			if (f.exists()) {
				return p;
			}
		}

		return null;
	}

	/**
	 * Handle include token.
	 *
	 * @param aapp aapp.
	 * @exception Exception.
	 */
	public static void include(Aapp aapp) throws Exception {
		Tokenizer.skipSpaces();
		Tokenizer.reset();

		Token t=Tokenizer.getToken();
		String file=null;

		if (t==Token.STRING) {
			file=Tokenizer.strVal;
		} else if (t==Token.ID) {
			StkVar s=CmdVariables.getStkVar(aapp, Tokenizer.strVal);

			file=s.strVal;
		}

		if ((file==null) || (file.length()==0)) {
			Tokenizer.error("Missing filename");
		}

		String f=locate(aapp, file);
		if (f==null) {
			Tokenizer.error("Cannot find include file '"+file+"'");
		}

		Inputter.addFile(new File(f));
	}
}
