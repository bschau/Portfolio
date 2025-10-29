package dk.schau.vkm.util;

import dk.schau.vkm.media.Feedback;

public class Program {
	public static final int MAX_DAYS = 48;
	
	private static final short R = (short) (1 << 11);
	private static final short W = (short) (1 << 12);
	private static final short T = (short) (1 << 13);
	private static final short TR = (short) (R | T);
	private static final short TW = (short) (W | T);
	private static final short TIME_MASK = (short) (R - 1);

	// r = run, R = run back
	// w = walk, W = walk back
	private static final short[] program = new short[] {
		// Debug - only enable this during development :-)
	//	4, (short) (R|3), (short) (W|3), (short) (TR|3), (short) (TW|3),

		// week 1
		6, (short) (R|150), (short) (W|150), (short) (R|90), (short) (TR|90), (short) (TW|150), (short) (TR|150),
		6, (short) (R|150), (short) (W|150), (short) (R|90), (short) (TR|90), (short) (TW|150), (short) (TR|150),
		6, (short) (R|270), (short) (W|150), (short) (R|150), (short) (TR|150), (short) (TW|150), (short) (TR|270),

		// Week 2
		6, (short) (R|180), (short) (W|120), (short) (R|90), (short) (TR|90), (short) (TW|120), (short) (TR|180),
		6, (short) (R|300), (short) (W|120), (short) (R|150), (short) (TR|150), (short) (TW|120), (short) (TR|300),
		6, (short) (R|270), (short) (W|120), (short) (R|180), (short) (TR|180), (short) (TW|120), (short) (TR|270),

		// Week 3
		6, (short) (R|270), (short) (W|120), (short) (R|180), (short) (TR|180), (short) (TW|120), (short) (TR|270),
		8, (short) (R|180), (short) (W|120), (short) (R|180), (short) (W|60), (short) (TW|60), (short) (TR|180), (short) (TW|120), (short) (TR|180),
		6, (short) (R|390), (short) (W|45), (short) (R|45), (short) (TR|45), (short) (TW|45), (short) (TR|390),

		// Week 4
		6, (short) (R|300), (short) (W|120), (short) (R|150), (short) (TR|150), (short) (TW|120), (short) (TR|300),
		8, (short) (R|180), (short) (W|120), (short) (R|180), (short) (W|60), (short) (TW|60), (short) (TR|180), (short) (TW|120), (short) (TR|180),
		8, (short) (R|300), (short) (W|120), (short) (R|300), (short) (W|60), (short) (TW|60), (short) (TR|300), (short) (TW|120), (short) (TR|300),

		// Week 5
		4, (short) (R|420), (short) (W|90), (short) (TW|90), (short) (TR|420),
		8, (short) (R|300), (short) (W|120), (short) (R|300), (short) (W|60), (short) (TW|60), (short) (TR|300), (short) (TW|120), (short) (TR|300),
		4, (short) (R|600), (short) (W|120), (short) (TW|120), (short) (TR|600),

		// Week 6
		4, (short) (R|600), (short) (W|90), (short) (TW|90), (short) (TR|600),
		8, (short) (R|300), (short) (W|120), (short) (R|300), (short) (W|60), (short) (TW|60), (short) (TR|300), (short) (TW|120), (short) (TR|300),
		4, (short) (R|600), (short) (W|90), (short) (TW|90), (short) (TR|600),

		// Week 7
		3, (short) (R|720), (short) (TW|120), (short) (TR|480),
		8, (short) (R|300), (short) (W|120), (short) (R|300), (short) (W|60), (short) (TW|60), (short) (TR|300), (short) (TW|120), (short) (TR|300),
		4, (short) (R|600), (short) (W|90), (short) (TW|90), (short) (TR|600),

		// Week 8
		3, (short) (R|720), (short) (TW|180), (short) (TR|480),
		8, (short) (R|300), (short) (W|120), (short) (R|300), (short) (W|60), (short) (TW|60), (short) (TR|300), (short) (TW|120), (short) (TR|300),
		4, (short) (R|720), (short) (TR|180), (short) (TW|180), (short) (TR|300),

		// Week 9
		4, (short) (R|600), (short) (TR|420), (short) (TW|60), (short) (TR|180),
		6, (short) (R|420), (short) (W|120), (short) (R|210), (short) (TR|210), (short) (TW|120), (short) (TR|420),
		3, (short) (R|900), (short) (TW|120), (short) (TR|300),

		// Week 10
		4, (short) (R|600), (short) (TR|300), (short) (TW|120), (short) (TR|300),
		6, (short) (R|420), (short) (W|120), (short) (R|210), (short) (TR|210), (short) (TW|120), (short) (TR|420),
		4, (short) (R|600), (short) (W|60), (short) (TW|60), (short) (TR|600),

		// Week 11
		3, (short) (R|900), (short) (TW|120), (short) (TR|600),
		2, (short) (R|600), (short) (TR|600),
		2, (short) (R|600), (short) (TR|600),

		// Week 12
		2, (short) (R|600), (short) (TR|600),
		2, (short) (R|600), (short) (TR|600),
		2, (short) (R|600), (short) (TR|600),

		// Week 13
		2, (short) (R|780), (short) (TR|720),
		2, (short) (R|480), (short) (TR|420),
		2, (short) (R|600), (short) (TR|600),

		// Week 14
		2, (short) (R|600), (short) (TR|600),
		2, (short) (R|780), (short) (TR|720),
		2, (short) (R|690), (short) (TR|690),

		// Week 15
		2, (short) (R|780), (short) (TR|720),
		2, (short) (R|900), (short) (TR|900),
		2, (short) (R|840), (short) (TR|840),

		// Week 16
		2, (short) (R|780), (short) (TR|720),
		2, (short) (R|900), (short) (TR|900),
		2, (short) (R|900), (short) (TR|900),

		// If we ever go too far
		0
	};
	private static boolean[] completed = new boolean[MAX_DAYS];
	private static boolean dirty = false, running = false;
	private static int day, dayPointer;

