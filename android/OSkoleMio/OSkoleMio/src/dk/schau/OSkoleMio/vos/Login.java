package dk.schau.OSkoleMio.vos;


public class Login {
	private int _id = -1;
	private String _name = "";
	private String _url = "";
	private String _login = "";
	private String _password = "";

	public Login() { }
	
	public Login(int id, String name, String url, String login, String password) {
		_id = id;
		_name = name;
		_url = url;
		_login = login;
		_password = password;
	}

	public Login(String login) {
		String[] parts = login.split("\t");

		for (int index = 0; index < parts.length; index++) {
			String part = parts[index];

			if ((part == null) || (part.trim().length() < 1)) {
				continue;
			}

			switch (parts[index].charAt(0)) {
				case '1':
					_id = Integer.parseInt(parts[index].substring(1));
					break;

				case '2':
					_name = parts[index].substring(1);
					break;

				case '3':
					_url = parts[index].substring(1);
					break;

				case '4':
					_login = parts[index].substring(1);
					break;

				case '5':
					_password = parts[index].substring(1);
					break;
			}
		}
	}

	public int getId() {
		return _id;
	}
	
	public void setId(int id) {
		_id = id;
	}

	public String getName() {
		return _name;
	}
	
	public void setName(String name) {
		_name = name;
	}

	public String getUrl() {
		return _url;
	}
	
	public void setUrl(String url) {
		_url = url;
	}

	public String getLogin() {
		return _login;
	}

	public String getPassword() {
		return _password;
	}

	public String toString() {
		StringBuilder builder = new StringBuilder();
		Boolean pre = false;

		if (_id != -1) {
			builder.append("1");
			builder.append(Integer.toString(_id));
			pre = true;
		}

		pre = addToStringBuilder(pre, "2", builder, _name);
		pre = addToStringBuilder(pre, "3", builder, _url);
		pre = addToStringBuilder(pre, "4", builder, _login);
		pre = addToStringBuilder(pre, "5", builder, _password);
		if (pre) {
			builder.append("\t");
		}

		return builder.toString();
	}

	private Boolean addToStringBuilder(Boolean pre, String index, StringBuilder builder, String value) {
		value = value.trim();

		if (value.length() > 0) {
			if (pre) {
				builder.append("\t");
			}

			builder.append(index);
			builder.append(value);
			pre = true;
		}

		return pre;
	}
}
