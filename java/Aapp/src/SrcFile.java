package com.schau;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.File;
import java.io.StringReader;

/**
 * This is how a source file looks like.   Or, a file string.
 */
public class SrcFile {
	/**
	 * Current line.
	 */
	private int line=1;
       
	/**
	 * Last read character.
	 */
	private int chr=-1;

	/**
	 * Reader.
	 */
	private BufferedReader rdr;

	/**
	 * Name of file/token.
	 */
	private String name;

	/**
	 * Open a file and create a reader for it.
	 *
	 * @param f file.
	 * @exception Exception.
	 */
	public void open(File f) throws Exception {
		name=f.getAbsolutePath();
		rdr=new BufferedReader(new FileReader(f));
	}

	/**
	 * "Open" a string and create a reader for it.
	 *
	 * @param s string.
	 * @param t token.
	 * @exception Exception.
	 */
	public void open(String s, String t) throws Exception {
		name="Indirect from "+t;
		rdr=new BufferedReader(new StringReader(s));
	}

	/**
	 * Close the stream.
	 */
	public void close() {
		if (rdr!=null) {
			try {
				rdr.close();
			} catch (Exception ex) {}

			rdr=null;
		}
	}

	/**
	 * Get the next character from the stream.
	 *
	 * @return character (or -1 on EOF).
	 * @exception Exception.
	 */
	public int getChar() throws Exception {
		int c;

		if (chr!=-1) {
			c=chr;
			chr=-1;
		} else {
			c=rdr.read();
		}

		return c;
	}

	/**
	 * Put back a character.
	 */
	public void putBack(int c) {
		chr=c;
	}

	/**
	 * Get name of file/token.
	 *
	 * @return name.
	 */
	public String getName() {
		return name;
	}

	/**
	 * Get current line number.
	 *
	 * @return line number.
	 */
	public int getLine() {
		return line;
	}

	/**
	 * Increment line number.
	 */
	public void bumpLine() {
		line++;
	}
}
