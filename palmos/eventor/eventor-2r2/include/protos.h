#ifndef PROTOS_H
#define PROTOS_H

#include <PalmOS.h>
#include "structures.h"

/* about.c */
void About(void);

/* alarm.c */
void AlarmSetNextAlarm(void);
void AlarmShowEvents(Boolean);

/* db.c */
Int16 EventorCompareFunction(void *, void *, Int16, SortRecordInfoPtr, SortRecordInfoPtr, MemHandle);
void DBCreateDatabases(void);
void DBInit(DB *, Char *);
Boolean DBOpen(DB *, UInt16, Char *);
void DBClose(DB *);
void DBSetRecord(DB *, UInt16, void *, UInt16, UInt16);
MemHandle DBGetRecord(DB *, UInt16);

/* editform.c */
void EditOpen(void);
void EditClose(void);
Boolean EditFormInit(void);
Boolean EditFormEventHandler(EventPtr);

/* find.c */
void FSearch(FindParamsPtr);

/* mainform.c */
Boolean MainFormInit(void);
Boolean MainFormEventHandler(EventPtr);

/* palmprint.c */
Boolean PalmPrintOpen(void);
void PalmPrintGetInfo(UInt32 *, UInt32 *);
void PalmPrintLineModeStart(void);
void PalmPrintLineModeStop(void);
void PalmPrintLineModePrint(Char *);

/* preferences.c */
void PreferencesDialog(void);
Boolean PreferencesGet(Preferences *);

/* reo.c */
Boolean Reo1To2(DB *, DB *);

/* timeselector.c */
Boolean TimeSelectorNeedsAMPM(void);
Boolean TimeSelector(Int16 *, Int16 *, const Char *);

/* ui.c */
void *UIFormGetObject(UInt16);
void UIFormPrologue(FormSave *, UInt16, FormEventHandlerType *);
Boolean UIFormEpilogue(FormSave *, void (*)(), UInt16);
void UIFieldFocus(UInt16);
Char *UIFieldGetText(UInt16);
void UIFieldSetText(UInt16, Char *);
// void UIFieldUneditable(UInt16);
// void UIFieldClear(UInt16);
Boolean UIFieldScrollBarHandler(EventType *, UInt16, UInt16);
Boolean UIFieldScrollBarKeyHandler(EventType *, UInt16, UInt16);
void UIFieldUpdateScrollBar(UInt16, UInt16);
void UIFieldScrollLines(UInt16, Int16);
void UIFieldPageScroll(UInt16, UInt16, WinDirectionType);
// void UITextDraw(Char *, RectangleType *);
// void UITextDrawR(Char *, RectangleType *);
void UITableInit(DB *, Table *, UInt16, UInt16, TableDrawItemFuncPtr, TableStyle *);
void UITableScroll(Table *, Int16);
void UITableUpdateValues(Table *);
Boolean UITableEvents(Table *, EventType *);
void UITableChanged(Table *, UInt16);
// Boolean UIRefocus(UInt16 *, WChar);
void UIShowObject(UInt16);
// void UIListSetPopup(UInt16, UInt16, Int16);

/* utils.c */
UInt16 UtilsPackDate(Int16, Int16, Int16);
void UtilsUnpackDate(UInt16, Int16 *, Int16 *, Int16 *);
void UtilsDateFormat(UInt16 *, Char *);
void UtilsFormatDate(Char *, Int16, Int16, Int16);
void UtilsGetShortDate(Char *, UInt16, UInt16, Char);
MemHandle UtilsTidyString(Char *);
void UtilsSetNotification(Boolean, UInt32);
Boolean UtilsGetRomVersion(UInt32 *, UInt32);
MemHandle UtilsLockString(UInt16);
void UtilsUnlockString(MemHandle);

/* viewform.c */
Boolean ViewFormInit(void);
Boolean ViewFormEventHandler(EventPtr);

/* debug.c */
#if DEBUG==1
#define D(x) x
void DAlert(UInt8 *, ...);
void DPreLoad(void);
void DSendAlarmSignal(void);
#else
#define D(x) ;
#endif

#endif
