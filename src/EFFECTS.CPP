/*-----------------------------------------------------------------------**
** 								Diablo
**
**							 Sound sgpSFX
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/EFFECTS.CPP 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------**/

#include "diablo.h"
#pragma hdrstop
#include "storm.h"
#include "sound.h"
#include "monster.h"
#include "monstdat.h"
#include "gendung.h"
#include "items.h"
#include "player.h"
#include "engine.h"
#include "effects.h"
#include "multi.h"


//******************************************************************
// debugging
//******************************************************************
#define DEBUG_STREAM	1	// 0 in final
#ifdef NDEBUG
#undef DEBUG_STREAM
#define DEBUG_STREAM 0
#endif


//******************************************************************
// extern
//******************************************************************
void snd_update(BOOL bStopAll);


//******************************************************************
// public
//******************************************************************
int sfxdelay, sfxdnum;


//******************************************************************
// private
//******************************************************************
// sound constants
#define sfx_STREAM			0x01 // streaming sound effect
#define sfx_ALLOWMULTIPLE	0x02 // only valid for non-streamed sounds
#define sfx_MENU			0x04 // menu sound
#define sfx_MONK			0x08 // only needed for the monk
#define sfx_ROGUE			0x10 // only needed for the rogue
#define sfx_WARRIOR			0x20 // only needed for the warrior
#define sfx_SORCEROR		0x40 // only needed for the sorceror
#define sfx_BARD			0x10 // reuse the rogue
#define sfx_BARBARIAN		0x20 // reuse the warrior
#define sfx_DEBUG_STREAM	0x80 // for debugging when streaming is off

#define sfx_CHAR_MASK (sfx_MONK|sfx_ROGUE|sfx_WARRIOR|sfx_SORCEROR)

// sound structure
#pragma pack(push,1)
typedef struct TSFX {
	BYTE bFlags;
	char * pszName;
	TSnd * pSnd;
} TSFX;
#pragma pack(pop)


// build sound data table
#define EFFECTS_DATA
#include "effects.h"
#undef EFFECTS_DATA
#define NUM_SFX (sizeof(sgSFX) / sizeof(sgSFX[0]))


static HSFILE sghStream = NULL;
static TSFX * sgpStreamSFX = NULL;


//******************************************************************
//******************************************************************
BOOL effect_is_playing(int nSFX) {
	app_assert(nSFX < NUM_SFX);
	TSFX * pSFX = &sgSFX[nSFX];

	// if a sound buffer is allocated, then just return play status
	if (pSFX->pSnd) return snd_playing(pSFX->pSnd);

	// if this is a streamed sound, is it the current stream?
	if (pSFX->bFlags & sfx_STREAM)
		return (pSFX == sgpStreamSFX);

	return FALSE;
}


//******************************************************************
//******************************************************************
#if DEBUG_STREAM
static void debug_stream_update(BOOL bStop) {
	// if sound isn't initialized, we don't have to do any work
	if (! gbSndInited) return;

	TSFX * pSFX = sgSFX;
	for (DWORD d = NUM_SFX; d--; pSFX++) {
		if (! (pSFX->bFlags & sfx_DEBUG_STREAM)) continue;
		if (! pSFX->pSnd) continue;
		if (!bStop && snd_playing(pSFX->pSnd)) continue;
		pSFX->bFlags &= ~sfx_DEBUG_STREAM;
		snd_free_snd(pSFX->pSnd);
		pSFX->pSnd = NULL;
	}
}
#endif


//******************************************************************
//******************************************************************
void stream_stop() {
	if (sghStream) {
		SFileDdaEnd(sghStream);
		SFileCloseFile(sghStream);
		sghStream = NULL;
		sgpStreamSFX = NULL;
	}

	#if DEBUG_STREAM
	debug_stream_update(TRUE);
	#endif
}


//******************************************************************
//******************************************************************
#if DEBUG_STREAM
static void debug_stream(TSFX * pSFX,LONG lVolume,LONG lPan) {
	// OK, the streaming stuff failed, just load
	// it into memory and play it, then free it
	if (pSFX->pSnd) return;
	if (NULL == (pSFX->pSnd = snd_load_snd(pSFX->pszName)))
		return;
	pSFX->bFlags |= sfx_DEBUG_STREAM;
	snd_play_snd(pSFX->pSnd,lVolume,lPan);
}
#endif


