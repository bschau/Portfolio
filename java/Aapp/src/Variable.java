package com.schau;

/**
 * A command line variable - used by ant to feed in the "command-line" variables
 * (in build.xml) to Aapp.
 */
public class Variable {
	/**
	 * Name of variable.
	 */
	private String name;

	/**
	 * Value of variable.
	 */
	private String value;

	/**
	 * Ho hum, nothing here.
	 */
	public Variable() {}

	/**
	 * Set the name of the variable.
	 *
	 * @param name name.
	 */
	public void setName(String name) {
		this.name=name;
	}

	/**
	 * Get name of variable.
	 *
	 * @return name.
	 */
	public String getName() {
		return name;
	}

	/**
	 * Set the value of the variable.
	 *
	 * @param value value.
	 */
	public void addText(String value) {
		this.value=value;
	}

	/**
	 * Get value of variable.
	 *
	 * @return value.
	 */
	public String getValue() {
		return value;
	}
}
