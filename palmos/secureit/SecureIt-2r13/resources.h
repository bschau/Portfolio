/*******************************************************************************
*
*	constants
*/
#define PINCODETOGGLEX 0
#define PINCODETOGGLEY 18
#define TEXTUALTOGGLEX 8
#define TEXTUALTOGGLEY 83
#define PASSWORDLEN 24
#define MAXTITLELEN 255
#define MAXSECRETLEN 16383
#define MAXABOUTLEN 1024
#define MAXMEMOFIELDLEN 128
#define MAXTEMPLATETITLELEN 32

/*******************************************************************************
*
*	forms
*/
#define formstart 1000
#define formeditentry 1001
#define formshowentry 1002
#define formdeleteentry 1003
#define formpincode 1004
#define formabout 1005
#define formnewdb 1006
#define formnopassword 1007
#define formaboutchangepwd 1008
#define formstartnqa 1009
#define formshowentrynqa 1010
#define formvisual 1011
#define formsecurity 1012
#define formtextual 1013
#define formshowtitle 1014
#define formshowtitlenqa 1015
#define formimportsecrets 1016
#define formimporttemplates 1017
#define formtemplateeditor 1018
#define formedittemplate 1019
#define formselecttemplate 1020
#define formworking 1021
#define formexportsecrets 1022
#define formexporttemplates 1023

/*******************************************************************************
*
* 	globals
*/
#define alertwrongentries 2000
#define alertoom 2001
#define alertonechar 2002
#define alertpwdmismatch 2003
#define alertnodbi 2004
#define alertillcred 2005
#define alertrom20 2006
#define alertpwdoldeqnew 2007
#define alertlastchance 2009
#define alertdeadentry 2010
#define alertsetdbinfo 2011
#define alertqueryfailed 2012
#define alertnomemodb 2013
#define alertnomemos 2014
#define alertimportinfo 2015
#define alertexportinfo 2016
#define alertnoneselected 2017
#define alertemptycategory 2018

#define pincodeswitchon 100
#define pincodeswitchoff 101
#define sysEditMenuID 10000

/*******************************************************************************
*
*	start
*/
#define sfcatpopup 1000
#define sfcatlist 1001
#define sfmainlist 1002
#define sfnew 1020
#define startmenu 1050
#define sfmabout 1051
#define sfmchpwd 1052
#define sfmvisual 1053
#define sfmsecurity 1054
#define sfmfrommemo 1055
#define sfmtomemo 1056
#define sfmtemplates 1057
#define sfmaincat 100

/*******************************************************************************
*
*	edit entry
*/
#define eefcatpopup 1100
#define eefcatlist 1101
#define eeftitlef 1102
#define eeftitlescrb 1103
#define eefsecretf 1104
#define eefsecretscrb 1105
#define eefok 1110
#define eeftemplate 1111
#define eefcancel 1112

/*******************************************************************************
*
*	show entry
*/
#define sefok 1200
#define seftitle 1201
#define sefedit 1202
#define sefdelete 1203
#define seffield 1204
#define seffieldscrb 1205

/*******************************************************************************
*
*	delete entry
*/
#define defyes 1300
#define defno 1301
#define defcheckbox 1302

/*******************************************************************************
*
*	password
*/
#define pinbut00 1400
#define pinbut01 1401
#define pinbut02 1402
#define pinbut03 1403
#define pinbut04 1404
#define pinbut05 1405
#define pinbut06 1406
#define pinbut07 1407
#define pinbut08 1408
#define pinbut09 1409
#define pinbutok 1410
#define pinbutclear 1411
#define pinbuttoggle 1412

/*******************************************************************************
*
*	about
*/
#define abtok 1500
#define abtfield 1501
#define abtfieldscrb 1502

/*******************************************************************************
*
*	visual
*/
#define vquickchk 1600
#define vconfirmchk 1601
#define vpagescroll 1602
#define vpinpopup 1603
#define vpinlist 1604
#define vok 1605
#define vcancel 1606

/*******************************************************************************
*
*	security
*/
#define secok 1700
#define seccancel 1701
#define secalalist 1702
#define secalapopup 1703
#define seckaboomlist 1704
#define seckaboompopup 1705
#define secprotect 1706
#define seclockdev 1707
#define seclogoutpd 1708

/*******************************************************************************
*
*	textual
*/
#define txtok 1800
#define txtpwd1 1801
#define txttoggle 1802

/*******************************************************************************
*
*	showtitle
*/
#define shtok 1900
#define shtfield 1901
#define shtfieldscrb 1902

/*******************************************************************************
*
*	importmemo
*/
#define fimimport 2000
#define fimcancel 2001
#define fimcheck 2002
#define fimcatpopupfrom 2003
#define fimcatlistfrom 2004
#define fimcatpopupto 2005
#define fimcatlistto 2006

/*******************************************************************************
*
*	templateeditor
*/
#define telist 2100
#define teok 2101
#define tenew 2102
#define teimport 2103
#define teexport 2104

#define teetitlef 2200
#define teetitlescrb 2201
#define teetemplatef 2202
#define teetemplatescrb 2203
#define teeok 2204
#define teecancel 2205
#define teedelete 2206

#define stlist 2300
#define stok 2301
#define stcancel 2302

/*******************************************************************************
*
*	export
*/
#define fexexport 2400
#define fexcancel 2401
#define fexcheck 2402
#define fexcatpopupfrom 2403
#define fexcatlistfrom 2404
#define fexcatpopupto 2405
#define fexcatlistto 2406
