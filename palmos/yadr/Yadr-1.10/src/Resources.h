/*
 * Globals.
 */
#define sysEditMenuID 10000
#define defCatHistory 100
#define bmpButtonLeftAlign 100
#define bmpButtonRightAlign 101
#define bmpButtonCenter 102
#define bmpButtonJustify 103
#define bmpButtonUpDown 104
#define bmpButtonLeftRight 105
#define bmpFolder 106
#define bmpUp 107
#define bmpBookmarks 108
/* DocNameLength matches dmDBNameLength - the actual document title is
 * 31 characters. DocNameLength includes a terminating zero. */
#define DocNameLength 32

/*
 * Strings and Bitmaps.
 */
#define strAboutText 100

/*
 * Forms.
 */
#define fAbout 1000
#define fMain 1001
#define fReader 1002
#define fFormatting 1003
#define fAddBookmark 1004
#define fBookmarks 1005
#define fEditBookmark 1006
#define fHistory 1007
#define fHistoryDetails 1008
#define fPreferences 1009
#define fMainEdit 1010
#define fFind 1011
#define fGoTo 1012
#define fReaderFullScreen 1013

/*
 * Alerts.
 */
#define aBadRom 2000
#define aCannotOpenDatabase 2001
#define aMaxPathReached 2002
#define aNoBookmarks 2003
#define aDeleteBookmark 2004
#define aDeleteAllBookmarks 2005
#define aUnknownDocType 2006
#define aInvalidDocument 2007
#define aHistoryClear 2008
#define aDeleteDetail 2009
#define aNoHistoryDB 2010
#define aNothingSelected 2011
#define aDatabaseNotFound 2012
#define aAlreadyExist 2013
#define aCannotRename 2014
#define aCannotDelete 2015
#define aNoHeaderInfo 2016
#define aDBCopyPrevent 2017
#define aIRSendError 2018
#define aIRReceiveError 2019
#define aIRDeleteDB 2020
#define aDeleteDoc 2021
#define aFindNotFound 2022

/*
 * AboutForm.
 */
#define cAboutDone 2000
#define cAboutText 2001
#define cAboutScrollBar 2002

/*
 * MainForm.
 */
#define MainTableLinesMemory 11
#define MainTableLinesCard 10
#define cMainTableMemory 2100
#define cMainScrollBarMemory 2101
#define cMainTableCard 2102
#define cMainScrollBarCard 2103
#define cMainPopup 2104
#define cMainList 2105
#define cMainPath 2106
#define cMainUp 2107
#define cMainRead 2108
#define cMainEdit 2109
#define mMain 2150
#define mMainAbout 2151
#define mMainBookmarks 2152
#define mMainHistory 2153
#define mMainPreferences 2154

/*
 * ReaderForm.
 */
#define TopY 15
#define BookmarksListLines 12
#define cReaderPopup 2200
#define cReaderList 2201
#define cReaderControl 2202
#define cReaderBookmarksPopup 2203
#define cReaderBookmarksList 2204
#define mReader 2250
#define mReaderDocList 2251
#define mReaderFormatting 2252
#define mReaderManage 2253
#define mReaderFind 2254
#define mReaderFindNext 2255
#define mReaderAdd 2256
#define mReaderGoTo 2257
#define mReaderTop 2258
#define mReader10 2259
#define mReader20 2260
#define mReader30 2261
#define mReader40 2262
#define mReader50 2263
#define mReader60 2264
#define mReader70 2265
#define mReader80 2266
#define mReader90 2267

/*
 * FormattingForm
 */
#define cFormattingOK 2300
#define cFormattingCancel 2301
#define cFormattingGDGF 2302
#define cFormattingGDGB 2303
#define cFormattingFontPush1 2304
#define cFormattingFontPush2 2305
#define cFormattingFontPush3 2306
#define cFormattingAlignL 2307
#define cFormattingAlignC 2308
#define cFormattingAlignR 2309
#define cFormattingAlignJ 2310
#define cFormattingUpDown 2311
#define cFormattingLeftRight 2312
#define cFormattingSpacingPopup 2313
#define cFormattingSpacingList 2314
#define cFormattingViewPopup 2315
#define cFormattingViewList 2316

/*
 * AddBookmarkForm
 */
#define BookmarkLength 16
#define cAddBookmarkOK 2400
#define cAddBookmarkCancel 2401
#define cAddBookmarkName 2402

/*
 * BookmarksForm
 */
#define BookmarksDocListLength 12
#define BookmarksTableLines 10
#define cBookmarksDone 2500
#define cBookmarksDocPopup 2501
#define cBookmarksDocList 2502
#define cBookmarksTable 2503
#define cBookmarksScrollBar 2504
#define cBookmarksDeleteAll 2505

/*
 * EditBookmarkForm
 */
#define cEditBookmarkOK 2400
#define cEditBookmarkCancel 2401
#define cEditBookmarkDelete 2402
#define cEditBookmarkName 2403

/*
 * HistoryForm
 */
#define HistoryTableLines 10
#define cHistoryDone 2500
#define cHistoryClear 2501
#define cHistoryPopup 2502
#define cHistoryList 2503
#define cHistoryTable 2504
#define cHistoryScrollBar 2505

/*
 * HistoryDetailsForm
 */
#define cHistoryDetailsOK 2600
#define cHistoryDetailsCancel 2601
#define cHistoryDetailsDelete 2602
#define cHistoryDetailsName 2603
#define cHistoryDetailsOpened 2604
#define cHistoryDetailsLast 2605
#define cHistoryDetailsPopup 2606
#define cHistoryDetailsList 2607

/*
 * PreferencesForm
 */
#define cPreferencesOK 2700
#define cPreferencesCancel 2701
#define cPreferencesHistoryTracking 2702

/*
 * MainEditForm
 */
#define cMainEditOK 2800
#define cMainEditCancel 2801
#define cMainEditDelete 2802
#define cMainEditBeam 2803
#define cMainEditName 2804

/*
 * FindForm
 */
#define FindTermLength 255
#define cFindOK 2900
#define cFindCancel 2901
#define cFindTxtFind 2902
#define cFindChkFromTop 2903

/*
 * GoToForm
 */
#define GoToListLines 11
#define cGoToCancel 3000
#define cGoToList 3001
