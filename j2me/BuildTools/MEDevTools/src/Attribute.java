package dk.schau.MEDevTools;

/**
 * An attribute (name, value).
 */
public class Attribute {
	private String name, value;

	/**
	 * Ho hum, nothing here.
	 */
	public Attribute() {}

	/**
	 * Setters (setName, setValue) and Getters (getName, getValue).
	 */
	public void setName(String name) { this.name = name; }
	public String getName() { return name; }
	public void setValue(String value) { this.value = value; }
	public String getValue() { return value; }
}
