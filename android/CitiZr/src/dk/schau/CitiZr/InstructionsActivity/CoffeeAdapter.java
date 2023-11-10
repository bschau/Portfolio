package dk.schau.CitiZr.InstructionsActivity;

import java.util.List;

import dk.schau.CitiZr.R;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

public class CoffeeAdapter extends ArrayAdapter<Coffee> {
	private List<Coffee> coffees;
	private Context context;

	public CoffeeAdapter(Context context, int textViewResourceId, List<Coffee> coffees) {
		super(context, textViewResourceId, coffees);
		this.context = context;
		this.coffees = coffees;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		View v = convertView;
		if (v == null) {
			LayoutInflater vi = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
			v = vi.inflate(R.layout.instructionitem, null);
		}

		Coffee c = coffees.get(position);
		if (c != null) {
			((ImageView) v.findViewById(R.id.image)).setImageResource(c.getImageId());
			((TextView) v.findViewById(R.id.name)).setText(c.getName());
			((ImageView) v.findViewById(R.id.whip)).setImageResource(c.getWhipId());
		}

		return v;
	}
}
