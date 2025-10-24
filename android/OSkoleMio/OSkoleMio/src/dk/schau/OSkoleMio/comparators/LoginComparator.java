package dk.schau.OSkoleMio.comparators;

import java.util.Comparator;

import dk.schau.OSkoleMio.Danish;
import dk.schau.OSkoleMio.vos.Login;

public class LoginComparator implements Comparator<Login> {
	@Override
	public int compare(Login lhs, Login rhs) {
		return Danish.collator.compare(lhs.getName(), rhs.getName());
	}
}
