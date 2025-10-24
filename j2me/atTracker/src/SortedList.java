public class SortedList {
	private final int INITIALSIZE = 50;
	private final int GROWTH = 25;
	private String[] keys = new String[INITIALSIZE];
	private String[] tasks = new String[INITIALSIZE];
	private long[] times = new long[INITIALSIZE];
	private int count = 0;

	/**
	 * Add a task/time combo to the SortedList.
	 *
	 * @param task Task.
	 * @param time Time.
	 */
	public int add(String task, int time) {
		String key = task.toLowerCase();

		if (count >= keys.length) {
			String[] t = new String[count + GROWTH];
			System.arraycopy(keys, 0, t, 0, count);
			keys = t;

			t = new String[count + GROWTH];
			System.arraycopy(tasks, 0, t, 0, count);
			tasks = t;

			long[] i = new long[count + GROWTH];
			System.arraycopy(times, 0, i, 0, count);
			times = i;

			System.gc();
		}

		int i, r;
		for (i = 0; i < count; i++) {
			r = keys[i].compareTo(key);
			if (r > 0) {
				if ((count - i) > 0) {
					int dst = i + 1, len = count - i;

					System.arraycopy(keys, i, keys, dst, len);
					System.arraycopy(tasks, i, tasks, dst, len);
					System.arraycopy(times, i, times, dst, len);
				}
				break;
			} else if (r == 0) {
				times[i] += time;
				return i;
			}
		}

		keys[i] = key;
		tasks[i] = task;
		times[i] = time;
		count++;
		return i;
	}

	/**
	 * Return number of elements.
	 *
	 * @return count.
	 */
	public int getCount() {
		return count;
	}

	/**
	 * Get key of index.
	 *
	 * @param idx Index.
	 * @return key.
	 */
	public String getKey(int idx) {
		return keys[idx];
	}

	/**
	 * Get task of index.
	 *
	 * @param idx Index.
	 * @return task.
	 */
	public String getTask(int idx) {
		return tasks[idx];
	}

	/**
	 * Get time of index.
	 *
	 * @param idx Index.
	 * @return time.
	 */
	public long getTime(int idx) {
		return times[idx];
	}
}
