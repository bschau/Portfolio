package com.schau.TrailTools;

import java.util.HashMap;

/**
 * A class to parse options. There exist numerous Options classes on the 'net.
 * However, I wouldn't be a geek if I used one of those :-)
 */
public class Options {
	private HashMap<String, String> hm = new HashMap<String, String>();
	private String[] argv = null;
	private int aIdx = 0;

	/**
	 * Parses the options list. Builds a hashmap with the options and their
	 * values. An option without value will go into the hashmap as (key, 1).
	 *
	 * @param args The arguments array.
	 * @param spec The option specifiers.
	 * @exception Exception.
	 */
	public void Parse(String[] args, String spec) throws Exception {
		int i;
		char c;

		argv = new String[args.length];

		for (int idx = 0; idx < args.length; idx++) {
			if (args[idx].charAt(0) == '-') {
				if (args[idx].length() > 1) {
					c = args[idx].charAt(1);
					if ((i = spec.indexOf(c)) == -1) {
						throw new Exception("Unknown argument: -" + c);
					}

					if (((i + 1) < spec.length()) && (spec.charAt(i + 1) == ':')) {
						idx++;
						if (idx < args.length) {
							hm.put("" + c, args[idx]);
						} else {
							throw new Exception("Option -" + c + " requires an argument");
						}
					} else {
						hm.put("" + c, "1");
					}
				}
			} else {
				argv[aIdx++] = args[idx];
			}
		}
	}

	/**
	 * Get the value to a key.
	 *
	 * @param key Option to get.
	 * @return The value. If the key is not set this function returns null.
	 */
	public String Get(String key) {
		if (hm.containsKey(key)) {
			return (String)hm.get(key);
		}

		return null;
	}

	/**
	 * Get number of "unmatched" options.
	 *
	 * @return Number of unmatched options.
	 */
	public int GetRestArgsCount() {
		return aIdx;
	}

	/**
	 * Get unmatched option at position idx.
	 *
	 * @param idx Index.
	 * @return The option.
	 * @exception Exception if index is out of range.
	 */
	public String GetRestArg(int idx) throws Exception {
		if ((idx < 0) || (idx >= aIdx)) {
			throw new Exception("Argument index out of bounds");
		}

		return argv[idx];
	}
}
