//******************************************************************
// sound.cpp
// created 10.18.96
// written by Patrick Wyatt
//******************************************************************


#include "diablo.h"
#pragma hdrstop
#include "storm.h"
#include "sound.h"
#include "engine.h"
#include "resource.h"


//******************************************************************
// debugging
//******************************************************************
#define ALLOW_DUP_SOUNDS	1	// 1 in final


//******************************************************************
// extern
//******************************************************************
BOOL wave_read_header(HSFILE hsFile,WAVEFORMATEX * pwfx);
LPBYTE wave_load_file(HSFILE hsFile,WAVEFORMATEX * pwfx,CKINFO * pWaveInfo);
void wave_free_file(LPBYTE lpWave);
void ErrorDlg(int nDlgId,DWORD dwErr,const char * pszFile,int nLine);


//******************************************************************
// public
//******************************************************************
BYTE gbSndInited = FALSE;
BYTE gbMusicOn = TRUE;
BYTE gbSoundOn = TRUE;
BYTE gbDupSounds = TRUE;


//******************************************************************
// private
//******************************************************************
static LONG sglMusicVolume = VOLUME_MAX;
static LONG sglSoundVolume = VOLUME_MAX;
static int sgnMusicTrack = NUM_MUSIC;

static HINSTANCE sghDSlib = NULL;
static LPDIRECTSOUND sglpDS;
static HSFILE sghMusic = NULL;
static const char * sgszMusicTracks[NUM_MUSIC] = {
#if IS_VERSION(SHAREWARE)
	"Music\\sTowne.wav",
	"Music\\sLvla.wav",
	"Music\\sLvla.wav",
	"Music\\sLvla.wav",
	"Music\\sLvla.wav",
	"Music\\sLvla.wav",
	"Music\\sLvla.wav",
	"Music\\sintro.wav",
#else
	"Music\\DTowne.wav",
	"Music\\DLvlA.wav",
	"Music\\DLvlB.wav",
	"Music\\DLvlC.wav",
	"Music\\DLvlD.wav",
	"Music\\DLvlE.wav",
	"Music\\DLvlF.wav",
	"Music\\Dintro.wav",
#endif
};

static const char sgszSoundVol[] = "Sound Volume";
static const char sgszMusicVol[] = "Music Volume";
extern char gszProgKey[];


#if ALLOW_DUP_SOUNDS
#define MAX_DUP_DSB 8
#define S1 0xf00ff00f
#define S2 0xe11ee11e
static int signpost1 = S1;
static LPDIRECTSOUNDBUFFER sgDupDSB[MAX_DUP_DSB];
static int signpost2 = S2;
#endif

// do not play the same sound effect more frequently than this value
#define MIN_REPLAY_THRESHOLD	80	// milliseconds

//******************************************************************
//******************************************************************
void snd_update(BOOL bStopAll) {

#if ALLOW_DUP_SOUNDS
	for (DWORD d = 0; d < MAX_DUP_DSB; d++) {
		if (! sgDupDSB[d]) continue;
		if (! bStopAll) {
			DWORD dwStatus;
			HRESULT hr = sgDupDSB[d]->GetStatus(&dwStatus);
			if (hr == DS_OK && dwStatus == DSBSTATUS_PLAYING) continue;
		}

		sgDupDSB[d]->Stop();
		sgDupDSB[d]->Release();
		sgDupDSB[d] = NULL;
	}
#endif
}


//******************************************************************
//******************************************************************
#if ALLOW_DUP_SOUNDS
static LPDIRECTSOUNDBUFFER snd_dup_snd(LPDIRECTSOUNDBUFFER pDSB) {
	// did user disable duplicate sounds?
	if (! gbDupSounds) return NULL;

	for (DWORD d = 0; d < MAX_DUP_DSB; d++) {
		if (sgDupDSB[d]) continue;
		if (DS_OK != sglpDS->DuplicateSoundBuffer(pDSB,&sgDupDSB[d]))
			return NULL;
		return sgDupDSB[d];
	}

	return NULL;
}
#endif


//******************************************************************
//******************************************************************
static void snd_get_volume(const char * pszKey,LONG * plVolume) {
	DWORD dwTemp = (DWORD) *plVolume;
	if (! SRegLoadValue(gszProgKey,pszKey,0,&dwTemp))
		dwTemp = VOLUME_MAX;
	*plVolume = (LONG) dwTemp;

	if (*plVolume < VOLUME_MIN)
		*plVolume = VOLUME_MIN;
	else if (*plVolume > VOLUME_MAX)
		*plVolume = VOLUME_MAX;
	*plVolume -= *plVolume % VOLUME_STEP;
}


