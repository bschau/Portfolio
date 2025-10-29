package dk.schau.OSkoleMio.comparators;

import java.util.Comparator;

import dk.schau.OSkoleMio.Danish;

public class StringComparator implements Comparator<String> {
	@Override
	public int compare(String lhs, String rhs) {
		return Danish.collator.compare(lhs, rhs);
	}
}
