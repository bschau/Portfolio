package dk.schau.vkm;

import javax.microedition.rms.RecordStore;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;

public class State
{
	private static boolean saved = false;
	private static RecordStore rs = null;
	public static short activity, activityTime;
	public static boolean running;

	public static void save()
	{
		ByteArrayOutputStream baos = null;
		DataOutputStream dos = null;

		if (saved)
		{
			return;
		}

		try
		{
			rs = RecordStore.openRecordStore(C.DBSTATE, true);

			baos = new ByteArrayOutputStream();
			dos = new DataOutputStream(baos);

			dos.writeShort(Program.getDay());
			dos.writeShort(running ? 1 : 0);
			if (running)
			{
				dos.writeShort(activity);
				dos.writeShort(activityTime);
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

			saved = true;
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

	public static int load()
	{
		ByteArrayInputStream bais = null;
		DataInputStream dis = null;

		try
		{
			rs = RecordStore.openRecordStore(C.DBSTATE, true);

			if (rs.getNumRecords() > 0)
			{
				byte[] b = rs.getRecord(1);

				bais = new ByteArrayInputStream(b);
				dis = new DataInputStream(bais);

				Program.selectDay(dis.readShort());
				if (dis.readShort() == 1)
				{
					activity = dis.readShort();
					activityTime = dis.readShort();
					return C.ScreenContinue;
				}

				return C.ScreenInitPrefs;
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
		}

		return C.ScreenInitPrefs;
	}
}
