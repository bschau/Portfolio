package com.schau.TrailTools;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.ArrayList;

/**
 * Output coodinate data to (possibly a) file.
 */
public class Emit {
	/**
	 * Number of coordinates written.
	 */
	public static int count = 0;

	/** */
	private static ArrayList<String> al = new ArrayList<String>();
	private static boolean emitToStdOut = false;
	private static FileWriter fw = null;
	private static BufferedWriter bw = null;
	private static PrintWriter pw = null;

	/**
	 * Write this pair.
	 *
	 * @param x X Coordinate.
	 * @param y Y Coordinate.
	 * @exception Exception.
	 */
	public static void emit(int x, int y) throws Exception {
		if (emitToStdOut) {
			System.out.println(x + ", " + y);
		} else {
			al.add(x + " " + y);
		}

		count++;
	}

	/**
	 * Set that output should go to Stdout.
	 */
	public static void setEmitToStdOut() {
		emitToStdOut = true;
	}

	/**
	 * Set that output should go to the file given by the name.
	 *
	 * @param file File to write to.
	 * @exception Exception.
	 */
	public static void setEmitToFile(String file) throws Exception {
		emitToStdOut = false;

		fw = new FileWriter(file);
		bw = new BufferedWriter(fw);
		pw = new PrintWriter(bw);
	}

	/**
	 * Commit output to file.
	 *
	 * @exception Exception.
	 */
	public static void commit() throws Exception {
		if (!emitToStdOut) {
			System.out.println("Committing to coordinates file");
			pw.println(count);

			for (int i = 0; i < count; i++) {
				pw.println(al.get(i));
			}
		}
	}

	/**
	 * Close output stream.
	 */
	public static void close() {
		if (!emitToStdOut) {
			try {
				if (pw != null) {
					pw.close();
				}
			} catch (Exception ex) {}

			try {
				if (bw != null) {
					bw.close();
				}
			} catch (Exception ex) {}

			try {
				if (fw != null) {
					fw.close();
				}
			} catch (Exception ex) {}
		}
	}
}
