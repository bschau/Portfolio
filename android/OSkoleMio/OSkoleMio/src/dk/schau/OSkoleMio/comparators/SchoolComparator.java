package dk.schau.OSkoleMio.comparators;

import java.util.Comparator;

import dk.schau.OSkoleMio.Danish;
import dk.schau.OSkoleMio.vos.School;

public class SchoolComparator implements Comparator<School> {
	@Override
	public int compare(School lhs, School rhs) {
		return Danish.collator.compare(lhs.getName(), rhs.getName());
	}
}
