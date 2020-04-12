package com.schau;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.File;
import java.lang.Character;
import java.util.Stack;
import java.util.Vector;

/**
 * Wrapper for all input functions.
 */
public class Inputter {
	/**
	 * Stack of files. Topmost file is the current file.
	 */
	private static Stack<SrcFile> fileStack=new Stack<SrcFile>();

	/**
	 * Current file.
	 */
	private static SrcFile top;

	/**
	 * Add and open a file to the file stack.
	 *
	 * @param f file.
	 * @exception Exception.
	 */
	public static void addFile(File f) throws Exception {
		top=new SrcFile();

		top.open(f);
		fileStack.push(top);
	}

	/**
	 * Add and open a string to the file stack.
	 *
	 * @param s string.
	 * @param t token name.
	 * @exception Exception.
	 */
	public static void addFile(String s, String t) throws Exception {
		top=new SrcFile();

		top.open(s, t);
		fileStack.push(top);
	}

	/**
	 * Remove and close the top file.
	 *
	 * @exception Exception.
	 */
	public static void remFile() throws Exception {
		SrcFile s=fileStack.pop();

		s.close();

		if (fileStack.empty()) {
			top=null;
		} else {
			top=fileStack.peek();
		}
	}

	/**
	 * Get a character from the current file.
	 *
	 * @return character or -1 on EOF.
	 * @exception Exception.
	 */
	public static int getChar() throws Exception {
		int c;

		if (top==null) {
			return -1;
		}

		if ((c=top.getChar())==-1) {
			remFile();
			return getChar();
		}

		return c;
	}

	/**
	 * Put back a character to the top file.
	 *
	 * @exception Exception.
	 */
	public static void putBack(int c) throws Exception {
		if (top==null) {
			throw new Exception("Corrupt file list");
		}

		top.putBack(c);
	}

	/**
	 * Get current file.
	 *
	 * @return current file.
	 */
	public static SrcFile getTop() {
		return top;
	}

	/**
	 * Return a copy of the file stack (for tracing purposes).
	 *
	 * @return file stack.
	 */
	public static Stack cloneStack() {
		return (Stack)(((Vector)fileStack).clone());
	}

	/**
	 * Get name of current file.
	 *
	 * @return file name.
	 */
	public static String getSrcFile() {
		return top.getName();
	}

	/**
	 * Get current line number.
	 *
	 * @return line number.
	 */
	public static int getSrcLine() {
		return top.getLine();
	}

	/**
	 * Increase current files line number.
	 */
	public static void bumpLine() {
		top.bumpLine();
	}
}
