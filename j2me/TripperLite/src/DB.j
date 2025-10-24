	/**
	 * Open a record store.
	 *
	 * @param rsName RecordStore name.
	 * @param name Friendly name (used in popup).
	 * @return RecordStore.
	 * @exception Exception.
	 */
	private RecordStore dbOpen(String rsName, String name) throws Exception {
		RecordStore r=null;

		try {
			r=RecordStore.openRecordStore(rsName, true);
		} catch (Exception e) {
			throw new Exception("#=(LExcNoDb): "+name);
		}
		return r;
	}

	/**
	 * Close a recordstore.
	 *
	 * @param rs RecordStore.
	 */
	private void dbClose(RecordStore rs) {
		if (rs!=null) {
			try {
				rs.closeRecordStore();
			} catch (Exception e) {}
		}
	}

	/**
	 * Store a byte stream at an optional index.
	 *
	 * @param rs RecordStore.
	 * @param data Data to store.
	 * @param idx Index (or -1 for new record).
	 * @exception Exception.
	 */
	private void dbSave(RecordStore rs, byte[] data, int idx) throws Exception {
		if (idx==-1) {
			rs.addRecord(data, 0, data.length);
		} else {
			rs.setRecord(idx, data, 0, data.length);
		}
	}
