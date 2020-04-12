package com.schau;

/**
 * Handle miscellanous tokens (echo, run).
 */
public class CmdMisc {
	/**
	 * Handle echo token.
	 *
	 * @param aapp aapp.
	 * @exception Exception.
	 */
	public static void echo(Aapp aapp) throws Exception {
		Tokenizer.skipSpaces();
		Tokenizer.reset();

		Expression.parse();
		StkVar sv=Expression.getStkVar();
		if (sv.type==StkVar.Type.STRING) {
			aapp.syslog(sv.strVal);
		} else {
			aapp.syslog(Integer.toString(sv.intVal));
		}
	}

	/**
	 * Handle run token.
	 *
	 * @param aapp aapp.
	 * @exception Exception.
	 */
	public static void run(Aapp aapp) throws Exception {
		Tokenizer.skipSpaces();
		Tokenizer.reset();

		Expression.parse();
		StkVar sv=Expression.getStkVar();
		if (sv.type==StkVar.Type.INT) {
			Tokenizer.error("run - missing command");
		}

		int r=ProcessManager.run(aapp, sv.strVal);

		if (r!=0) {
			aapp.syslog(ProcessManager.getOutput());
		}
	}

	/**
	 * Handle end token.
	 *
	 * @param aapp aapp.
	 * @exception Exception.
	 */
	public static void end(Aapp aapp) throws Exception {
		aapp.syslog("END in "+Inputter.getSrcFile()+" on line "+Inputter.getSrcLine());
		Inputter.remFile();
	}

	/**
	 * Handle rem token.
	 *
	 * @param aapp aapp.
	 * @exception Exception.
	 */
	public static void rem(Aapp app) throws Exception {
		int c;

		while ((c=Inputter.getChar())!=-1) {
			if ((char)c==LineSeparator.ls) {
				Inputter.bumpLine();
				return;
			}
		}
	}
}
