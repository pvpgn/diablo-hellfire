//******************************************************************
// sound.h
// created 10.18.96
// written by Patrick Wyatt
//******************************************************************


#ifndef H_SND
#define H_SND


//******************************************************************
// public structures
//******************************************************************
#define DDA_BUF_SIZE	0x40000

typedef struct CKINFO {
	DWORD dwSize;
	DWORD dwOffset;
} CKINFO;


typedef struct TSnd {
	WAVEFORMATEX wfx;
	CKINFO waveInfo;
	const char * pszName;
	LPDIRECTSOUNDBUFFER pDSB;
	DWORD dwLastPlayTime;
} TSnd;


//******************************************************************
// public vars
//******************************************************************
extern BYTE gbMusicOn;
extern BYTE gbSoundOn;
extern BYTE gbSndInited;


//******************************************************************
// public functions
//******************************************************************
void snd_init(HWND hWnd);
void snd_exit(void);

TSnd * snd_load_snd(const char * pszName);
void snd_free_snd(TSnd * pSnd);
void snd_play_snd(TSnd * pSnd,LONG lVolume,LONG lPan);
void snd_stop_snd(TSnd * pSnd);
BOOL snd_playing(TSnd * pSnd);


enum {
	MUSIC_TOWN,
	MUSIC_L1,
	MUSIC_L2,
	MUSIC_L3,
	MUSIC_L4,
	MUSIC_L5,
	MUSIC_L6,
	MUSIC_INTRO,
	NUM_MUSIC
};
void music_start(int nTrack);
void music_stop();


#define VOLUME_READ		1
#define VOLUME_MIN		-1600
#define VOLUME_MAX		0
#define VOLUME_STEP		100
#define VOLUME_TICKS	(((VOLUME_MAX - VOLUME_MIN) / VOLUME_STEP) + 1)
LONG music_volume(LONG lVolume);
LONG sound_volume(LONG lVolume);



#endif
