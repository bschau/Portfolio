package com.schau;

/**
 * A Key/String combo.
 */
public class KeyString {
	/**
	 * Key name.
	 */
	private String key="";

	/**
	 * String.
	 */
	private String string="";

	/**
	 * Constructor.
	 */
	public KeyString() {}

	/**
	 * Setters (setKey, addText).
	 */
	public void setKey(String key) { this.key=key; }
	public void addText(String string) { this.string=string; }

	/**
	 * Getters (getKey, getString).
	 */
	public String getKey() { return key; }
	public String getString() { return string; }
}
