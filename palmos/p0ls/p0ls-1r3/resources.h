/*******************************************************************************
 *
 *	constants
 */
#define AXIOMLEN 32
#define PRODRULELEN 64
#define KEYSIZE 32

/*******************************************************************************
 *
 *	forms
 */
#define formstart 1000
#define formabout 1001
#define formprodrules 1002
#define formworking 1003
#define formdisplay 1004
#define formio 1005

/*******************************************************************************
 *
 * 	globals
 */
#define alertrom20 2000
#define alertoom 2001
#define alertvalues 2002
#define alertaxiomerr 2003
#define alertruleerr 2004
#define alertconaxerr 2005
#define alertconruleerr 2006
#define alertexerr 2007
#define alertstackunbal 2008
#define alertstringerr 2009
#define alertoffscrwin 2010
#define alertdelete 2011
#define alertnodelete 2012
#define alertdeletefailed 2013
#define alertnoload 2014
#define alertloadfailed 2015
#define alertnosave 2016
#define alertsavefailed 2017
#define alertoverwrite 2018

/*******************************************************************************
 *
 *	menu
 */
#define menuglobal 4000
#define menuabout 4001

#define menuundo 4010
#define menuedit 4011
#define menucut 4012
#define menucopy 4013
#define menupaste 4014
#define menuselectall 4015
#define menukeyboard 4016
#define menugraffiti 4017

/*******************************************************************************
 *
 *	start
 */
#define fsdepth 1001
#define fsangle 1002
#define fsaxiom 1003
#define fsrules 1004
#define fsload 1005

/*******************************************************************************
 *
 *	production rules
 */
#define prgenerate 1100
#define praxiom 1101
#define prclear 1102
#define prr0 1120
#define prr1 1121
#define prr2 1122
#define prr3 1123
#define prr4 1124
#define prr5 1125
#define prr6 1126
#define prr7 1127
#define prr8 1128
#define prr9 1129
#define pruf 1130
#define prlf 1131

/*******************************************************************************
 *
 *	working
 */
#define fwcancel 1200

/*******************************************************************************
 *
 *	display
 */
#define ddone 1300
#define dsave 1301

/*******************************************************************************
 *
 *	ioform (load, save)
 */
#define iocancel 1400
#define ioaction 1401
#define iodelete 1402
#define iolist 1403
#define iofield 1404
