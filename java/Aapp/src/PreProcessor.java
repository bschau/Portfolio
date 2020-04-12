package com.schau;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.lang.Character;
import java.lang.StringBuffer;

/**
 * Pre-processor engine.
 */
public class PreProcessor {
	/**
	 * Destination file content.
	 */
	private static StringBuffer dstBuf=new StringBuffer();

	/**
	 * Pre-processor token identification.
	 */
	private static String ppid;

	/**
	 * Length of pre-processor token identification.
	 */
	private static int ppidLen;

	/**
	 * Aapp.
	 */
	private static Aapp aapp;

	/**
	 * Initialize the pre-processor.
	 * 
	 * @param aapp aapp.
	 * @param s pre-processor token identification.
	 */
	public static void init(Aapp a, String s) {
		ppid=s;
		ppidLen=ppid.length();
		aapp=a;
		LineSeparator.init();
		Expression.init(a);
		Tokenizer.init(a);
	}

	/**
	 * Process a file.
	 *
	 * @param dst destination directory.
	 * @param srcFile source file.
	 * @exception Exception.
	 */
	public static void process(File dst, File srcFile) throws Exception {
		Inputter.addFile(srcFile);

		dstBuf.delete(0, dstBuf.length());
		preProcess();

		String n=srcFile.getName();
		dst=new File(dst, n);

		writeToFile(dst);
	}

	/**
	 * Write the processed content to the file.
	 *
	 * @param dst full path to the destination file.
	 * @exception Exception.
	 */
	private static void writeToFile(File dst) throws Exception {
		BufferedWriter w=null;

		try {
			w=new BufferedWriter(new FileWriter(dst));
			w.write(dstBuf.toString(), 0, dstBuf.length());
		} catch (Exception ex) {
			Tokenizer.error("Failed to write destination file '"+dst.getName()+"'");
		} finally {
			if (w!=null) {
				try {
					w.close();
				} catch (Exception ex) {}
			}
		}
	}

	/**
	 * Pre-processor main loop.
	 *
	 * @exception Exception.
	 */
	private static void preProcess() throws Exception {
		int ppidIdx=0, c;

		while ((c=Inputter.getChar())!=-1) {
			if ((char)c==LineSeparator.ls) {
				dstBuf.append(LineSeparator.all);
				Inputter.bumpLine();
				ppidIdx=0;
			} else if (c==ppid.charAt(ppidIdx)) {
				ppidIdx++;
				if (ppidIdx==ppidLen) {
					handleCmd();
					ppidIdx=0;
				}
			} else {
				if (ppidIdx>0) {
					dstBuf.append(ppid.substring(0, ppidIdx));
					ppidIdx=0;
				}

				dstBuf.append((char)c);
			}
		}
	}

	/**
	 * Handle a token command.
	 *
	 * @exception Exception.
	 */
	private static void handleCmd() throws Exception {
		Tokenizer.reset();

		switch (Tokenizer.getToken()) {
			case INCLUDE:
				CmdFiles.include(aapp);
				break;
			case DEFINE:
				CmdVariables.define(aapp);
				break;
			case REDEF:
				CmdVariables.redef(aapp);
				break;
			case UNDEF:
				CmdVariables.undef(aapp);
				break;
			case EXPAND:
				CmdVariables.expand(aapp, dstBuf);
				break;
			case IF:
				CmdConditionals.hvis(aapp, ppid);
				break;
			case ELIF:
				CmdConditionals.skipBranch(aapp, ppid, true);
				break;
			case IFDEF:
				CmdConditionals.ifdef(aapp, ppid);
				break;
			case IFNDEF:
				CmdConditionals.ifndef(aapp, ppid);
				break;
			case ELSE:
				CmdConditionals.skipBranch(aapp, ppid, false);
				break;
			case ECHO:
				CmdMisc.echo(aapp);
				break;
			case RUN:
				CmdMisc.run(aapp);
				break;
			case END:
				CmdMisc.end(aapp);
				break;
			case REM:
				CmdMisc.rem(aapp);
				break;
		}
	}
}
