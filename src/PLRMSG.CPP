//******************************************************************
// plrmsg.cpp
//******************************************************************

#include "diablo.h"
#pragma hdrstop
#include "msg.h"
#include "engine.h"
#include "scrollrt.h"
#include "control.h"
#include "inv.h"
#include "quests.h"
#include "gendung.h"
#include "items.h"
#include "player.h"


//******************************************************************
// private
//******************************************************************
typedef struct TMsg {
	long lMsgTime;
	BYTE bPlr;
	// a little padding for "(level %d): " and stuff
	char str[MAX_SEND_STR_LEN + PLR_NAME_LEN + 32];
} TMsg;


#define MAX_MSGS		8
#define MAX_MSGS_MASK	(MAX_MSGS-1)
static TMsg sgMsgs[MAX_MSGS];
static BYTE sgbNextMsg;

#define MSG_TIME		10000	// milliseconds
#define MSG_XOFF		10
#define MSG_YOFF		70
#define MSG_YFONT		10
#define MSG_YDELTA		((GAMEY - MSG_YOFF) / MAX_MSGS)
#define MAX_MSG_LINES	3


//******************************************************************
//******************************************************************
void plrmsg_hold(BOOL bStart) {
	static long slStartTime;

	// save starting time
	if (bStart) {
		slStartTime = - (long) GetTickCount();
	}
	else {
		// calculate hold interval
		slStartTime += (long) GetTickCount();
		TMsg * pMsg = sgMsgs;
		for (int i = MAX_MSGS; i--; pMsg++)
			pMsg->lMsgTime += slStartTime;
	}
}


//******************************************************************
//******************************************************************
void sysmsg_add_string(const char * pszMsg) {
	app_assert(pszMsg);
	TMsg * pMsg = &sgMsgs[sgbNextMsg++];
	sgbNextMsg &= MAX_MSGS_MASK;

	pMsg->bPlr = MAX_PLRS;
	pMsg->lMsgTime = (long) GetTickCount();

	strncpy(pMsg->str,pszMsg,sizeof(pMsg->str));
	pMsg->str[sizeof(pMsg->str) - 1] = 0;
}


//******************************************************************
//******************************************************************
void __cdecl sysmsg_add(const char * pszFmt,...) {
	app_assert(pszFmt);
	TMsg * pMsg = &sgMsgs[sgbNextMsg++];
	sgbNextMsg &= MAX_MSGS_MASK;

	pMsg->bPlr = MAX_PLRS;
	pMsg->lMsgTime = (long) GetTickCount();

	va_list args;
	va_start(args,pszFmt);
	vsprintf(pMsg->str,pszFmt,args);
	va_end(args);
	app_assert(strlen(pMsg->str) < sizeof(pMsg->str));
}


//******************************************************************
//******************************************************************
void plrmsg_add(int pnum, const char * pszStr) {
	app_assert((DWORD) pnum < MAX_PLRS);
	app_assert(pszStr);

	TMsg * pMsg = &sgMsgs[sgbNextMsg++];
	sgbNextMsg &= MAX_MSGS_MASK;

	pMsg->bPlr = (BYTE) pnum;
	pMsg->lMsgTime = (long) GetTickCount();

	app_assert(strlen(plr[pnum]._pName) < PLR_NAME_LEN);
	app_assert(strlen(pszStr) < MAX_SEND_STR_LEN);
	sprintf(pMsg->str,"%s (lvl %d): %s",plr[pnum]._pName,plr[pnum]._pLevel,pszStr);
}


//******************************************************************
//******************************************************************
void plrmsg_update() {

#if 0
	// debugging
	static DWORD d = 0;
	static long lTime = 0;
	if ((long) GetTickCount() - lTime > 2000) {
		plrmsg_add(myplr,strGetError(d++));
		lTime = (long) GetTickCount();
	}
#endif

	TMsg * pMsg = sgMsgs;
	long lCurrTime = (long) GetTickCount();
	for (int i = MAX_MSGS; i--; pMsg++) {
		if (lCurrTime - pMsg->lMsgTime > MSG_TIME)
			pMsg->str[0] = 0;
	}
}


//******************************************************************
//******************************************************************
void plrmsg_init() {
	ZeroMemory(&sgMsgs[0],sizeof(sgMsgs));
	sgbNextMsg = 0;
}


//******************************************************************
//******************************************************************
static void draw_str(DWORD x,DWORD y,DWORD wdt,const char * pszStr,char cColor) {

	DWORD dwLines = 0;
	while (*pszStr) {
		// calc offset into draw buffer
		DWORD dwOffset = nBuffWTbl[y] + x;

		// calculate maximum wrappable line
		DWORD dwLineWdt = 0;
		const char * pszLine = pszStr;
		const char * pszEOL = pszStr;
		while (1) {
			if (! *pszLine) {
				pszEOL = pszLine;
				break;
			}
			BYTE c = char2print(*pszLine++);
			c = fonttrans[c];
			dwLineWdt += fontkern[c] + 1;
			if (! c)
				pszEOL = pszLine;
			else if (dwLineWdt >= wdt)
				break;
		}

		// draw line
		while (pszStr < pszEOL) {
			BYTE c = char2print(*pszStr++);
			c = fonttrans[c];
			if (c) DrawPanelFont(dwOffset,c,cColor);
			dwOffset += fontkern[c] + 1;
		}
	
		// move down a line
		y += MSG_YFONT;

		if (++dwLines == MAX_MSG_LINES) break;
	}
}


//******************************************************************
//******************************************************************
void plrmsg_draw() {
	static const char scColorTbl[MAX_PLRS + 1] = {
		ICOLOR_WHITE,
		ICOLOR_WHITE,
		ICOLOR_WHITE,
		ICOLOR_WHITE,
		ICOLOR_GOLD
	};

	DWORD x	= MSG_XOFF + (BUFFERX - TOTALX) /2 ;
	DWORD y = MSG_YOFF + 160;
	DWORD wdt = TOTALX - MSG_XOFF*2;
	if (chrflag || questlog) {
		if (invflag || sbookflag) return;
		x += TOTALX / 2;
		wdt -= TOTALX / 2;
	}
	else if (invflag || sbookflag) {
		wdt -= TOTALX / 2;
	}

	TMsg * pMsg = sgMsgs;
	for (int i = MAX_MSGS; i--; pMsg++,y += MSG_YDELTA) {
		if (! pMsg->str[0]) continue;
		draw_str(x,y,wdt,pMsg->str,scColorTbl[pMsg->bPlr]);
	}
}
