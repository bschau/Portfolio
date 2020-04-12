package com.schau;

/**
 * Representing a line separator.  This is really an ugly hack, but I don't
 * know how to handle this using traditional system calls.
 * The problem is that getProperty("line.separator") on Windows returns "\r\n"
 * (2 characters) where as on Mac and Linux only one character is returned.
 * The tokenizer _only_ needs one character, the terminating one.
 * However, when generating the output file the line separator should be set
 * to the character(s) which the system expects.
 *
 * This class saves a lot of checks and typing in various places.
 *
 * But I think it is ugly and it should go as soon as possible ...!
 *
 * The publics should be hidden and accessed by accessors .. but hey! I want
 * this class to go as soon as possible so ... :-)
 */
public class LineSeparator {
	/**
	 * Line separator character.
	 */
	public static char ls;

	/**
	 * Full line separator sequence.
	 */
	public static String all;

	/**
	 * Initialize the LineSeparator.
	 */
	public static void init() {
		all=System.getProperty("line.separator");
		int i=all.length()-1;

		ls=all.charAt(i);
	}
}
