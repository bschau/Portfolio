package dk.schau.CitiZr.CitiZrActivity;

import java.util.List;

import dk.schau.CitiZr.R;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

public class CapsuleAdapter extends ArrayAdapter<Capsule> {
	private List<Capsule> capsules;
	private Context context;

	public CapsuleAdapter(Context context, int textViewResourceId, List<Capsule> capsules) {
		super(context, textViewResourceId, capsules);
		this.context = context;
		this.capsules = capsules;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		View v = convertView;
		if (v == null) {
			LayoutInflater vi = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
			v = vi.inflate(R.layout.listitem, null);
		}

		Capsule c = capsules.get(position);
		if (c != null) {
			((ImageView) v.findViewById(R.id.image)).setImageResource(c.getImageId());
			((ImageView) v.findViewById(R.id.strength)).setImageResource(c.getStrengthId());
			((TextView) v.findViewById(R.id.name)).setText(c.getName());
			((TextView) v.findViewById(R.id.teaser)).setText(c.getTeaser());
		}

		return v;
	}
}
