	/**
	 * Load preferences.
	 *
	 * @exception Exception.
	 */
	private void loadPreferences() throws Exception {
		byte[] b;

		if (dbPrf.getNumRecords()==0) {
			b=formPreferences();
			dbPrf.addRecord(b, 0, b.length);
		} else {
			ByteArrayInputStream bais=null;
			DataInputStream dis=null;

			try {
				byte[] buf;
				byte x;
				short len;
				int bLen;

				b=dbPrf.getRecord(#=DBRPrefs);
				bLen=b.length;

				bais=new ByteArrayInputStream(b);
				dis=new DataInputStream(bais);

				while (bLen>0) {
					x=dis.readByte();
					bLen--;

					switch (x) {
						case #=(PrfPin):
							if (bLen>0) {
								pin=prfGetString(dis, bLen);
								bLen-=prfReadLen;
							}
							break;
						case #=(PrfUID):
							if (bLen>0) {
								uid=prfGetString(dis, bLen);
								bLen-=prfReadLen;
							}
							break;
						default:	// corrupted
							dbPrf.deleteRecord(#=DBRPrefs);
							throw new Exception("#=LExcCorrupted");
					}
				}
			} catch (Exception ex) {
				if (dis!=null) {
					try {
						dis.close();
						dis=null;
					} catch (Exception e) {}
				}

				if (bais!=null) {
					try {
						bais.close();
						bais=null;
					} catch (Exception e) {}
				}

				System.gc();
			}
		}
	}

	/**
	 * Get a length encoded string from the input stream.
	 *
	 * @param dis DataInputStream to read from.
	 * @param bLen Remaining length of DataInputStream.
	 * @return string.
	 * @exception Exception.
	 */
	private String prfGetString(DataInputStream dis, int bLen) throws Exception {
		int len=dis.readShort();
		String str="";
		byte[] buf;

		prfReadLen=2;
		bLen-=prfReadLen;
		if ((len>0) && (bLen>=len)) {
			prfReadLen+=len;

			buf=new byte[len];
			dis.read(buf, 0, len);
			str=new String(buf);
			buf=null;
			System.gc();
		}

		return str;
	}

	/**
	 * Form an byte array to be written to data store with all the
	 * preferences entries.
	 *
	 * @return byte array.
	 * @exception Exception.
	 */
	private byte[] formPreferences() throws Exception {
		ByteArrayOutputStream baos=null;
		DataOutputStream dos=null;
		byte[] b=null;

		try {
			baos=new ByteArrayOutputStream();
			dos=new DataOutputStream(baos);

			prfWriteString(dos, (byte)#=PrfPin, pin);
			prfWriteString(dos, (byte)#=PrfUID, uid);

			b=baos.toByteArray();
		} catch (Exception ex) {
			throw new Exception("#=(LExcFailedPrefs): "+ex.getMessage());
		} finally {
			if (dos!=null) {
				try {
					dos.close();
					dos=null;
				} catch (Exception ex) {}
			}

			if (baos!=null) {
				try {
					baos.close();
					baos=null;
				} catch (Exception ex) {}
			}

			System.gc();
		}

		return b;
	}

	/**
	 * Write a preferences entry to the DataOutputStream.
	 *
	 * @param dos DataOutputStream.
	 * @param id Preferences ID.
	 * @param str Value.
	 * @exception Exception.
	 */
	private void prfWriteString(DataOutputStream dos, byte id, String str) throws Exception {
		dos.writeByte(id);
		if (str.length()>0) {
			byte[] o=str.getBytes();
			dos.writeShort(o.length);
			dos.write(o, 0, o.length);
			o=null;
			System.gc();
		} else {
			dos.writeShort(0);
		}
	}

	/**
	 * Save the preferences.
	 */
	private void savePrefs() {
		if ((pin=prfPin.getString())==null) {
			pin="";
		}

		if ((uid=prfUID.getString())==null) {
			uid="";
		}

		try {
			byte[] b=formPreferences();
			dbPrf.setRecord(#=DBRPrefs, b, 0, b.length);
		} catch (Exception ex) {
			popup("#=LPopupUnrecErr", "#=(LPopupCannotStorePrefs): "+ex.getMessage());
		}
	}
