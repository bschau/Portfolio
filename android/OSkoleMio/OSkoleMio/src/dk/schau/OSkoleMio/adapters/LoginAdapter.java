package dk.schau.OSkoleMio.adapters;

import java.util.List;

import dk.schau.OSkoleMio.R;
import dk.schau.OSkoleMio.vos.Login;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

public class LoginAdapter extends ArrayAdapter<Login> {
	private List<Login> _logins;
	private Context _context;

	public LoginAdapter(Context context, int textViewResourceId, List<Login> logins) {
		super(context, textViewResourceId, logins);
		_context = context;
		_logins = logins;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		View view = convertView;
		if (view == null) {
			LayoutInflater layoutInflater = (LayoutInflater) _context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
			view = layoutInflater.inflate(R.layout.listitem, null);
		}

		Login login = _logins.get(position);
		if (login != null) {
			((TextView) view.findViewById(R.id.name)).setText(login.getName());
		}

		return view;
	}
}
