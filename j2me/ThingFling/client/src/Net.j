	/**
	 * URL encode a string (' ' to '+', %XX etc).
	 *
	 * @param s Source string.
	 * @return encoded string.
	 */
	private String urlEncode(String s) {
		StringBuffer sBuf=new StringBuffer();
		char c;
		int b, i;

		for (i=0; i<s.length(); i++) {
			c=s.charAt(i);

			if (c==' ') {
				sBuf.append('+');
			} else if (((c>='0') && (c<='9')) ||
				   ((c>='A') && (c<='Z')) ||
				   ((c>='a') && (c<='z')) ||
				   ((c=='.') || (c=='*') ||
				    (c=='-') || (c=='_'))) {
				sBuf.append(c);
			} else {
				b=((int)c)&0xff;
				sBuf.append('%');
				sBuf.append(hexDigits.charAt((b>>4)&0x0f));
				sBuf.append(hexDigits.charAt(b&0x0f));
			}
		}

		return sBuf.toString();
	}

	/**
	 * Initialize HTTP request. Setup uid and pin variables.
	 */
	private void httpInit() {
		httpArgs.delete(0, httpArgs.length());
		httpArgs.append("uid=");
		httpArgs.append(urlEncode(uid));
		httpArgs.append("&pin=");
		httpArgs.append(urlEncode(pin));
	}

	/**
	 * Add key=value to the POST request.
	 *
	 * @param key Key to add.
	 * @param value Value of key.
	 */
	private void httpAddArg(String key, String value) {
		httpArgs.append('&');
		httpArgs.append(key);
		httpArgs.append('=');
		httpArgs.append(urlEncode(value));
	}

	/**
	 * Connect to remote host.
	 *
	 * @exception Exception.
	 */
	private void httpConnect() throws Exception {
		StringBuffer s=new StringBuffer();

		s.append(getJADProperty("Flinger"));
		s.append(httpArgs.toString());

		http=(HttpConnection)Connector.open(getJADProperty("Flinger"), Connector.READ_WRITE);
		http.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
		http.setRequestMethod(HttpConnection.POST);
	}

	/**
	 * Close the connection.
	 */
	private void httpClose() {
		if (http!=null) {
			try {
				http.close();
			} catch (Exception e) {}

			http=null;
		}
		System.gc();
	}

	/**
	 * Send query/post data to remote host.
	 *
	 * @exception Exception.
	 */
	private void httpSend() throws Exception {
		DataOutputStream dos=null;
		String tmp=httpArgs.toString();
		String post=new String(tmp.getBytes(), "UTF-8");
		byte[] req=post.getBytes();

		tmp=null;
		try {
			dos=(DataOutputStream)http.openDataOutputStream();

			for (int i=0; i<req.length; i++) {
				dos.writeByte(req[i]);
			}

			req=null;
		} catch (Exception e) {
			popup("#=LPopupNetErr", e.getMessage());
		} finally {
			dos.flush();
			dos.close();
			req=null;
			post=null;
			System.gc();
		}
	}

	/**
	 * Get response from remote host.   Response will be available in
	 * the httpResp buffer.
	 *
	 * @exception Exception.
	 */
	private void httpGetResponse() throws Exception {
		InputStream is=null;

		httpResp.delete(0, httpResp.length());
		try {
			is=http.openInputStream();
			int i;

			while ((i=is.read())!=-1) {
				httpResp.append((char)i);
			}
		} catch (Exception e) {
			popup("#=LPopupNetErr", e.getMessage());
		} finally {
			is.close();
		}
	}