	public static int totalTime, activities, activityTime, activityPointer;
	public static short activity;

	public static boolean selectDay(int newDay) {
		if (newDay < 0 || newDay > (MAX_DAYS - 1)) {
			return false;
		}

		day = newDay;
		dayPointer = 0;
		for (int i = 0; i < newDay; i++) {
			dayPointer += program[dayPointer] + 1;
		}

		activities = program[dayPointer++];

		totalTime = 0;

		int index = dayPointer;

		for (int i = 0; i < activities; i++) {
			totalTime += (program[index++] & TIME_MASK);
		}

		return true;
	}

	public static void startRun() {
		Feedback.stopFeedback();

		activityPointer = 0;
		activity = getActivity(activityPointer);

		// The +1 below is highly illogical - the update handler runs too soon so to get the
		// first timing correct one have to add +1 ...
		activityTime = getTimeFromActivity(activity) + 1;
		running = true;
	}
	
	public static void cancelRun() {
		Feedback.stopFeedback();
		
		running = false;
	}
	
	public static boolean countDown() {
		activityTime--;
		totalTime--;

		if (activityTime == 0) {
			Feedback.buzz(1000);
			Feedback.sound();

			activityPointer++;
			activity = getActivity(activityPointer);

			if (isDone(activity)) {
				running = false;
				Feedback.buzz(3000);
				return false;
			}

			activityTime = getTimeFromActivity(activity);
		}
		
		return true;
	}

	public static short getActivity(int activity) {
		if (activity < activities) {
			return program[dayPointer + activity];
		}

		return -1;
	}

	public static short getTimeFromActivity(short activity) {
		return (short) (activity & TIME_MASK);
	}

	public static boolean isRun(short activity) {
		return (activity & R) == R;
	}

	public static boolean isWalk(short activity) {
		return (activity & W) == W;
	}

	public static boolean isTurned(short activity) {
		return (activity & T) == T;
	}

	public static boolean isDone(short activity) {
		return activity == -1;
	}

	public static int getDay() {
		return day;
	}
	
	public static void loadCompleted() {
		String completedPreference = Preferences.getString("COMPLETED");
		int len = completedPreference.length() / 2, i;

		for (i = 0; i < MAX_DAYS; i++) {
			completed[i] = false;
		}

		for (i = 0; i < len; i++) {
			completed[((completedPreference.charAt(i++) - '0') * 10) + (completedPreference.charAt(i++) - '0')] = true;
		}
	}
	
	public static void saveCompleted() {
		if (dirty) {
			StringBuilder stringBuilder = new StringBuilder(MAX_DAYS * 2);

			for (int i = 0; i < MAX_DAYS; i++) {
				if (completed[i]) {
					stringBuilder.append((char) ((i / 10) + '0'));
					stringBuilder.append((char) ((i % 10) + '0'));
				}
			}

			Preferences.putString("COMPLETED", stringBuilder.toString());
			dirty = false;
		}
	}
	
	public static void markCompleted(int day) {
		completed[day] = true;
		dirty = true;
	}
	
	public static boolean isCompleted(int day) {
		return completed[day];
	}
	
	public static void setFirstUncompleted() {
		for (int i = 0; i < MAX_DAYS; i++) {
			if (!completed[i]) {
				selectDay(i);
				return;
			}
		}

		selectDay(0);
	}

	public static void toggleCompleted() {
		completed[day] = !completed[day];
		dirty = true;
	}
	
	public static boolean loadState() {
		if (Preferences.getString("STATE", null) == null) {
			return false;
		}

		day = Preferences.getInt("DAY");
		selectDay(day);
		
		totalTime = Preferences.getInt("TOTALTIME");
		activityTime = Preferences.getInt("ACTIVITYTIME");
		activityPointer = Preferences.getInt("ACTIVITYPOINTER");
		activity = (short) Preferences.getInt("ACTIVITY");
		
		Preferences.remove("STATE");

		running = true;
		return true;
	}
	
	public static void saveState() {
		if (running) {	
			Preferences.putInt("DAY", day);
			Preferences.putInt("TOTALTIME", totalTime);
			Preferences.putInt("ACTIVITYTIME", activityTime);
			Preferences.putInt("ACTIVITYPOINTER", activityPointer);
			Preferences.putInt("ACTIVITY", (int) activity);
			
			Preferences.putString("STATE", "yes");
			cancelRun();
		} else {
			Preferences.remove("STATE");
		}
	}
}