#ifndef PROTOS_H
#define PROTOS_H

#include <PalmOS.h>
#include "structures.h"

/* about.c */
void AboutAbout(void);
void AboutHelp(Char *, Char *);

/* bmi.c */
UInt16 BmiCalculate(UInt16, UInt32, UInt32);
void BmiDrawSmiley(UInt16, RectangleType *);
UInt16 BmiCount(DB *, UInt32);
Boolean BmiFirst(DB *, UInt32, UInt16 *);
void BmiDeleteAll(UInt32);
UInt16 BmiFindDate(UInt32, UInt16);

/* conversion.c */
Boolean ConversionInit(void);
Boolean ConversionEventHandler(EventPtr);

/* db.c */
Int16 DBCompare(void *, void *, Int16, SortRecordInfoPtr, SortRecordInfoPtr, MemHandle);
void DBCreateDatabase(void);
void DBInit(DB *, Char *);
Boolean DBOpen(DB *, UInt16);
void DBClose(DB *);
Boolean DBOpenDatabase(DB *, UInt16, Char *);
void DBSetRecord(DB *, MemHandle, UInt16);
Boolean DBDeleteRecord(DB *, UInt16, UInt16, Char *);
UInt16 DBTypeIdSearch(DB *, UInt8, UInt32, UInt16);

/* editpersonform.c */
Boolean EditPerson(UInt16);

/* find.c */
void FSearch(FindParamsType *);

/* history.c */
Boolean HistoryInit(void);
Boolean HistoryEventHandler(EventPtr);

/* mainform.c */
Boolean MainInit(void);
Boolean MainEventHandler(EventPtr);

/* palmprint.c */
Boolean PalmPrintOpen(void);
void PalmPrintGetInfo(UInt32 *, UInt32 *);
void PalmPrintLineModeStart(void);
void PalmPrintLineModeStop(void);
void PalmPrintLineModePrint(Char *);

/* preference.c */
Boolean PreferencesGet(Preferences *);
void PreferencesNew(void);

/* query.c */
Boolean QueryFindHeightInit(void);
Boolean QueryFindHeightEventHandler(EventPtr);
Boolean QueryFindWeightInit(void);
Boolean QueryFindWeightEventHandler(EventPtr);

/* sqrt.c */
double __ieee754_sqrt(double);

/* ui.c */
void *UIFormGetObject(UInt16);
void UIFormPrologue(FormSave *, UInt16, FormEventHandlerType *);
Boolean UIFormEpilogue(FormSave *, void (*)(), UInt16);
void UIFieldFocus(UInt16);
Char *UIFieldGetText(UInt16);
void UIFieldSetText(UInt16, Char *);
void UIFieldUneditable(UInt16);
void UIFieldClear(UInt16);
Boolean UIFieldScrollBarHandler(EventType *, UInt16, UInt16);
Boolean UIFieldScrollBarKeyHandler(EventType *, UInt16, UInt16);
void UIFieldUpdateScrollBar(UInt16, UInt16);
void UIFieldScrollLines(UInt16, Int16);
void UIFieldPageScroll(UInt16, UInt16, WinDirectionType);
void UITextDraw(Char *, RectangleType *);
void UITextDrawR(Char *, RectangleType *);
void UITableInit(DB *, Table *, UInt16, UInt16, TableDrawItemFuncPtr, TableStyle *);
void UITableScroll(Table *, Int16);
void UITableUpdateValues(Table *);
Boolean UITableEvents(Table *, EventType *);
void UITableChanged(Table *, UInt16);
Boolean UIRefocus(UInt16 *, WChar);
void UIShowObject(UInt16);
void UIListSetPopup(UInt16, UInt16, Int16);

/* utils.c */
Boolean UtilsRomOK(void);
MemHandle UtilsTidyString(Char *);
UInt16 UtilsPackDate(Int16, Int16, Int16);
void UtilsUnpackDate(UInt16, Int16 *, Int16 *, Int16 *);
UInt16 UtilsGetHistoryForm(UInt16);
void UtilsCmToFeetInch(UInt32, UInt32 *, UInt32 *);

/* debug.c */
#ifdef DEBUG
void DAlert(UInt8 *, ...);
#endif

#endif
