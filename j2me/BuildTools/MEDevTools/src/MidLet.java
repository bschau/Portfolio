package dk.schau.MEDevTools;

/**
 * A MidLet (name, entry, icon).
 */
public class MidLet {
	private String name, entry, icon;

	/**
	 * Ho hum, nothing here.
	 */
	public MidLet() {}

	/**
	 * Setters (setName, setEntry, setIcon).
	 * Getters (getName, getEntry, getIcon).
	 */
	public void setName(String name) { this.name = name; }
	public String getName() { return name; }
	public void setEntry(String entry) { this.entry = entry; }
	public String getEntry() { return entry; }
	public void setIcon(String icon) { this.icon = icon; }
	public String getIcon() { return icon; }
}
