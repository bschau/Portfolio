/*
 * Globals.
 */
#define sysEditMenuID 10000
#define defCategories 100
#define SLetLargeIcon 2000
#define SLetSmallIcon 2001
#define bmpKeyboard 130
#define dotFont 128
#define scissorFont 129
#define miscFont 130

/*
 * Strings and Bitmaps.
 */
#define strAboutText 100
#define strPasswordDialog 101
#define strPasswordDialogTitle 102
#define strShowDialog 103
#define strDefEntryCard 104
#define strDefEntryPassword 105
#define strIEAction 106

/*
 * Forms.
 */
#define fAbout 1000
#define fMain 1001
#define fNewDatabase 1002
#define fCreateDatabase 1003
#define fPassword 1004
#define fEdit 1005
#define fIcon 1006
#define fShow 1007
#define fPreferences 1008
#define fTemplate 1009
#define fKeywords 1010
#define fPwdGen 1011
#define fLet 1012
#define fLetManage 1013
#define fIE 1014
#define fImportSettings 1015
#define fExportSettings 1016
#define fPasswordHint 1017

/*
 * Alerts.
 */
#define aBadRom 2000
#define aCannotOpenDatabase 2001
#define aPasswordChanged 2002
#define aDeleteRecord 2003
#define aClearAll 2004
#define aNotificationError 200
#define aPwdGenError 2006
#define aPwdGenNone 2007
#define aPwdChangeReminder 2008
#define aPwdChange 2009
#define aNoCopyPrevention 2010
#define aCopyPrevBitSet 2011
#define aIRSocketError 2012
#define aIRSendError 2013
#define aIRReceiveNotInApp 2014
#define aNoRecByID 2015
#define aSletAlreadyExist 2016
#define aCannotCreateSlet 2017
#define aSletCreated 2018
#define aCannotOpenSlet 2019
#define aLetDelete 2020
#define aLetDeleteError 2021
#define aNoLibrary 2022
#define aNoSuchLet 2023
#define aPruneLet 2024
#define aPruneStatus 2025
#define aPruneNoneDeleted 2026
#define aIRSendEntryError 2027
#define aIRReceiveMustBeInApp 2028
#define aNoFocus 2029
#define aAddOrReplace 2030
#define aNothingToImport 2031
#define aNothingToExport 2032
#define aImpNothingSelected 2033
#define aImpImported 2034
#define aExpNothingSelected 2035
#define aExpExported 2036
#define aSetNewHint 2037
#define aNoPasswordHint 2038
#define aPasswordHint 2039
#define aNoPose 2040

/*
 * AboutForm.
 */
#define cAboutDone 2000
#define cAboutTxt 2001
#define cAboutScrBar 2002

/*
 * MainForm.
 */
#define MainSecretsListLines 11
#define cMainNew 2100
#define cMainPopup 2101
#define cMainList 2102
#define cMainSecretsList 2103
#define cMainPopFilter 2104
#define cMainLstFilter 2105
#define cMainTmpl 2106
#define mMain 2150
#define mMainAbout 2151
#define mMainChange 2152
#define mMainPreferences 2153
#define mMainBeam 2154
#define mMainManage 2155
#define mMainNew 2156
#define mMainNewTmpl 2157
#define mMainImport 2158
#define mMainExport 2159
#define mMainPasswordHint 2160

/*
 * NewDatabaseForm.
 */
#define cNewDatabaseNext 2200

/*
 * PasswordForm.
 */
#define cPasswordOK 2300
#define cPasswordPwd 2301
#define cPasswordPwdLabel 2302
#define cPasswordRPwd 2303
#define cPasswordRPwdLabel 2304
#define cPasswordRConfirm 2305
#define cPasswordRConfirmLabel 2306
#define cPasswordPwdDot 2307
#define cPasswordPwdTxt 2308
#define cPasswordGenerate 2309
#define cPasswordCancel 2310
#define cPasswordHint 2311
#define cPasswordRPwdKbd 2312
#define cPasswordRConfirmKbd 2313
#define cPasswordPwdKbd 2314
#define mPassword 2350
#define mPasswordGenerate 2251
#define mPasswordMask 2252
#define mPasswordUnmask 2253

/*
 * EditForm.
 */
#define TitleLength 255
#define SecretLength 16383
#define cEditOK 2400
#define cEditCancel 2401
#define cEditDelete 2402
#define cEditFldTitle 2403
#define cEditFldSecret 2404
#define cEditScrBarSecret 2405
#define cEditPopup 2406
#define cEditList 2407
#define cEditLblIcon 2408
//#define cEditGdgIcon 2409
#define cEditIcon 2409
#define cEditChkTemplate 2410
#define cEditLoad 2411
#define cEditGenerate 2412
#define mEdit 2450
#define mEditCreate 2451
#define mEditLoad 2452
#define mEditDelete 2453
#define mEditGenerate 2454

