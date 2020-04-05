//******************************************************************
// track.cpp
//******************************************************************


#include	"diablo.h"
#pragma	hdrstop
#include	"storm.h"
#include	"msg.h"
#include	"items.h"
#include	"gendung.h"
#include	"player.h"
#include	"cursor.h"


//******************************************************************
// compiler constants
//******************************************************************
#define STOP_ON_MOUSEUP	0	// 0 in final
#define TRACKING_CURSOR	0	// 1 in final


//******************************************************************
// private
//******************************************************************
	// track state on/off
	static BOOL sgbMouseDown;
	static BYTE sgbTrackMode;

	// time tracking last performed
	#define TRACK_START_DELAY	250	// milliseconds
	#define TRACK_LOOP_DELAY	300	// milliseconds
	static long sglTrackTime;


//******************************************************************
//******************************************************************
void TrackMouse() {
	// don't track if the mouse isn't down
	if (! sgbMouseDown) return;

	// don't track if the cursor is out of range
	if (cursmx < 0) return;
	if (cursmx >= DMAXX - 1) return;
	if (cursmy < 0) return;
	if (cursmy >= DMAXY - 1) return;

	// don't track if the player isn't in a stand state or late walk
	if (plr[myplr]._pVar8 <= 6 && plr[myplr]._pmode != PM_STAND) return;

	// don't track if mouse still on same target
	if (cursmx == plr[myplr]._ptargx && cursmy == plr[myplr]._ptargy) return;

	// don't track if it hasn't been long enough since we last tracked
	long lCurrTime = (long) GetTickCount();
	long lDelta = lCurrTime - sglTrackTime;
	if (lDelta < TRACK_LOOP_DELAY) return;
	sglTrackTime = lCurrTime;

	// track!
	NetSendCmdLoc(TRUE,CMD_WALKXY,cursmx,cursmy);

	if (! sgbTrackMode) {
		sgbTrackMode = TRUE;
		#if TRACKING_CURSOR
		if (curs == GLOVE_CURS) SetCursor(TARGET_CURS);
		#endif
	}
}


//******************************************************************
//******************************************************************
void TrackInit(BOOL bMouseDown) {
	// are we already in desired state?
	if (sgbMouseDown == bMouseDown) return;
	sgbMouseDown = bMouseDown;

	if (sgbMouseDown) {
		// indicate we haven't moved from click mode to track mode yet
		sgbTrackMode = FALSE;

		// make next track time occur after TRACK_START_DELAY
		// instead of TRACK_LOOP_DELAY
		sglTrackTime = (long) GetTickCount();
		sglTrackTime += TRACK_START_DELAY;
		sglTrackTime -= TRACK_LOOP_DELAY;

		// start tracking immediately by pumping a command into queue
		NetSendCmdLoc(TRUE,CMD_WALKXY,cursmx,cursmy);
	}
	else if (sgbTrackMode) {
		// turn off track mode
		sgbTrackMode = FALSE;

		#if STOP_ON_MOUSEUP	
		// the user clicked on a map coordinate and held the mouse down
		// long enough to indicate tracking rather than clicking, so
		// since the mouse is up, stop the player at current coordinate
		NetSendCmdLoc(TRUE,CMD_WALKXY,plr[myplr]._pfutx,plr[myplr]._pfuty);
		#endif

		// restore cursor
		#if TRACKING_CURSOR
		if (curs == TARGET_CURS) SetCursor(GLOVE_CURS);
		#endif
	}
}


//******************************************************************
//******************************************************************
BOOL IsTracking() {
	return sgbTrackMode;
}