package dk.schau.vkm;

import javax.microedition.rms.RecordStore;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;

public class Program
{
	public static final int MAX_DAYS = 48;

	private static final short R = (short) (1 << 11);
	private static final short W = (short) (1 << 12);
	private static final short T = (short) (1 << 13);
	private static final short TR = (short) (R | T);
	private static final short TW = (short) (W | T);
	private static final short TIME_MASK = (short) (R - 1);

	private static final short[] program = new short[]
	{
		// Debug
		//4, (short) (R|3), (short) (W|3), (short) (TR|3), (short) (TW|3),

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
	private static RecordStore rs = null;
	private static int day, dayPtr;
	public static int totalTime, activities;

	public static boolean selectDay(int d)
	{
		if (d < 0 || d > MAX_DAYS - 1)
		{
			return false;
		}

		day = d;
		dayPtr = 0;
		for (int i = 0; i < d; i++)
		{
			int len = program[dayPtr++];

			dayPtr += len;
		}

		activities = program[dayPtr++];

		totalTime = 0;

		int p = dayPtr;

		for (int i = 0; i < activities; i++)
		{
			totalTime += (program[p++] & TIME_MASK);
		}

		return true;
	}

	public static short getActivity(int act)
	{
		if (act < activities)
		{
			return program[dayPtr + act];
		}

		return -1;
	}

	public static short getTimeFromActivity(short act)
	{
		return (short) (act & TIME_MASK);
	}

	public static boolean isRun(short act)
	{
		return (act & R) == R ? true : false;
	}

	public static boolean isWalk(short act)
	{
		return (act & W) == W ? true : false;
	}

	public static boolean isTurned(short act)
	{
		return (act & T) == T ? true : false;
	}

	public static boolean isDone(short act)
	{
		return (act == -1) ? true : false;
	}

	public static int getDay()
	{
		return day;
	}

	public static void saveCompleted()
	{
		ByteArrayOutputStream baos = null;
		DataOutputStream dos = null;

		try
		{
			rs = RecordStore.openRecordStore(C.DBCOMPLETED, true);

			baos = new ByteArrayOutputStream();
			dos = new DataOutputStream(baos);

			for (int i = 0; i < Program.MAX_DAYS; i++)
			{
				dos.writeBoolean(completed[i]);
			}

			byte[] b = baos.toByteArray();

			if (rs.getNumRecords() > 0)
			{
				rs.setRecord(1, b, 0, b.length);
			}
			else
			{
				rs.addRecord(b, 0, b.length);
			}
		}
		catch (Exception ex)
		{					
		}
		finally
		{
			if (dos != null)
			{
				try
				{
					dos.close();
				}
				catch (Exception ex)
				{
				}
			}

			if (rs != null)
			{
				try
				{
					rs.closeRecordStore();
				}
				catch (Exception ex)
				{
				}
			}

			rs = null;
		}
	}

	public static void loadCompleted()
	{
		ByteArrayInputStream bais = null;
		DataInputStream dis = null;

		try
		{
			rs = RecordStore.openRecordStore(C.DBCOMPLETED, true);

			if (rs.getNumRecords() > 0)
			{
				byte[] b = rs.getRecord(1);
				int i;

				bais = new ByteArrayInputStream(b);
				dis = new DataInputStream(bais);

				for (i = 0; i < Program.MAX_DAYS; i++)
				{
					completed[i] = dis.readBoolean();
				}
			}
		}
		catch (Exception ex)
		{					
		}
		finally
		{
			if (dis != null)
			{
				try
				{
					dis.close();
				}
				catch (Exception ex)
				{					
				}
			}

			if (rs != null)
			{
				try
				{
					rs.closeRecordStore();
				}
				catch (Exception ex)
				{
				}
			}

			rs = null;
		}
	}

	public static void setCompleted(int d)
	{
		completed[d] = true;
	}
	
	public static boolean getCompleted(int d)
	{
		return completed[d];
	}

	public static void setFirstUncompleted()
	{
		for (int idx = 0; idx < MAX_DAYS; idx++)
		{
			if (completed[idx] == false)
			{
				selectDay(idx);
				return;
			}
		}

		selectDay(0);
	}

	public static void toggleCompleted()
	{
		completed[day] = !completed[day];
	}
}