//******************************************************************
//******************************************************************
static void snd_set_volume(const char * pszKey,LONG lVolume) {
	SRegSaveValue(gszProgKey,pszKey,0,lVolume);
}


//******************************************************************
//******************************************************************
// pjw.patch1.start.1/13/97
static BOOL snd_restore_snd(TSnd * pSnd,LPDIRECTSOUNDBUFFER pDSB) {
	app_assert(pSnd);
	app_assert(pDSB);

	// restore sound memory
	if (DS_OK != pDSB->Restore())
		return FALSE;

	// open file containing sound only if it isn't already open
	HSFILE hsFile;
	BOOL bResult = FALSE;
	patSFileOpenFile(pSnd->pszName,&hsFile);
	patSFileSetFilePointer(hsFile,pSnd->waveInfo.dwOffset,NULL,FILE_BEGIN);

	// lock sound buffer
	BYTE * pbData;
	BYTE * pbData2;
	DWORD dwLen;
	DWORD dwLen2;
    HRESULT hr = pDSB->Lock(0,pSnd->waveInfo.dwSize,&pbData,&dwLen,&pbData2,&dwLen2,0);
	// dsound_assert(hr);
	if (DS_OK != hr) goto err;

	patSFileReadFile(hsFile,pbData,dwLen);

	// unlock sound buffer
    hr = pDSB->Unlock(pbData,dwLen,pbData2,dwLen2);
	// dsound_assert(hr);
	if (DS_OK != hr) goto err;
	
	bResult = TRUE;
err:
	patSFileCloseFile(hsFile);
	return bResult;
}
// pjw.patch1.end.1/13/97


//******************************************************************
//******************************************************************
void snd_stop_snd(TSnd * pSnd) {
	if (! pSnd) return;
	if (pSnd->pDSB == NULL) return;
	pSnd->pDSB->Stop();
}


//******************************************************************
//******************************************************************
BOOL snd_playing(TSnd * pSnd) {
	if (! pSnd) return FALSE;
	if (pSnd->pDSB == NULL) return FALSE;

	// get new status
	DWORD dwStatus;
	HRESULT hr = pSnd->pDSB->GetStatus(&dwStatus);
	// pjw.patch1.start.1/13/97
	// dsound_assert(hr);
	if (DS_OK != hr) return FALSE;
	// pjw.patch1.end.1/13/97

	// still playing?
	return (dwStatus == DSBSTATUS_PLAYING);
}


//******************************************************************
//******************************************************************
void snd_play_snd(TSnd * pSnd,LONG lVolume,LONG lPan) {
	HRESULT hr;
	LPDIRECTSOUNDBUFFER pDSB;

	if (! pSnd) return;
	if (! gbSoundOn) return;
	if (NULL == (pDSB = pSnd->pDSB))
		return;

	// don't allow sounds to be replayed too frequently
	DWORD dwCurrTime = GetTickCount();
	if (dwCurrTime - pSnd->dwLastPlayTime < MIN_REPLAY_THRESHOLD) {
		dwCurrTime = GetTickCount();		
		return;
	}

	// if the sound is already playing, duplicate the sound
	if (snd_playing(pSnd)) {
		#if ALLOW_DUP_SOUNDS
		if (NULL == (pDSB = snd_dup_snd(pSnd->pDSB))) return;
		#else
		return;
		#endif
	}

	// set parameters
	lVolume += sglSoundVolume;
	if (lVolume < VOLUME_MIN) lVolume = VOLUME_MIN;
	else if (lVolume > VOLUME_MAX) lVolume = VOLUME_MAX;

	// pjw.patch1.start.1/13/97
	hr = pDSB->SetVolume(lVolume);
	// dsound_assert(hr);
	hr = pDSB->SetPan(lPan);
	// dsound_assert(hr);
	// pjw.patch1.end.1/13/97

	if (DSERR_BUFFERLOST != (hr = pDSB->Play(0,0,0)))
		dsound_assert(hr);
	else if (snd_restore_snd(pSnd,pDSB))
		pDSB->Play(0,0,0);

	pSnd->dwLastPlayTime = dwCurrTime;
}


//******************************************************************
//******************************************************************
static void snd_alloc_buffer(TSnd * pSnd) {
	app_assert(sglpDS);

    // set up the direct sound buffer. 
    DSBUFFERDESC dsbd;
    ZeroMemory(&dsbd,sizeof(dsbd));
    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
    dsbd.dwBufferBytes = pSnd->waveInfo.dwSize;
    dsbd.lpwfxFormat = &pSnd->wfx;
	HRESULT hr = sglpDS->CreateSoundBuffer(&dsbd,&pSnd->pDSB,NULL);
	dsound_assert(hr);
}