/*
 * IconForm.
 */
#define cIconNone 2500
#define cIconUp 2501
#define cIconDown 2502
#define cIconIcon 2503
#define IconX 8
#define IconY 16
#define IconW 136
#define IconH 118
#define IMW 8
#define IMH 10

/*
 * ShowForm.
 */
#define ShowSecretLines 10
#define cShowDone 2600
#define cShowEdit 2601
#define cShowFldSecret 2602
#define cShowScrBarSecret 2603
#define cShowPrev 2604
#define cShowNext 2605
#define cShowBeam 2606
#define cShowToggle 2607
#define mShow 2650
#define mShowCreate 2651
#define mShowBeam 2652
#define mShowEdit 2653

/*
 * PreferencesForm.
 */
#define cPreferencesOK 2700
#define cPreferencesCancel 2701
#define cPreferencesPopup 2702
#define cPreferencesList 2703
#define cPreferencesChkRemember 2710
#define cPreferencesFldRemember 2711
#define cPreferencesLblRemember 2712
#define cPreferencesChkRemind 2713
#define cPreferencesFldRemind 2714
#define cPreferencesLblRemind1 2715
#define cPreferencesLblRemind2 2716

#define cPreferencesChkMax 2720
#define cPreferencesFldMax 2721
#define cPreferencesChkDestroyDB 2722
#define cPreferencesLblDestroyDB 2723
#define cPreferencesChkLogOut 2724
#define cPreferencesChkTimeOut 2725
#define cPreferencesChkAutoPopup 2726

#define cPreferencesChkOpenEdit 2730
#define cPreferencesChkFind 2731
#define cPreferencesChkClipboard 2732
#define cPreferencesChkNoBeam 2733
#define cPreferencesChkNewSecret 2734
#define cPreferencesChkSecretStay 2735
#define cPreferencesChkTemplateInherit 2736
#define cPreferencesChk5Way 2737

//#define cPreferencesLASTOBJ 2750

/*
 * TemplateForm.
 */
#define TemplateItemsLines 9
#define cTemplateCancel 2800
#define cTemplatePopup 2801
#define cTemplateList 2802
#define cTemplateItems 2803

/*
 * KeywordsForm.
 */
#define KeywordLength 100
#define cKeywordsOK 2900
#define cKeywordsCancel 2901
#define cKeywordsClearAll 2902
#define cKeywordsUp 2903
#define cKeywordsDown 2904
#define cKeywordsF1 2905
#define cKeywordsF2 2906
#define cKeywordsF3 2907
#define cKeywordsF4 2908
#define cKeywordsF5 2909
#define cKeywordsS1 2910
#define cKeywordsS2 2911
#define cKeywordsS3 2912
#define cKeywordsS4 2913
#define cKeywordsS5 2914
#define cKeywordsGenerate 2915

/*
 * PwdGenForm.
 */
#define GeneratePasswordLength 20
#define cPwdGenUse 3000
#define cPwdGenCancel 3001
#define cPwdGenGenerate 3002
#define cPwdGenL6 3003
#define cPwdGenL8 3004
#define cPwdGenL10 3005
#define cPwdGenL12 3006
#define cPwdGenL20 3007
#define cPwdGenLower 3008
#define cPwdGenUpper 3009
#define cPwdGenDigits 3010
#define cPwdGenSpecials 3011
#define cPwdGenPassword 3012

/*
 * LetForm.
 */
#define cLetOK 3100
#define cLetCancel 3101
#define cLetFldName 3102

/*
 * LetManageForm.
 */
#define LetManageListLines 11
#define cLetManageDone 3200
#define cLetManageList 3201
#define cLetManageLbl 3202
#define cLetManagePrune 3203

/*
 * Import/ExportForm.
 */
#define IEListLines 11
#define cIEItems 3300
#define cIEDone 3301
#define cIEImport 3302
#define cIEExport 3303
#define cIEAll 3304
#define cIENone 3305
#define cIEPopup 3306
#define cIEList 3307

/*
 * ImportSettingsForm
 */
#define cImportSettingsCancel 3400
#define cImportSettingsImport 3401
#define cImportSettingsPopup 3402
#define cImportSettingsList 3403
#define cImportSettingsDelete 3404
#define cImportSettingsTemplate 3405
#define cImportSettingsIcon 3406
#define cImportSettingsLblIcon 3407

/*
 * ExportSettingsForm
 */
#define cExportSettingsCancel 3500
#define cExportSettingsExport 3501
#define cExportSettingsPopup 3502
#define cExportSettingsList 3503
#define cExportSettingsDelete 3504

/*
 * PasswordHintForm
 */
#define PasswordHintLength 200
#define cPasswordHintOK 3600
#define cPasswordHintCancel 3601
#define cPasswordHintField 3602