//******************************************************************
//******************************************************************
static void stream_play(TSFX * pSFX,LONG lVolume,LONG lPan) {
	app_assert(pSFX);
	app_assert(pSFX->bFlags & sfx_STREAM);
	stream_stop();

	// adjust volume by global volume amount
	lVolume += sound_volume(VOLUME_READ);
	if (lVolume < VOLUME_MIN) return;
	else if (lVolume > VOLUME_MAX) lVolume = VOLUME_MAX;

	// open stream file
	#ifndef NDEBUG
	SFileEnableDirectAccess(0);
	#endif
	BOOL bResult = SFileOpenFile(pSFX->pszName,&sghStream);
	#ifndef NDEBUG
	SFileEnableDirectAccess(1);
	#endif
	if (! bResult) {
		sghStream = NULL;
		#if DEBUG_STREAM
		debug_stream(pSFX,lVolume,lPan);
		#endif
		return;
	}

	// play it
	if (! SFileDdaBeginEx(sghStream,DDA_BUF_SIZE,0,0,lVolume,lPan,0)) {
		stream_stop();
		#if DEBUG_STREAM
		debug_stream(pSFX,lVolume,lPan);
		#endif
		return;
	}

	sgpStreamSFX = pSFX;
}


//******************************************************************
//******************************************************************
static void stream_update() {
	// is there a stream playing?
	if (! sghStream) return;

	// get current stream position
	DWORD nPosition,nMaxPosition;
	if (! SFileDdaGetPos(sghStream,&nPosition,&nMaxPosition))
		return;

	// if it hasn't finished playing, let it run
	if (nPosition < nMaxPosition)
		return;

	stream_stop();
}


//******************************************************************
//******************************************************************
static void sfx_stop() {
	TSFX * pSFX = sgSFX;
	for (DWORD d = NUM_SFX; d--; pSFX++) {
		if (! pSFX->pSnd) continue;
		snd_stop_snd(pSFX->pSnd);
	}
}


//******************************************************************
//******************************************************************
void InitMonsterSND(int monst) {

	// if sound isn't initialized, we don't have to do any work
	if (! gbSndInited) return;

	static const char sndletter[MAX_MS + 1] = "ahds";
	int mtype = Monsters[monst].mtype;
	for (int snd = 0; snd < MAX_MS; snd++) {
		// if this is the "special" sound, and monster doesn't have
		// a special sound then we don't need to load the sound
		if (sndletter[snd] == 's' && !monsterdata[mtype].snd_special)
			continue;
		
		for (int i = 0; i < 2; i++) {
			// derive path for sound effect
			char szTemp[MAX_PATH];
			sprintf(szTemp,monsterdata[mtype].sndfile,sndletter[snd],i+1);
			char * pszBuf = (char *) DiabloAllocPtrSig(strlen(szTemp) + 1,'SNDN');
			strcpy(pszBuf,szTemp);

			// load sound effect
			// leave ptr active
			TSnd * pSnd =  snd_load_snd(pszBuf);
			Monsters[monst].Snds[snd].effect[i] = pSnd;
			
			// if the sound was never allocated, free the file name buffer
			if (! pSnd) DiabloFreePtr(pszBuf);
		}
	}
}


//******************************************************************
//******************************************************************
void FreeMonsterSnd() {
	for (int monst = 0;monst < nummtypes; monst++) {
		int mtype = Monsters[monst].mtype;
		for (int snd = 0; snd < MAX_MS; snd++) {
			for (int i = 0; i < 2; i++) {
				TSnd * pSnd = Monsters[monst].Snds[snd].effect[i];
				if (! pSnd) continue;
				Monsters[monst].Snds[snd].effect[i] = NULL;

				// save ptr to sound effect name
				char * pszBuf = (char *) pSnd->pszName;
				pSnd->pszName = NULL;

				// free sound
				snd_free_snd(pSnd);

				// free sound name
				DiabloFreePtr(pszBuf);
			}
		}
	}
}


//******************************************************************
//******************************************************************
static BOOL calc_snd_position(int x,int y,LONG * plVolume,LONG * plPan) {
	// calc position relative to player
	x -= plr[myplr]._px;
	y -= plr[myplr]._py;
	
	// calc pan
	*plPan = (x - y) * 256;
	if (abs(*plPan) > 6400) return FALSE;

	// calc volume
	*plVolume = max(abs(x),abs(y)) * 64;
	if (*plVolume >= 6400) return FALSE;
	*plVolume = - *plVolume;

	return TRUE;
}


