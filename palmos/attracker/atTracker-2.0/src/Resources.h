/*
 * Globals
 */
#define sysEditMenuID 10000
#define defaultCategories 100
#define SPACING 2
#define DATEFIELDWIDTH 26
#define TIMEFIELDWIDTH 26
/* 150-DATEFIELDWIDTH-TIMEFIELDWIDTH-(2*SPACING) */
#define TASKFIELDWIDTH 94
#define ARCHIVENAMEWIDTH 98
#define TASKSWIDTH 36
#define TaskLength 2000
#define CleanUpDaysLength 4
#define ArchiveDaysLength CleanUpDaysLength
#define SelectorLabelLength 32

/*
** Forms
 */
#define fMain 1000
#define fAbout 1001
#define fEdit 1002
#define fTimeSelector 1003
#define fPreferences 1004
#define fRange 1005
#define fCleanUp 1006
#define fSummary 1007
#define fArchive 1008
#define fManager 1009
#define fManagerNewDB 1010
#define fManagerEditDB 1011
#define fRestore 1012
#define fQuickTextManager 1013
#define fQuickTextManagerEdit 1014

/*
 * Strings and Bitmaps
 */
#define strAboutText 100
#define bQuickText 800
#define bBullet 801

/*
 * Alerts
 */
#define aBadRom 2000
#define aCannotOpenDatabase 2001
#define aDeleteRecord 2002
#define aCleanUpOK 2003
#define aCleanUpResult 2004
#define aSummaryBothDates 2005
#define aSummaryResult 2006
#define aNoPalmPrint 2007
#define aNothingToPrint 2008
#define aArchiveExists 2009
#define aArchiveUncreateable 2010
#define aDeleteArchive 2011
#define aNeedsArchive 2012
#define aCannotDeleteArchive 2013
#define aSelectArchive 2014
#define aArchiveOK 2015
#define aArchiveNotSet 2016
#define aArchiveResult 2017
#define aRestoreOK 2018
#define aRestoreResult 2019
#define aQuickTextManagerNoneSelected 2020
#define aQuickTextFull 2021
#define aQuickTextDelete 2022

/*
 * MainForm
 */
#define MainTableLines 10
#define cMainTable 2000
#define cMainCategoryPopup 2001
#define cMainCategoryList 2002
#define cMainScrollBar 2003
#define cMainNew 2004
#define cMainFilterPopup 2005
#define cMainFilterList 2006
#define cMainTaskLabel 2007
#define mMain 2050
#define mMainAbout 2051
#define mMainPreferences 2052
#define mMainCleanUp 2053
#define mMainPrint 2054
#define mMainSummary 2055
#define mMainArchive 2056
#define mMainRestore 2057
#define mMainManager 2058
#define mMainRegistration 2059
#define mMainQuickText 2060

/*
 * AboutForm
 */
#define cAboutDone 2100
#define cAboutText 2101
#define cAboutScrollBar 2102

/*
 * EditForm
 */
#define QuickTextListLines 9
#define cEditOK 2200
#define cEditCancel 2201
#define cEditDelete 2202
#define cEditCategoryPopup 2203
#define cEditCategoryList 2204
#define cEditTask 2205
#define cEditTaskScrollbar 2206
#define cEditDate 2207
#define cEditTime 2208
#define cEditQuickTextPopup 2209
#define cEditQuickTextList 2210

/*
 * TimeSelectorForm
 */
#define cTimeSelectorOK 2300
#define cTimeSelectorCancel 2301
#define cTimeSelectorHour 2302
#define cTimeSelectorMinuteH 2303
#define cTimeSelectorMinuteL 2304
#define cTimeSelectorUp 2305
#define cTimeSelectorDown 2306
#define cTimeSelectorPopup 2307
#define cTimeSelectorList 2308

/*
 * PreferencesForm
 */
#define cPreferencesOK 2400
#define cPreferencesCancel 2401
#define cPreferencesTime 2402
#define cPreferencesDouble 2403
#define cPreferencesAlternate 2404
#define cPreferencesGadget 2405

/*
 * RangeForm
 */
#define cRangeOK 2500
#define cRangeCancel 2501
#define cRangeStart 2502
#define cRangeEnd 2503

/*
 * CleanUpForm
 */
#define cCleanUpOK 2600
#define cCleanUpCancel 2601
#define cCleanUpPopup 2602
#define cCleanUpList 2603
#define cCleanUpSelector 2604
#define cCleanUpDays 2605
#define cCleanUpString 2606

/*
 * SummaryForm
 */
#define cSummaryOK 2700
#define cSummaryCancel 2701
#define cSummaryPopup 2702
#define cSummaryList 2703
#define cSummaryStart 2704
#define cSummaryEnd 2705
#define cSummarySearch 2706

/*
 * ArchiveForm
 */
#define cArchiveOK 2800
#define cArchiveCancel 2801
#define cArchivePopup 2802
#define cArchiveList 2803
#define cArchiveSelector 2804
#define cArchiveDays 2805
#define cArchiveString 2806
#define cArchiveName 2807
#define cArchiveMove 2808
#define cArchiveCopy 2809

/*
 * ManagerForm
 */
#define ManagerTableLines 9
#define cManagerOK 2900
#define cManagerNew 2901
#define cManagerEdit 2902
#define cManagerTable 2903
#define cManagerScrollBar 2904

/*
 * ManagerNewDBForm
 */
#define ArchiveNameLength 24
#define cManagerNewDBOK 3000
#define cManagerNewDBCancel 3001
#define cManagerNewDBName 3002

/*
 * ManagerEditDBForm
 */
#define cManagerEditDBOK 3100
#define cManagerEditDBCancel 3101
#define cManagerEditDBDelete 3102
#define cManagerEditDBName 3103

/*
 * RestoreForm
 */
#define cRestoreOK 3200
#define cRestoreCancel 3201
#define cRestoreName 3202

/*
 * QuickTextManagerForm
 */
#define QuickTextManagerLines 10
#define cQuickTextManagerOK 3300
#define cQuickTextManagerNew 3301
#define cQuickTextManagerEdit 3302
#define cQuickTextManagerList 3303

/*
 * QuickTextManagerEditForm
 */
#define QTEntryLen 32
#define cQuickTextManagerEditOK 3400
#define cQuickTextManagerEditCancel 3401
#define cQuickTextManagerEditDelete 3402
#define cQuickTextManagerEditName 3403
