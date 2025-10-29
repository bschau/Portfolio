import java.io.*;
import java.lang.*;

/**
 * ProcessManager is the class handling external command execution.
 *
 * @author	Brian Schau
 * @since	0.1
 */
public class ProcessManager {
	private StringBuffer sBuf, inBuf, errBuf;

	/**
	 * Inner class representing a 'Stream Eater' - it will read and store
	 * data from a stream.
	 */
	private class StreamEater extends Thread {
		String lineSeparator;
		InputStream iS;
		StringBuffer sBuf;

		StreamEater(InputStream s, StringBuffer b) {
			iS=s;
			sBuf=b;
			if ((lineSeparator=System.getProperty("line.separator"))==null)
				lineSeparator="\n";
		}

		public void run() {
			try {
				InputStreamReader i=new InputStreamReader(iS);
				BufferedReader b=new BufferedReader(i);
				String l;

				while ((l=b.readLine())!=null) {
					sBuf.append(l);
					sBuf.append(lineSeparator);
				}
			} catch (Exception ex) {}
		}
	}

	/**
	 * Run a command, save any output for later retrival.
	 *
	 * @param	cmd	Command (and arguments) to run.
	 * @return -1 in case of error or return code from command (which can
	 * also be -1 ... beware!)
	 */
	public int Run(String cmd) {
		int retCode=-1;
		Process p;

		sBuf=new StringBuffer();
		inBuf=new StringBuffer();
		errBuf=new StringBuffer();
		try {
			p=Runtime.getRuntime().exec(cmd);

			StreamEater i=new StreamEater(p.getInputStream(), inBuf);
			StreamEater e=new StreamEater(p.getErrorStream(), errBuf);
			i.start();
			e.start();
			retCode=p.waitFor();

			if (inBuf.length()>0)
				sBuf.append(inBuf.toString());

			if (errBuf.length()>0)
				sBuf.append(errBuf.toString());

		} catch (Exception ex) {
			sBuf.setLength(0);
			sBuf.append(ex.getMessage());
		}

		return retCode;
	}

	/**
	 * Get any messages from the previous command.
	 *
	 * @return	Messages.
	 */
	public String Messages() {
		return sBuf.toString().trim();
	}

	/**
	 * A wrapper for Run.	Executes the command and writes any output.
	 * If a -1 return code is detected, exits j2mec!
	 *
	 * @param	cmd	Command (and arguments) to execute.
	 */
	public void RunCommand(String cmd) {
		int retCode=Run(cmd);

		String s=Messages();

		if (s.length()>0)
			System.out.println(s);

		if (retCode!=0)
			System.exit(1);
	}
}
