//******************************************************************
// tmsg.cpp
//******************************************************************


#include "diablo.h"
#pragma hdrstop
#include "engine.h"


//******************************************************************
// timed messages
//******************************************************************
typedef struct TTimedMsg {
	struct TTimedMsg * pNext;
	long lTime;
	BYTE bLen;
	BYTE bData[1];
} TTimedMsg;

#define TIMED_MSG_DELAY	500 // ticks
static TTimedMsg * sgpTimedMsgHead;


//******************************************************************
//******************************************************************
DWORD tmsg_get(BYTE * pbMsg,DWORD dwMaxLen) {
	app_assert(pbMsg);
	
	// any msgs?
	if (! sgpTimedMsgHead) return 0;

	// is it time to get this msg?
	if (sgpTimedMsgHead->lTime - (long) GetTickCount() >= 0) return 0;

	// dequeue msg
	TTimedMsg * ptMsg = sgpTimedMsgHead;
	sgpTimedMsgHead = sgpTimedMsgHead->pNext;

	// get msg data
	BYTE bLen = ptMsg->bLen;
	app_assert(bLen);
	app_assert(bLen <= dwMaxLen);
	CopyMemory(pbMsg,ptMsg->bData,bLen);

	// free msg
	DiabloFreePtr(ptMsg);

	return bLen;
}


//******************************************************************
//******************************************************************
void tmsg_add(const BYTE * pbMsg,BYTE bLen) {
	app_assert(pbMsg);
	app_assert(bLen);

	// allocate a new message buffer for this message
	TTimedMsg * ptMsg = (TTimedMsg *) DiabloAllocPtrSig(sizeof(TTimedMsg) + bLen,'TMSG');
	ptMsg->pNext = NULL;
	ptMsg->lTime = (long) GetTickCount() + TIMED_MSG_DELAY;
	ptMsg->bLen = bLen;
	CopyMemory(ptMsg->bData,pbMsg,bLen);

	// add to tail of list
	TTimedMsg ** ppMsg = &sgpTimedMsgHead;
	while (*ppMsg) ppMsg = &(*ppMsg)->pNext;
	*ppMsg = ptMsg;
}


//******************************************************************
//******************************************************************
void tmsg_init() {
	app_assert(! sgpTimedMsgHead);
}


//******************************************************************
//******************************************************************
void tmsg_free() {
	while (sgpTimedMsgHead) {
		TTimedMsg * pNext = sgpTimedMsgHead->pNext;
		DiabloFreePtr(sgpTimedMsgHead);
		sgpTimedMsgHead = pNext;
	}
}
