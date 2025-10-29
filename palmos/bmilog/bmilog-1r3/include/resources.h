/*
**	constants
*/
#define sysEditMenuID 10000
#define MaxNameLength 40		// if this changes, change in conduit
#define bmpNegativeBW 2000
#define bmpNullBW 2001
#define bmpPositiveBW 2002
#define bmpNegativeColor 2003
#define bmpNullColor 2004
#define bmpPositiveColor 2005
#define OffSetToColor 3
#define strSelectDate 100
#define strWhatIsTitle 101
#define strWhatIsText 102
#define strBMITableTitle 103
#define strBMITableTextAmerican 104
#define strBMITableTextEuropean 105
#define strCm 106
#define strFeet 107
#define strInch 108
#define strKg 109
#define strLbs 110
#define strAbout 111
#define strName 112
#define strDate 113
#define strBMI 114
#define strFeetInch 115
#define MaxAnswerLength 5
#define MaxQueryLength 128

/*
**	forms
*/
#define fMain 1000
#define fEditPerson 1001
#define fHistoryKgCm 1002
#define fHistoryLbsInch 1003
#define fHistoryEditKgCm 1004
#define fHistoryEditLbsInch 1005
#define fPreferences 1006
#define fAboutAbout 1007
#define fAboutHelp 1008
#define fConversion 1009
#define fQueryFindHeight 1010
#define fQueryFindWeight 1011

/*
**	alerts
*/
#define aBadRom 2000
#define aNoSuchForm 2001
#define aCannotOpenDatabase 2003
#define aDeletePerson 2004
#define aNameExists 2005
#define aNotNull 2006
#define aKgCmHeightInvalid 2009
#define aWeightInvalid 2010
#define aDeleteBMIEntry 2011
#define aBMIInfo 2012
#define aFeetHeightInvalid 2013
#define aInchHeightInvalid 2014
#define aEitherKgOrLbs 2015
#define aEitherCmOrFeetInch 2016
#define aBMIExists 2017
#define aNothingToPrint 2018

/*
**	MainForm
*/
#define MainTableLines 11
#define fMainTable 1000
#define fMainScrollBar 1001
#define fMainNew 1002
#define mMain 1050
#define mQueryFindHeight 1051
#define mQueryFindWeight 1052
#define mQueryConversion 1053
#define mOptionsPreferences 1060
#define mAboutBMITable 1070
#define mAboutWhatIs 1071
#define mAboutAbout 1072

/*
**	EditPersonForm
*/
#define MaxEntriesLength 4	/* 0000-9999 */
#define fEPOK 1100
#define fEPCancel 1101
#define fEPDelete 1102
#define fEPName 1103
#define fEPFormatPopup 1104
#define fEPFormatList 1105
#define fEPFormatLabel 1106

/*
**	HistoryForm
*/
#define HistoryTableLines 8
#define fHistoryDone 1200
#define fHistoryNew 1201
#define fHistoryName 1202
#define fHistoryTable 1203
#define fHistoryScrollBar 1204
#define fHistoryPrint 1205

/*
**	HistoryEditForm
*/
#define MaxWeightLength 3
#define MaxHeightLength 3
#define MaxHeightLengthFeet 2
#define MaxHeightLengthInch 2
#define fHistoryEditOK 1300
#define fHistoryEditCancel 1301
#define fHistoryEditDate 1302
#define fHistoryEditWeight 1303
#define fHistoryEditHeight1 1304
#define fHistoryEditHeight2 1305

/*
**	PreferencesForm
*/
#define fPreferencesOK 1400
#define fPreferencesCancel 1401
#define fPreferencesTablePopup 1403
#define fPreferencesTableList 1404
#define fPreferencesFormatPopup 1405
#define fPreferencesFormatList 1406

/*
**	About*Form
*/
#define MaxHelpLength 4096
#define fAboutAboutDone 1500
#define fAboutAboutText 1501
#define fAboutAboutScrollBar 1502
#define fAboutHelpDone 1510
#define fAboutHelpText 1511
#define fAboutHelpScrollBar 1512

/*
**	ConversionForm
*/
#define fConversionDone 1600
#define fConversionSolve 1601
#define fConversionClear 1602
#define fConversionFeet 1603
#define fConversionInch 1604
#define fConversionFeetInchA 1605
#define fConversionCm 1606
#define fConversionCmAFeet 1607
#define fConversionCmAInch 1608
#define fConversionLbs 1609
#define fConversionLbsA 1610
#define fConversionKg 1611
#define fConversionKgA 1612

/*
**	QueryFindHeightForm
*/
#define fQueryFindHeightDone 1700
#define fQueryFindHeightSolve 1701
#define fQueryFindHeightClear 1702
#define fQueryFindHeightTableText 1703
#define fQueryFindHeightTablePopup 1704
#define fQueryFindHeightTableList 1705
#define fQueryFindHeightKg 1706
#define fQueryFindHeightLbs 1707
#define fQueryFindHeightOptimal 1708

/*
**	QueryFindWeightForm
*/
#define fQueryFindWeightDone 1800
#define fQueryFindWeightSolve 1801
#define fQueryFindWeightClear 1802
#define fQueryFindWeightTableText 1803
#define fQueryFindWeightTablePopup 1804
#define fQueryFindWeightTableList 1805
#define fQueryFindWeightCm 1806
#define fQueryFindWeightFeet 1807
#define fQueryFindWeightInch 1808
#define fQueryFindWeightOptimal 1809
