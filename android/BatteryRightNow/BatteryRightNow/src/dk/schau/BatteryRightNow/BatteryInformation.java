package dk.schau.BatteryRightNow;

import java.text.DecimalFormat;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.BatteryManager;

public class BatteryInformation extends BroadcastReceiver {
	private StringBuilder stringBuilder = new StringBuilder();
	private String present = "Unknown";
	private String level = "Unknown";
	private String voltage = "Unknown";
	private String temperature = "Unknown";
	private String technology = "Unknown";
	private String status = "Unknown";
	private String health = "Unknown";
	private String pluggedIn = "Unknown";
	private BatteryRightNowActivity _batteryRightNowActivity;

	public BatteryInformation(BatteryRightNowActivity batteryRightNowActivity) {
		_batteryRightNowActivity = batteryRightNowActivity;
	}

	public String getPresent() {
		return present;
	}

	private void setPresent(Intent intent) {
		if (intent.getBooleanExtra(BatteryManager.EXTRA_PRESENT, false)) {
			present = "Present";
		} else {
			present = "Not present";
		}
	}

	public String getLevel() {
		return level;
	}

	private void setLevel(Intent intent) {
		int currentLevel = intent.getIntExtra(BatteryManager.EXTRA_LEVEL, 0);
		int currentScale = intent.getIntExtra(BatteryManager.EXTRA_SCALE, 100);

		stringBuilder.delete(0, stringBuilder.length());
		stringBuilder.append(Integer.toString((currentLevel * 100) / currentScale));
		stringBuilder.append('%');
		
		level = stringBuilder.toString();
	}

	public String getVoltage() {
		return voltage;
	}

	private void setVoltage(Intent intent) {
		DecimalFormat decimalFormat = new DecimalFormat("#.###");

		stringBuilder.delete(0, stringBuilder.length());
		stringBuilder.append(decimalFormat.format((float) intent.getIntExtra(BatteryManager.EXTRA_VOLTAGE, 0) / 1000.0f));
		stringBuilder.append('V');

		voltage = stringBuilder.toString();
	}

	public String getTemperature() {
		return temperature;
	}

	private void setTemperature(Intent intent) {
		int celsius = intent.getIntExtra(BatteryManager.EXTRA_TEMPERATURE, 0) / 10;
		int fahrenheit = (int) ((float) celsius * 1.8) + 32;

		stringBuilder.delete(0, stringBuilder.length());
		stringBuilder.append(Integer.toString(celsius));
		stringBuilder.append((char) 0x00b0);
		stringBuilder.append("C / ");
		stringBuilder.append(Integer.toString(fahrenheit));
		stringBuilder.append((char) 0x00b0);
		stringBuilder.append('F');

		temperature = stringBuilder.toString();
	}

	public String getTechnology() {
		return technology;
	}

	private void setTechnology(Intent intent) {
		technology = intent.getStringExtra(BatteryManager.EXTRA_TECHNOLOGY);
	}

	public String getStatus() {
		return status;
	}

	private void setStatus(Intent intent) {
		int currentStatus = intent.getIntExtra(BatteryManager.EXTRA_STATUS, BatteryManager.BATTERY_STATUS_UNKNOWN);

		if (currentStatus == BatteryManager.BATTERY_STATUS_CHARGING) {
			status = "Charging";
		} else if (currentStatus == BatteryManager.BATTERY_STATUS_DISCHARGING) {
			status = "Discharging";
		} else if (currentStatus == BatteryManager.BATTERY_STATUS_FULL) {
			status = "Full";
		} else if (currentStatus == BatteryManager.BATTERY_STATUS_NOT_CHARGING) {
			status = "Not charging";
		} else {
			status = "Unknown";
		}
	}

	public String getHealth() {
		return health;
	}

	private void setHealth(Intent intent) {
		int currentHealth = intent.getIntExtra(BatteryManager.EXTRA_HEALTH, BatteryManager.BATTERY_HEALTH_UNKNOWN);

		if (currentHealth == BatteryManager.BATTERY_HEALTH_DEAD) {
			health = "Dead";			
		} else if (currentHealth == BatteryManager.BATTERY_HEALTH_GOOD) {
			health = "Good";
		} else if (currentHealth == BatteryManager.BATTERY_HEALTH_OVERHEAT){
			health = "Over Heat";
		} else if (currentHealth == BatteryManager.BATTERY_HEALTH_OVER_VOLTAGE){
			health = "Over Voltage";
		} else if (currentHealth == BatteryManager.BATTERY_HEALTH_UNSPECIFIED_FAILURE){
			health = "Unspecified Failure";
		} else{
			health = "Unknown";
		}
	}

	public String getPluggedIn() {
		return pluggedIn;
	}

	private void setPluggedIn(Intent intent) {
		int currentPluggedIn = intent.getIntExtra(BatteryManager.EXTRA_PLUGGED, 0);

		if (currentPluggedIn == BatteryManager.BATTERY_PLUGGED_AC) {
			pluggedIn = "On A/C";
		} else if (currentPluggedIn == BatteryManager.BATTERY_PLUGGED_USB) {
			pluggedIn = "On USB";
		} else {
			pluggedIn = "On battery";
		}
	}

	public void onReceive(Context context, Intent intent) {
		if (intent.getAction().equals(Intent.ACTION_BATTERY_CHANGED)) {
			setPresent(intent);
			setLevel(intent);
			setVoltage(intent);
			setTemperature(intent);
			setTechnology(intent);
			setStatus(intent);
			setHealth(intent);
			setPluggedIn(intent);

			_batteryRightNowActivity.updateView(this);
		}
	}
}
