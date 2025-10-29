/*
**	bmi.c
**
**	Various bmi utilities.
*/
#include "bmilog.h"

/*
**	BmiCalculate
**
**	Calculate BMI based on format.
**
**	 -> f		Format.
**	 -> h		Height.
**	 -> w		Weight.
**
**	Returns BMI index.
*/
UInt16
BmiCalculate(UInt16 f, UInt32 h, UInt32 w)
{
	UInt16 bmi;

/*
**	Calculations:
**
**	KgCm:
**
**	  bmi = weight / (height in meters * height in meters)
**
**	So, a person weighing 90 kg and being 180 cm tall:
**
**	  90 / (1,8 * 1,8) =	27 (BMI)
**
**
**	LbsInch
**
**	  bmi = weight * 704,5 / (height in inches * height in inches)
**
**	So, a person weighing 198 lbs and being 5"11' tall (the same person
**	as above :o)
**
**	  198 * 704,5 / (71 * 71) =	27 (BMI)
*/
	if (f==FormatKgCm)
		bmi=(UInt16)(w*10000/(h*h));
	else
		bmi=(UInt16)((w*7045)/(h*h*10));

	return bmi;
}

/*
**	BmiDrawSmiley
**
**	Based on BMI draw a smiley in the rectangle.
**
**	 -> bmi		BMI.
**	 -> r		Rectangle.
*/
void
BmiDrawSmiley(UInt16 bmi, RectangleType *r)
{
	MemHandle src=bmpNegativeH;
	Coord x=r->topLeft.x, y=r->topLeft.y, w, h;
	BitmapType *bmp;
	
	if (prefs.bmiTable==BmiTableAmerican) {
		if (bmi<20 || (bmi>=25 && bmi<30))
			src=bmpNullH;
		else if (bmi>=20 && bmi<25)
			src=bmpPositiveH;
	} else {
		bmi*=10;
		if (bmi<185 || (bmi>=250 && bmi<300))
			src=bmpNullH;
		else if (bmi>=185 && bmi<250)
			src=bmpPositiveH;
	}

	bmp=MemHandleLock(src);
	BmpGlueGetDimensions(bmp, &w, &h, NULL);
	x+=(r->extent.x-w)/2;
	y+=(r->extent.y-h)/2;

	WinEraseRectangle(r, 0);
	WinDrawBitmap(bmp, x, y);
	MemHandleUnlock(src);
}

/*
**	BmiCount
**
**	Count the number of bmi records for a given person.
**
**	 -> dbo		Database object.
**	 -> id		Person ID.
*/
UInt16
BmiCount(DB *dbo, UInt32 id)
{
	DmOpenRef db=dbo->db;
	UInt16 recIdx=0, count=0;
	MemHandle mh;
	BMI *b;

	for (EVER) {
		if ((mh=DmQueryNextInCategory(db, &recIdx, dmAllCategories))==NULL)
			break;

		b=MemHandleLock(mh);
		if (b->r.type>RBmi) {
			MemHandleUnlock(mh);
			break;
		}
		if (b->r.type==RBmi) {
			if (b->r.id==id)
				count++;
		}
		MemHandleUnlock(mh);

		recIdx++;
	}

	return count;
}

/*
**	BmiFirst
**
**	Fast forward to the first BMI for this id.
**
**	 -> dbo		Database object.
**	 -> id		ID.
**	<-> item	Start (and stop) item.
**
**	Returns true if a BMI was found, false otherwise.
*/
Boolean
BmiFirst(DB *dbo, UInt32 id, UInt16 *item)
{
	DmOpenRef db=dbo->db;
	MemHandle mh;
	BMI *b;
	UInt8 type;
	UInt32 bId;

	for (EVER) {
		if ((mh=DmQueryNextInCategory(db, item, dmAllCategories))==NULL)
			break;

		b=MemHandleLock(mh);
		type=b->r.type;
		bId=b->r.id;
		MemHandleUnlock(mh);
		if (type>RBmi)
			break;

		if (type==RBmi) {
			if (bId>id)
				break;

			if (bId==id)
				return true;
		}

		(*item)++;
	}

	return false;
}

/*
**	BmiDeleteAll
**
**	Delete all BMI entries for a person.
**
**	 -> id		Person id.
*/
void
BmiDeleteAll(UInt32 id)
{
	DmOpenRef db=dbData.db;
	UInt16 recIdx=0;
	MemHandle mh;
	BMI *b;

	for (EVER) {
		if ((mh=DmQueryNextInCategory(db, &recIdx, dmAllCategories))==NULL)
			break;

		b=MemHandleLock(mh);
		if (b->r.type==RBmi && b->r.id==id) {
			MemHandleUnlock(mh);
			DmDeleteRecord(db, recIdx);
		} else {
			MemHandleUnlock(mh);
			recIdx++;
		}
	}
}

/*
**	BmiFindDate
**
**	Find a given entry for a given person at a given date.
**
**	 -> id		Person ID.
**	 -> date	Date.
**
**	Returns the record index if found, dmMaxRecordIndex if not.
*/
UInt16
BmiFindDate(UInt32 id, UInt16 date)
{
	DmOpenRef db=dbData.db;
	UInt16 recIdx=0;
	Record *r;
	MemHandle mh;

	for (EVER) {
		if ((mh=DmQueryNextInCategory(db, &recIdx, dmAllCategories))==NULL)
			break;

		r=MemHandleLock(mh);
		if (r->type==RBmi && r->id==id) {
			if (((BMI *)r)->date==date) {
				MemHandleUnlock(mh);
				return recIdx;
			}
		}
		MemHandleUnlock(mh);
		recIdx++;
	}

	return dmMaxRecordIndex;
}