//******************************************************************
//******************************************************************
static void PlaySFX_priv(TSFX * pSFX,BOOL loc,int x, int y) {

	// Don't play sfx if in just onto level
	if ((plr[myplr].pLvlLoad) && (gbMaxPlayers != 1)) return;

	// if sound isn't initialized, we don't have to do any work
	if (! gbSndInited) return;
	if (! gbSoundOn) return;

	// this is the lowest level location to intercept game sound
	// effects.  If we are in some buffering mode (either because
	// we're receiving or parsing information prior to starting a
	// level) then we don't want to play any sound effects because
	// the player is not in a position to interact with anything yet.
	if (gbBufferMsgs != BUFFER_OFF) return;

	// check for sound buffer duplication
	if (pSFX->bFlags & sfx_STREAM) {
		// streams sounds are always allowed to play
		// as they will shut off any previous stream
	}
	else if (pSFX->bFlags & sfx_ALLOWMULTIPLE) {
		// we allow this sound to use multiple buffers
	}
	else if (pSFX->pSnd && snd_playing(pSFX->pSnd)) {
		// this sound is already playing -- skip
		return;
	}

	// calculate volume and panning, and clip sound if necessary
	LONG lPan = 0;
	LONG lVolume = 0;
	if (loc && !calc_snd_position(x,y,&lVolume,&lPan))
		return;

	if (pSFX->bFlags & sfx_STREAM) {
		stream_play(pSFX,lVolume,lPan);
	}
	else {
		if (! pSFX->pSnd) pSFX->pSnd = snd_load_snd(pSFX->pszName);
		if (pSFX->pSnd) snd_play_snd(pSFX->pSnd,lVolume,lPan);
	}
}

	
//******************************************************************
// Plays a monster sound effect
// i = monster# to player for
// mode = which sfx to play (attack, hit, death, etc)
//******************************************************************
void PlayEffect(int i, int mode) {

	// Don't play sfx if in just onto level
	if (plr[myplr].pLvlLoad) return;

	// choose which of the two rnd sfx to play
	// always perform random() function even if
	// sound is off so that random generators stay synced
	int nr = random(164, 2);

	// return *after* the random call so that all systems are synced
	if (! gbSndInited) return;
	if (! gbSoundOn) return;

	// this is the lowest level location to intercept game sound
	// effects.  If we are in some buffering mode (either because
	// we're receiving or parsing information prior to starting a
	// level) then we don't want to play any sound effects because
	// the player is not in a position to interact with anything yet.
	if (gbBufferMsgs != BUFFER_OFF) return;

	// monster type index (0 - nummtypes)
	int mi = monster[i]._mMTidx;

	// validate sound effect
	TSnd * pSnd = Monsters[mi].Snds[mode].effect[nr];
	if (! pSnd) {
		#ifndef NDEBUG	// don't fatal out in release version
		app_fatal("Monster sound problem\n:%s playing %i", Monsters[mi].MData->mName, mode);
		#endif
		return;
	}

	// don't allow duplication multiple monster effects
	if (snd_playing(pSnd)) return;

	// calculate volume and panning, and clip sound if necessary
	LONG lPan;
	LONG lVolume;
	if (!calc_snd_position(monster[i]._mx,monster[i]._my,&lVolume,&lPan))
		return;

	snd_play_snd(pSnd,lVolume,lPan);
}


//******************************************************************
// Determine random sfx to play
//******************************************************************
static int RndSFX(int psfx) {
	int nRand;
	if (psfx == PS_WARR69) nRand = 2;
	else if (psfx == PS_WARR14) nRand = 3;
	else if (psfx == PS_WARR15) nRand = 3;
	else if (psfx == PS_WARR16) nRand = 3;
#if !IS_VERSION(SHAREWARE)
	else if (psfx == PS_MAGE69) nRand = 2;
	else if (psfx == PS_ROGUE69) nRand = 2;
	else if (psfx == PS_MONK69) nRand = 2;
	else if (psfx == PS_BARD69) nRand = 2;
#endif
	else if (psfx == PS_SWING) nRand = 2;
	else if (psfx == LS_ACID) nRand = 2;
	else if (psfx == IS_FMAG) nRand = 2;
	else if (psfx == IS_MAGIC) nRand = 2;
	else if (psfx == IS_BHIT) nRand = 2;
//	else if (psfx == PS_WALK1) nRand = 4;
#if !IS_VERSION(SHAREWARE)
	else if (psfx == PS_WARR2) nRand = 3;
#endif
	else return psfx;

	return psfx + random(165,nRand);
}


//******************************************************************
// If player has hit something, play the hit sfx from the player
// (sword hitting metal, bone, etc)
//******************************************************************
void PlaySFX(int psfx) {
	psfx = RndSFX(psfx);
	app_assert(psfx < NUM_SFX);
	PlaySFX_priv(&sgSFX[psfx],FALSE,0,0);
}