//******************************************************************
//******************************************************************
TSnd * snd_load_snd(const char * pszName) {
	if (! sglpDS) return NULL;

	// open file containing sound
	HSFILE hsFile;
	patSFileOpenFile(pszName,&hsFile);

	// initialize sound record
	TSnd * pSnd = (TSnd *) DiabloAllocPtrSig(sizeof(TSnd),'SND ');
	ZeroMemory(pSnd,sizeof(TSnd));
	pSnd->pszName = pszName;
	pSnd->dwLastPlayTime = GetTickCount() - MIN_REPLAY_THRESHOLD - 1;

	// get the sound format
	LPBYTE lpWave = wave_load_file(hsFile,&pSnd->wfx,&pSnd->waveInfo);
	if (! lpWave) app_fatal("Invalid sound format on file %s",pSnd->pszName);

	// allocate a buffer based on sound format
	snd_alloc_buffer(pSnd);

	// lock sound buffer
	BYTE * pbData;
	BYTE * pbData2;
	DWORD dwLen;
	DWORD dwLen2;
    HRESULT hr = pSnd->pDSB->Lock(0,pSnd->waveInfo.dwSize,&pbData,&dwLen,&pbData2,&dwLen2,0);
	dsound_assert(hr);

	// read sound data
	CopyMemory(pbData,lpWave + pSnd->waveInfo.dwOffset,dwLen);

	// unlock sound buffer
    hr = pSnd->pDSB->Unlock(pbData,dwLen,pbData2,dwLen2);
	dsound_assert(hr);

	#if DEBUG_MEM
	mem_use_sig('DSND',pSnd->waveInfo.dwSize);
	#endif

	// cleanup
	wave_free_file(lpWave);
	patSFileCloseFile(hsFile);

	return pSnd;
}


//******************************************************************
//******************************************************************
void snd_free_snd(TSnd * pSnd) {
	if (pSnd) {
		if (pSnd->pDSB) {
			pSnd->pDSB->Stop();
			pSnd->pDSB->Release();
			pSnd->pDSB = NULL;
		}

		#if DEBUG_MEM
		mem_unuse_sig('DSND',pSnd->waveInfo.dwSize);
		#endif

		DiabloFreePtr(pSnd);
	}
}


//******************************************************************
//******************************************************************
static void snd_set_format(HSFILE hsFile) {
	HRESULT hr;
	app_assert(sglpDS);

	// Set up the primary direct sound buffer -- only try to
	// do this the first time we're called (when hsFile == NULL).
	// Don't worry about releasing the primary sound buffer, it
	// will be done by directsound on application exit.
	static LPDIRECTSOUNDBUFFER slpDSPrimary = NULL;
	if (hsFile == NULL) {
		DSBUFFERDESC dsbd;
		ZeroMemory(&dsbd,sizeof(dsbd));
		dsbd.dwSize = sizeof(dsbd);
		dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
		hr = sglpDS->CreateSoundBuffer(&dsbd,&slpDSPrimary,NULL);
		dsound_assert(hr);
	}
	if (! slpDSPrimary) return;

	// get sound card capabilities
	DSCAPS caps;
	caps.dwSize = sizeof(DSCAPS);
	hr = sglpDS->GetCaps(&caps);
	dsound_assert(hr);

	// setup new format for primary buffer
	WAVEFORMATEX fmt;
	if (! hsFile || !wave_read_header(hsFile,&fmt)) {
		ZeroMemory(&fmt,sizeof(fmt));
		fmt.wFormatTag = WAVE_FORMAT_PCM;
		fmt.nSamplesPerSec = 22050;			// 22k
		fmt.wBitsPerSample = 16;			// 16 bit
		fmt.nChannels = 2;					// stereo
		fmt.cbSize = 0;
	}

	// force stereo
	fmt.nChannels = 2;

	// calculate sound buffer parameters
	fmt.nBlockAlign	= fmt.nChannels * fmt.wBitsPerSample/8;
	fmt.nAvgBytesPerSec	= fmt.nSamplesPerSec * fmt.nBlockAlign;
	hr = slpDSPrimary->SetFormat(&fmt);

// pjw.patch1.start.1/13/97
	// since sound cards may not implement this feature, or may not
	// support certain formats, just ignore error codes...
	// if (hr != DS_OK && hr != DSERR_BADFORMAT && hr != DSERR_PRIOLEVELNEEDED)
	//	dsound_assert(hr);
// pjw.patch1.end.1/13/97
}


