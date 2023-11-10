package dk.schau.OSkoleMio.adapters;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Set;

import dk.schau.OSkoleMio.comparators.StringComparator;

import android.content.Context;
import android.widget.ArrayAdapter;
import android.widget.SectionIndexer;

public class SchoolPickerIndexerAdapter<T> extends ArrayAdapter<T> implements SectionIndexer {
	HashMap<String, Integer> _alphaIndexer;
	String[] _sections;

	public SchoolPickerIndexerAdapter(Context context, List<T> elements) {
		super(context, android.R.layout.simple_list_item_1, elements);

		_alphaIndexer = new HashMap<String, Integer>();
		for (int index = 0; index < elements.size(); index++) {
			String key = ((String) elements.get(index)).substring(0, 1);

			if (_alphaIndexer.containsKey(key) == false) {
				_alphaIndexer.put(key, index);
			}
		}

		Set<String> sectionLetters = _alphaIndexer.keySet();
		ArrayList<String> sectionList = new ArrayList<String>(sectionLetters);

		Collections.sort(sectionList, new StringComparator());
		_sections = new String[sectionList.size()];
		sectionList.toArray(_sections);
	}

	public int getPositionForSection(int section) {
		return _alphaIndexer.get(_sections[section]);
	}

	public int getSectionForPosition(int position) {
		return 1;
	}

	public Object[] getSections() {
		return _sections;
	}
}
