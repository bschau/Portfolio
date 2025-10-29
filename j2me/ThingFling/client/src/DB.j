	/**
	 * Open a record store.
	 *
	 * @param rsName RecordStore name.
	 * @param name Friendly name (used in popup).
	 * @return RecordStore.
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