//******************************************************************
// Like PlaySFX, but with a dungeon location
//******************************************************************
void PlaySfxLoc(int psfx, int x, int y) {
	psfx = RndSFX(psfx);
	app_assert(psfx < NUM_SFX);

	// don't let walk sounds get clipped!
	if (psfx >= PS_WALK1 && psfx <= PS_WALK4) {
		TSnd * pSnd = sgSFX[psfx].pSnd;
		if (pSnd) pSnd->dwLastPlayTime = 0;
	}

	PlaySFX_priv(&sgSFX[psfx],TRUE,x,y);
}


//******************************************************************
//******************************************************************
void sound_stop() {
	snd_update(TRUE);
	stream_stop();
	sfx_stop();

	// stop all monster sounds
	int mi, mode, nr;
	for (mi = 0; mi < nummtypes; mi++) {
		for (mode = 0; mode < MAX_MS; mode++) {
			for (nr = 0; nr < 2; nr++) {
				TSnd * pSnd = Monsters[mi].Snds[mode].effect[nr];
				snd_stop_snd(pSnd);
			}
		}
	}
}


//******************************************************************
//******************************************************************
void sound_update() {
	// if sound isn't initialized, we don't have to do any work
	if (! gbSndInited) return;

	snd_update(FALSE);
	stream_update();

	#if DEBUG_STREAM
	debug_stream_update(FALSE);
	#endif
}


//******************************************************************
//******************************************************************
void sound_exit() {
	sound_stop();
	for (DWORD d = 0; d < NUM_SFX; d++) {
		if (! sgSFX[d].pSnd) continue;

		#if DEBUG_STREAM
		sgSFX[d].bFlags &= ~sfx_DEBUG_STREAM;
		#endif

		snd_free_snd(sgSFX[d].pSnd);
		sgSFX[d].pSnd = NULL;
	}
}


//******************************************************************
//******************************************************************
static void priv_sound_init(BYTE bLoadMask) {
	// if sound manager isn't initialized, we don't have to do any work
	if (! gbSndInited) return;

	// save character load flags
	BYTE bCharMask = bLoadMask & sfx_CHAR_MASK;

	// load mask excludes character mask
	bLoadMask ^= bCharMask;

	// load sounds
	for (DWORD d = 0; d < NUM_SFX; d++) {
		// is it already loaded?
		if (sgSFX[d].pSnd) continue;

		// don't load streamed sounds
		if (sgSFX[d].bFlags & sfx_STREAM) continue;

		// if load mask is non-zero, only load sound effect if
		// it has a flag which is set in the load mask
		if (bLoadMask && !(sgSFX[d].bFlags & bLoadMask)) continue;

		// is this a character sound that we don't need?
		if (sgSFX[d].bFlags & sfx_CHAR_MASK) {
			if (! (sgSFX[d].bFlags & bCharMask))
				continue;
		}

		// load it
		sgSFX[d].pSnd = snd_load_snd(sgSFX[d].pszName);
	}
}


//******************************************************************
//******************************************************************
void sound_init() {
	BYTE bLoadMask = 0;
	if (gbMaxPlayers > 1)
		bLoadMask = sfx_CHAR_MASK;
	else if (plr[myplr]._pClass == CLASS_WARRIOR)
		bLoadMask = sfx_WARRIOR;
	else if (plr[myplr]._pClass == CLASS_ROGUE)
		bLoadMask = sfx_ROGUE;
	else if (plr[myplr]._pClass == CLASS_SORCEROR)
		bLoadMask = sfx_SORCEROR;
	else if (plr[myplr]._pClass == CLASS_MONK)
		bLoadMask = sfx_MONK;
	else if (plr[myplr]._pClass == CLASS_BARD)
		bLoadMask = sfx_BARD;
	else if (plr[myplr]._pClass == CLASS_BARBARIAN)
		bLoadMask = sfx_BARBARIAN;
	else
		app_fatal("effects:1");
	priv_sound_init(bLoadMask);
}


//******************************************************************
//******************************************************************
void menusnd_init() {
	priv_sound_init(sfx_MENU);
}


//******************************************************************
//******************************************************************
void CALLBACK menusnd_play(LPCSTR pszName) {
	// sound initialized?
	if (! gbSndInited) return;
	if (! gbSoundOn) return;

	for (DWORD d = 0; d < NUM_SFX; d++) {
		if (_stricmp(sgSFX[d].pszName,pszName)) continue;
		if (! sgSFX[d].pSnd) continue;
		if (snd_playing(sgSFX[d].pSnd)) break;
		snd_play_snd(sgSFX[d].pSnd,0,0);
		break;
	}
}

