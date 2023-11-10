package dk.schau.Kisa;

import java.util.Calendar;
import java.util.GregorianCalendar;

public class Mileage {
	private int _originYear;
	private int _originMonth;
	private int _originKm;
	private int _kmPrYear;
	
	public Mileage(int startYear, int startMonth, int startKm, int maxKmPrYear) {
		_originYear = startYear;
		_originMonth = startMonth;
		_originKm = startKm;
		_kmPrYear = maxKmPrYear;
	}
	
	public int CalculateMaxMilage() {
		int kmPrMonth = calculateKmPrMonth();
		int month = calculateUsedMonths();
		
		return (month * kmPrMonth) + _originKm;
	}
	
	private int calculateKmPrMonth() {
		return _kmPrYear / 12;
	}
	
	private int calculateUsedMonths() {
		GregorianCalendar origin = new GregorianCalendar(_originYear, _originMonth - 1, 1);
		GregorianCalendar now = new GregorianCalendar();
		
		int monthStart = (origin.get(Calendar.YEAR) * 12) + origin.get(Calendar.MONTH);
		int monthEnd = (now.get(Calendar.YEAR) * 12) + now.get(Calendar.MONTH);
		
		return monthEnd - monthStart + 1;
	}
}
