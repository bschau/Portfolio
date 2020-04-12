package com.schau;

import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.lang.StringBuffer;
import java.lang.ProcessBuilder;
import java.util.ArrayList;

/**
 * Handle external commands.
 */
public class ProcessManager {
	/**
	 * Buffer to receive output from external command.
	 */
	private static StringBuilder sBuf=new StringBuilder();

	/**
	 * Arguments list.
	 */
	private static ArrayList<String> aList=new ArrayList<String>();

	/**
	 * Break the command string into "tokens".  Example:
	 *
	 * 	ls -a /home/bsc
	 *
	 * ... is broken into:
	 *
	 * 	"ls"
	 * 	"-a"
	 * 	"/home/bsc"
	 * 
	 * This can be passed directly to the ProcessBuilder.
	 */
	private static void tokenize(String cmd) {
		int cLen=cmd.length(), idx;
		char c;

		aList.clear();

		for (idx=0; idx<cLen; ) {
			c=cmd.charAt(idx);
			if (Character.isSpaceChar(c)) {
				idx++;
			} else {
				sBuf.delete(0, sBuf.length());
				if ((c=='\'') || (c=='"')) {
					char term=c;

					sBuf.append(c);
					idx++;
					while (idx<cLen) {
						c=cmd.charAt(idx);
						sBuf.append(c);
						if (c==term) {
							break;
						}
						idx++;
					}
					sBuf.append(term);
					aList.add(sBuf.toString());
				} else {
					sBuf.append(c);
					idx++;
					while (idx<cLen) {
						c=cmd.charAt(idx);
				       		if (Character.isSpaceChar(c)) {
							break;
						}
						sBuf.append(c);
						idx++;
					}
					aList.add(sBuf.toString());
				}
			}
		}
	}

	/**
	 * Run a command.   The output of the command is save in $_ and the
	 * return code is saved in $$.
	 *
	 * @param aapp aapp.
	 * @param cmd command (and arguments) to run.
	 * @return return code from command.
	 * @exception Exception.
	 */
	public static int run(Aapp aapp, String cmd) throws Exception {
		tokenize(cmd);
		String[] arr=new String[aList.size()];
		aList.toArray(arr);

		ProcessBuilder pb=new ProcessBuilder(arr);
		int rCode=-1;

		pb.redirectErrorStream(true);
		Process p=pb.start();
		InputStream is=p.getInputStream();
		InputStreamReader isr=new InputStreamReader(is);
		BufferedReader br=new BufferedReader(isr);
		String line;

		sBuf.delete(0, sBuf.length());
		while ((line=br.readLine())!=null) {
			sBuf.append(line);
			sBuf.append(LineSeparator.all);
		}

		rCode=p.waitFor();

		aapp.vars.put("$_", new StkVar(getOutput()));
		aapp.vars.put("$$", new StkVar(rCode));
		return rCode;
	}

	/**
	 * Get output from last command.
	 *
	 * @return output.
	 */
	public static String getOutput() {
		return sBuf.toString().trim();
	}
}