//******************************************************************
//******************************************************************
static HRESULT InDirectSoundCreate(
	GUID * lpGUID,
	LPDIRECTSOUND * lplpDS,
	IUnknown * pUnkOuter
) {
	// load direct sound library
	if (! sghDSlib) sghDSlib = LoadLibrary(TEXT("dsound.dll"));
	if (! sghDSlib) ErrorDlg(IDD_DSOUND_DLL_ERR,GetLastError(),__FILE__,__LINE__);

	// bind to DirectSoundCreate
	typedef HRESULT (WINAPI * DSCREATETYPE)(GUID *,LPDIRECTSOUND *,IUnknown *);
    DSCREATETYPE dscreatefunc = (DSCREATETYPE) GetProcAddress(sghDSlib,TEXT("DirectSoundCreate"));
	if (! dscreatefunc) ErrorDlg(IDD_DSOUND_DLL_ERR,GetLastError(),__FILE__,__LINE__);

	// call DirectDrawCreate
	return dscreatefunc(lpGUID,lplpDS,pUnkOuter);
}


//******************************************************************
//******************************************************************
void snd_init(HWND hWnd) {
	snd_get_volume(sgszSoundVol,&sglSoundVolume);
	gbSoundOn = (sglSoundVolume > VOLUME_MIN);
	snd_get_volume(sgszMusicVol,&sglMusicVolume);
	gbMusicOn = (sglMusicVolume > VOLUME_MIN);

	app_assert(! sglpDS);
	HRESULT hr = InDirectSoundCreate(NULL,&sglpDS,NULL);
	if (hr != DS_OK) sglpDS = NULL;

    if (sglpDS && DS_OK == sglpDS->SetCooperativeLevel(hWnd,DSSCL_EXCLUSIVE))
		snd_set_format(NULL);

	BOOL bSuccess = SVidInitialize(sglpDS);
	app_assert(! sglpDS || bSuccess);

	bSuccess = SFileDdaInitialize(sglpDS);
	app_assert(! sglpDS || bSuccess);

	// tell the world we can do sound
	gbSndInited = (sglpDS != NULL);
}


//******************************************************************
//******************************************************************
void snd_exit() {
	snd_update(TRUE);

	// shut down storm stuff before we release directsound
	SVidDestroy();
	SFileDdaDestroy();

	if (sglpDS) {
		sglpDS->Release();
		sglpDS = NULL;
	}

// cannot free library now, still may be in use
// by directX window procedure...
/*
	if (sghDSlib) {
		FreeLibrary(sghDSlib);
		sghDSlib = NULL;
	}
*/

	// turn off sound
	if (gbSndInited) {
		gbSndInited = FALSE;
		snd_set_volume(sgszSoundVol,sglSoundVolume);
		snd_set_volume(sgszMusicVol,sglMusicVolume);
	}
}


//******************************************************************
//******************************************************************
void music_stop() {
	if (sghMusic) {
		SFileDdaEnd(sghMusic);
		SFileCloseFile(sghMusic);
		sghMusic = NULL;
		sgnMusicTrack = NUM_MUSIC;
	}
}


//******************************************************************
//******************************************************************
void music_start(int nTrack) {
	app_assert((DWORD) nTrack < NUM_MUSIC);
	music_stop();

	if (! sglpDS) return;
	if (! gbMusicOn) return;

	#ifndef NDEBUG
	SFileEnableDirectAccess(0);
	#endif
	BOOL bResult = SFileOpenFile(sgszMusicTracks[nTrack],&sghMusic);
	#ifndef NDEBUG
	SFileEnableDirectAccess(1);
	#endif
	snd_set_format(sghMusic);
	if (! bResult) {
		sghMusic = NULL;
		return;
	}

	SFileDdaBeginEx(sghMusic,DDA_BUF_SIZE,SFILE_DDA_LOOP,0,sglMusicVolume,0,0);
	sgnMusicTrack = nTrack;
}


//******************************************************************
//******************************************************************
void music_pause(BOOL bPause) {
	if (bPause)
		music_stop();
	else if (sgnMusicTrack != NUM_MUSIC)
		music_start(sgnMusicTrack);
}


//******************************************************************
//******************************************************************
LONG music_volume(LONG lVolume) {
	if (lVolume == VOLUME_READ)	return sglMusicVolume;
	app_assert(lVolume >= VOLUME_MIN);
	app_assert(lVolume <= VOLUME_MAX);
	sglMusicVolume = lVolume;

	if (sghMusic) SFileDdaSetVolume(sghMusic,sglMusicVolume,0);

	return sglMusicVolume;
}


//******************************************************************
//******************************************************************
LONG sound_volume(LONG lVolume) {
	if (lVolume == VOLUME_READ)	return sglSoundVolume;
	app_assert(lVolume >= VOLUME_MIN);
	app_assert(lVolume <= VOLUME_MAX);
	sglSoundVolume = lVolume;
	return sglSoundVolume;
}
