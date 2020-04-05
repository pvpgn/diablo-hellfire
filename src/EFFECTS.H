/*-----------------------------------------------------------------------**
** 								Diablo
**
**						Constants and Variables
**
** 				(C)1995 Condor, Inc. All rights reserved.
**-----------------------------------------------------------------------**
**	$Header: /Diablo/EFFECTS.H 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------*/

//******************************************************************
// Defines
//******************************************************************
// monster sfx
#define MS_ATTACK	0
#define MS_GOTHIT	1
#define MS_DEATH	2
#define MS_SATTACK	3
#define MAX_MS		4


//******************************************************************
// externs
//******************************************************************
extern sfxdelay, sfxdnum;


//******************************************************************
// public functions
//******************************************************************
void sound_init();
void sound_exit();
void sound_update();
void sound_stop();
void stream_stop();

void PlrTryHitSnd();
void PlaySFX(int);
void PlaySfxLoc(int psfx, int x, int y);

void InitMonsterSND();
void PlayEffect(int, int);
void FreeMonsterSnd();


//******************************************************************
// sound effects data
//******************************************************************
#ifdef EFFECTS_DATA
	#define SFX(a,b,c)	{ a,				b, NULL }
	#define rSFX(a,b,c)	{ a | sfx_ROGUE,	b, NULL }
	#define wSFX(a,b,c)	{ a | sfx_WARRIOR,	b, NULL }
	#define sSFX(a,b,c)	{ a | sfx_SORCEROR,	b, NULL }
	#define mSFX(a,b,c)	{ a | sfx_MONK,		b, NULL }
#else
	#define SFX(a,b,c)		c
	#define rSFX(a,b,c)		c
	#define wSFX(a,b,c)		c
	#define sSFX(a,b,c)		c
	#define mSFX(a,b,c)		c
#endif


// build data table or header enum
#ifdef EFFECTS_DATA
static TSFX sgSFX[] = {
#else
enum {
#endif
// NOTE NOTE NOTE NOTE NOTE
// do not put any sound effects before these effects
// DirectSound loads sound effects into hardware on
// a first-come, first-served basis, so we want the
// most common sounds to come first so they end up
// in hardware.
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Walk1.wav",			PS_WALK1),	
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Walk2.wav",			PS_WALK2),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Walk3.wav",			PS_WALK3),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Walk4.wav",			PS_WALK4),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\BFire.wav",			PS_BFIRE),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Fmag.wav",			PS_FMAG),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Tmag.wav",			PS_TMAG),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Lghit.wav",			PS_LGHIT),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Lghit1.wav",		PS_LGHIT1),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Swing.wav",			PS_SWING),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Swing2.wav",		PS_SWING2),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Dead.wav",			PS_DEAD),

//	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\NewBFire.wav",		PS_NEW_BFIRE),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Sting1.wav",		PS_NEW_BFIRE),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\FBallBow.wav",		PS_FB_BFIRE),
	// Quests SoundEffects
	SFX(sfx_STREAM,			"Sfx\\Misc\\Questdon.wav",		IS_QUESTDN),

	// Item SoundEffects
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Armrfkd.wav",		IS_ARMRFKD),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Barlfire.wav",		IS_BARLFIRE),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Barrel.wav",		IS_BARREL),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\PodPop8.wav",		IS_PODFIRE),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\PodPop5.wav",		IS_POD),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\UrnPop3.wav",		IS_URNFIRE),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\UrnPop2.wav",		IS_URN),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Bhit.wav",			IS_BHIT),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Bhit1.wav",		IS_BHIT1),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Chest.wav",		IS_CHEST),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Doorclos.wav",		IS_DOORCLOS),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Dooropen.wav",		IS_DOOROPEN),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Flipanvl.wav",		IS_FANVL),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Flipaxe.wav",		IS_FAXE),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Flipblst.wav",		IS_FBLST),  // ear
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Flipbody.wav",		IS_FBODY),  // ear
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Flipbook.wav",		IS_FBOOK),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Flipbow.wav",		IS_FBOW),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Flipcap.wav",		IS_FCAP),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Flipharm.wav",		IS_FHARM),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Fliplarm.wav",		IS_FLARM),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Flipmag.wav",		IS_FMAG),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Flipmag1.wav",		IS_FMAG1),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Flipmush.wav",		IS_FMUSH),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Flippot.wav",		IS_FPOT),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Flipring.wav",		IS_FRING),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Fliprock.wav",		IS_FROCK),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Flipscrl.wav",		IS_FSCRL),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Flipshld.wav",		IS_FSHLD),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Flipsign.wav",		IS_FSIGN),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Flipstaf.wav",		IS_FSTAF),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Flipswor.wav",		IS_FSWOR),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Gold.wav",			IS_GOLD),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Hlmtfkd.wav",		IS_HLMTFKD),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Invanvl.wav",		IS_IANVL),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Invaxe.wav",		IS_IAXE),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Invblst.wav",		IS_IBLST),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Invbody.wav",		IS_IBODY),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Invbook.wav",		IS_IBOOK),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Invbow.wav",		IS_IBOW),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Invcap.wav",		IS_ICAP),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Invgrab.wav",		IS_IGRAB),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Invharm.wav",		IS_IHARM),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Invlarm.wav",		IS_ILARM),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Invmush.wav",		IS_IMUSH),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Invpot.wav",		IS_IPOT),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Invring.wav",		IS_IRING),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Invrock.wav",		IS_IROCK),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Invscrol.wav",		IS_ISCROL),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Invshiel.wav",		IS_ISHIEL),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Invsign.wav",		IS_ISIGN),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Invstaf.wav",		IS_ISTAF),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Invsword.wav",		IS_ISWORD),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Lever.wav",		IS_LEVER),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Magic.wav",		IS_MAGIC),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Magic1.wav",		IS_MAGIC1),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Readbook.wav",		IS_RBOOK),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Sarc.wav",			IS_SARC),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Shielfkd.wav",		IS_SHLDFKD),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Swrdfkd.wav",		IS_SWRDFKD),
	SFX(sfx_MENU,			"Sfx\\Items\\Titlemov.wav",		IS_TITLEMOV),
	SFX(sfx_MENU,			"Sfx\\Items\\Titlslct.wav",		IS_TITLSLCT),
	SFX(sfx_MENU,			"Sfx\\Misc\\blank.wav",			SFX_SILENCE),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Trap.wav",			IS_TRAP),

	// Cast
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Cast1.wav",			IS_CAST1),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Cast10.wav",		IS_CAST10),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Cast12.wav",		IS_CAST12),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Cast2.wav",			IS_CAST2),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Cast3.wav",			IS_CAST3),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Cast4.wav",			IS_CAST4),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Cast5.wav",			IS_CAST5),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Cast6.wav",			IS_CAST6),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Cast7.wav",			IS_CAST7),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Cast8.wav",			IS_CAST8),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Cast9.wav",			IS_CAST9),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Healing.wav",		LS_HEALING),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Repair.wav",		IS_REPAIR),

	// Launch and impact sounds
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Acids1.wav",		LS_ACID),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Acids2.wav",		LS_ACIDS),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Apoc.wav",			LS_APOC),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Arrowall.wav",		LS_ARROWALL),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Bldboil.wav",		LS_BLODBOIL),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Blodstar.wav",		LS_BLODSTAR),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Blsimpt.wav",		LS_BLSIMPT),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Bonesp.wav",		LS_BONESP),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Bsimpct.wav",		LS_BSIMPCT),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Caldron.wav",		LS_CALDRON),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Cbolt.wav",			LS_CBOLT),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Chltning.wav",		LS_CHLTNING),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\DSerp.wav",			LS_DSERP),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Elecimp1.wav",		LS_ELECIMP1),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Elementl.wav",		LS_ELEMENTL),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Ethereal.wav",		LS_ETHEREAL),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Fball.wav",			LS_FBALL),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Fbolt1.wav",		LS_FBOLT1),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Fbolt2.wav",		LS_FBOLT2),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Firimp1.wav",		LS_FIRIMP1),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Firimp2.wav",		LS_FIRIMP2),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Flamwave.wav",		LS_FLAMWAVE),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Flash.wav",			LS_FLASH),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Fountain.wav",		LS_FOUNTAIN),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Golum.wav",			LS_GOLUM),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Golumded.wav",		LS_GOLUMDED),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Gshrine.wav",		LS_GSHRINE),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Guard.wav",			LS_GUARD),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Grdlanch.wav",		LS_GUARDLAN),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Holybolt.wav",		LS_HOLYBOLT),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Hyper.wav",			LS_HYPER),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Infravis.wav",		LS_INFRAVIS),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Invisibl.wav",		LS_INVISIBL),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Invpot.wav",		LS_INVPOT),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Lning1.wav",		LS_LNING1),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Ltning.wav",		LS_LTNING),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Mshield.wav",		LS_MSHIELD),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\NestXpld.wav",		LS_BIGEXP),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Nova.wav",			LS_NOVA),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Portal.wav",		LS_PORTAL),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Puddle.wav",		LS_PUDDLE),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Resur.wav",			LS_RESUR),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Scurse.wav",		LS_SCURSE),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Scurimp.wav",		LS_SCURIMP),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Sentinel.wav",		LS_SENTINEL),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Shatter.wav",		LS_SHATTER),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Soulfire.wav",		LS_SOULFIRE),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Spoutlop.wav",		LS_SPOUTLOP),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Spoutstr.wav",		LS_SPOUTSTR),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Storm.wav",			LS_STORM),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Trapdis.wav",		LS_TRAPDIS),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Teleport.wav",		LS_TELEPORT),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Vtheft.wav",		LS_VTHEFT),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Wallloop.wav",		LS_WALLLOOP),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\Wallstrt.wav",		LS_WALLSTRT),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Misc\\LMag.wav",			LS_LMAG),

	// town sounds
#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid01.wav",	TSFX_BMAID1),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid02.wav",	TSFX_BMAID2),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid03.wav",	TSFX_BMAID3),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid04.wav",	TSFX_BMAID4),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid05.wav",	TSFX_BMAID5),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid06.wav",	TSFX_BMAID6),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid07.wav",	TSFX_BMAID7),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid08.wav",	TSFX_BMAID8),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid09.wav",	TSFX_BMAID9),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid10.wav",	TSFX_BMAID10),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid11.wav",	TSFX_BMAID11),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid12.wav",	TSFX_BMAID12),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid13.wav",	TSFX_BMAID13),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid14.wav",	TSFX_BMAID14),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid15.wav",	TSFX_BMAID15),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid16.wav",	TSFX_BMAID16),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid17.wav",	TSFX_BMAID17),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid18.wav",	TSFX_BMAID18),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid19.wav",	TSFX_BMAID19),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid20.wav",	TSFX_BMAID20),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid21.wav",	TSFX_BMAID21),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid22.wav",	TSFX_BMAID22),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid23.wav",	TSFX_BMAID23),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid24.wav",	TSFX_BMAID24),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid25.wav",	TSFX_BMAID25),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid26.wav",	TSFX_BMAID26),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid27.wav",	TSFX_BMAID27),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid28.wav",	TSFX_BMAID28),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid29.wav",	TSFX_BMAID29),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid30.wav",	TSFX_BMAID30),
#endif
	// greeting -- good day...
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid31.wav",	TSFX_BMAID31),
#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid32.wav",	TSFX_BMAID32),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid33.wav",	TSFX_BMAID33),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid34.wav",	TSFX_BMAID34),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid35.wav",	TSFX_BMAID35),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid36.wav",	TSFX_BMAID36),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid37.wav",	TSFX_BMAID37),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid38.wav",	TSFX_BMAID38),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid39.wav",	TSFX_BMAID39),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bmaid40.wav",	TSFX_BMAID40),
#endif

	// BlackSmith
#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith01.wav",	TSFX_SMITH1),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith02.wav",	TSFX_SMITH2),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith03.wav",	TSFX_SMITH3),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith04.wav",	TSFX_SMITH4),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith05.wav",	TSFX_SMITH5),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith06.wav",	TSFX_SMITH6),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith07.wav",	TSFX_SMITH7),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith08.wav",	TSFX_SMITH8),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith09.wav",	TSFX_SMITH9),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith10.wav",	TSFX_SMITH10),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith11.wav",	TSFX_SMITH11),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith12.wav",	TSFX_SMITH12),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith13.wav",	TSFX_SMITH13),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith14.wav",	TSFX_SMITH14),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith15.wav",	TSFX_SMITH15),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith16.wav",	TSFX_SMITH16),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith17.wav",	TSFX_SMITH17),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith18.wav",	TSFX_SMITH18),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith19.wav",	TSFX_SMITH19),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith20.wav",	TSFX_SMITH20),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith21.wav",	TSFX_SMITH21),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith22.wav",	TSFX_SMITH22),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith23.wav",	TSFX_SMITH23),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith24.wav",	TSFX_SMITH24),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith25.wav",	TSFX_SMITH25),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith26.wav",	TSFX_SMITH26),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith27.wav",	TSFX_SMITH27),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith28.wav",	TSFX_SMITH28),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith29.wav",	TSFX_SMITH29),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith30.wav",	TSFX_SMITH30),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith31.wav",	TSFX_SMITH31),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith32.wav",	TSFX_SMITH32),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith33.wav",	TSFX_SMITH33),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith34.wav",	TSFX_SMITH34),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith35.wav",	TSFX_SMITH35),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith36.wav",	TSFX_SMITH36),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith37.wav",	TSFX_SMITH37),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith38.wav",	TSFX_SMITH38),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith39.wav",	TSFX_SMITH39),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith40.wav",	TSFX_SMITH40),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith41.wav",	TSFX_SMITH41),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith42.wav",	TSFX_SMITH42),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith43.wav",	TSFX_SMITH43),
#endif
	// greeting -- well, what can I do for you?
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith44.wav",	TSFX_SMITH44),
#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith45.wav",	TSFX_SMITH45),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith46.wav",	TSFX_SMITH46),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith47.wav",	TSFX_SMITH47),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith48.wav",	TSFX_SMITH48),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith49.wav",	TSFX_SMITH49),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith50.wav",	TSFX_SMITH50),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith51.wav",	TSFX_SMITH51),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith52.wav",	TSFX_SMITH52),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith53.wav",	TSFX_SMITH53),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith54.wav",	TSFX_SMITH54),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith55.wav",	TSFX_SMITH55),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Bsmith56.wav",	TSFX_SMITH56),
#endif

	SFX(0,					"Sfx\\Towners\\Cow1.wav",		TSFX_COW1),
	SFX(0,					"Sfx\\Towners\\Cow2.wav",		TSFX_COW2),

#if 0
	SFX(sfx_STREAM,			"Sfx\\Towners\\Cow3.wav",		TSFX_COW3),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Cow4.wav",		TSFX_COW4),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Cow5.wav",		TSFX_COW5),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Cow6.wav",		TSFX_COW6),
#endif
	SFX(sfx_STREAM,			"Sfx\\Towners\\Cow7.wav",		TSFX_COW7),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Cow8.wav",		TSFX_COW8),

#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Towners\\Deadguy2.wav",	TSFX_DEADGUY),
#endif
	
#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk01.wav",	TSFX_DRUNK1),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk02.wav",	TSFX_DRUNK2),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk03.wav",	TSFX_DRUNK3),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk04.wav",	TSFX_DRUNK4),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk05.wav",	TSFX_DRUNK5),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk06.wav",	TSFX_DRUNK6),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk07.wav",	TSFX_DRUNK7),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk08.wav",	TSFX_DRUNK8),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk09.wav",	TSFX_DRUNK9),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk10.wav",	TSFX_DRUNK10),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk11.wav",	TSFX_DRUNK11),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk12.wav",	TSFX_DRUNK12),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk13.wav",	TSFX_DRUNK13),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk14.wav",	TSFX_DRUNK14),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk15.wav",	TSFX_DRUNK15),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk16.wav",	TSFX_DRUNK16),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk17.wav",	TSFX_DRUNK17),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk18.wav",	TSFX_DRUNK18),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk19.wav",	TSFX_DRUNK19),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk20.wav",	TSFX_DRUNK20),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk21.wav",	TSFX_DRUNK21),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk22.wav",	TSFX_DRUNK22),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk23.wav",	TSFX_DRUNK23),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk24.wav",	TSFX_DRUNK24),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk25.wav",	TSFX_DRUNK25),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk26.wav",	TSFX_DRUNK26),
#endif
	// greeting -- can't a fellow drink in peace?
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk27.wav",	TSFX_DRUNK27),
#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk28.wav",	TSFX_DRUNK28),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk29.wav",	TSFX_DRUNK29),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk30.wav",	TSFX_DRUNK30),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk31.wav",	TSFX_DRUNK31),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk32.wav",	TSFX_DRUNK32),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk33.wav",	TSFX_DRUNK33),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk34.wav",	TSFX_DRUNK34),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Drunk35.wav",	TSFX_DRUNK35),
#endif

#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer01.wav",	TSFX_HEALER1),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer02.wav",	TSFX_HEALER2),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer03.wav",	TSFX_HEALER3),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer04.wav",	TSFX_HEALER4),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer05.wav",	TSFX_HEALER5),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer06.wav",	TSFX_HEALER6),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer07.wav",	TSFX_HEALER7),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer08.wav",	TSFX_HEALER8),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer09.wav",	TSFX_HEALER9),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer10.wav",	TSFX_HEALER10),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer11.wav",	TSFX_HEALER11),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer12.wav",	TSFX_HEALER12),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer13.wav",	TSFX_HEALER13),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer14.wav",	TSFX_HEALER14),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer15.wav",	TSFX_HEALER15),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer16.wav",	TSFX_HEALER16),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer17.wav",	TSFX_HEALER17),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer18.wav",	TSFX_HEALER18),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer19.wav",	TSFX_HEALER19),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer20.wav",	TSFX_HEALER20),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer21.wav",	TSFX_HEALER21),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer22.wav",	TSFX_HEALER22),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer23.wav",	TSFX_HEALER23),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer24.wav",	TSFX_HEALER24),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer25.wav",	TSFX_HEALER25),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer26.wav",	TSFX_HEALER26),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer27.wav",	TSFX_HEALER27),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer28.wav",	TSFX_HEALER28),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer29.wav",	TSFX_HEALER29),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer30.wav",	TSFX_HEALER30),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer31.wav",	TSFX_HEALER31),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer32.wav",	TSFX_HEALER32),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer33.wav",	TSFX_HEALER33),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer34.wav",	TSFX_HEALER34),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer35.wav",	TSFX_HEALER35),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer36.wav",	TSFX_HEALER36),
#endif
	// greeting -- what ails you my friend?
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer37.wav",	TSFX_HEALER37),
#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer38.wav",	TSFX_HEALER38),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer39.wav",	TSFX_HEALER39),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer40.wav",	TSFX_HEALER40),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer41.wav",	TSFX_HEALER41),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer42.wav",	TSFX_HEALER42),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer43.wav",	TSFX_HEALER43),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer44.wav",	TSFX_HEALER44),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer45.wav",	TSFX_HEALER45),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer46.wav",	TSFX_HEALER46),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Healer47.wav",	TSFX_HEALER47),
#endif

#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy01.wav",	TSFX_PEGBOY1),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy02.wav",	TSFX_PEGBOY2),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy03.wav",	TSFX_PEGBOY3),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy04.wav",	TSFX_PEGBOY4),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy05.wav",	TSFX_PEGBOY5),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy06.wav",	TSFX_PEGBOY6),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy07.wav",	TSFX_PEGBOY7),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy08.wav",	TSFX_PEGBOY8),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy09.wav",	TSFX_PEGBOY9),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy10.wav",	TSFX_PEGBOY10),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy11.wav",	TSFX_PEGBOY11),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy12.wav",	TSFX_PEGBOY12),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy13.wav",	TSFX_PEGBOY13),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy14.wav",	TSFX_PEGBOY14),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy15.wav",	TSFX_PEGBOY15),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy16.wav",	TSFX_PEGBOY16),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy17.wav",	TSFX_PEGBOY17),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy18.wav",	TSFX_PEGBOY18),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy19.wav",	TSFX_PEGBOY19),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy20.wav",	TSFX_PEGBOY20),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy21.wav",	TSFX_PEGBOY21),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy22.wav",	TSFX_PEGBOY22),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy23.wav",	TSFX_PEGBOY23),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy24.wav",	TSFX_PEGBOY24),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy25.wav",	TSFX_PEGBOY25),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy26.wav",	TSFX_PEGBOY26),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy27.wav",	TSFX_PEGBOY27),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy28.wav",	TSFX_PEGBOY28),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy29.wav",	TSFX_PEGBOY29),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy30.wav",	TSFX_PEGBOY30),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy31.wav",	TSFX_PEGBOY31),
#endif
	// greeting -- psst, over here...
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy32.wav",	TSFX_PEGBOY32),
#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy33.wav",	TSFX_PEGBOY33),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy34.wav",	TSFX_PEGBOY34),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy35.wav",	TSFX_PEGBOY35),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy36.wav",	TSFX_PEGBOY36),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy37.wav",	TSFX_PEGBOY37),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy38.wav",	TSFX_PEGBOY38),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy39.wav",	TSFX_PEGBOY39),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy40.wav",	TSFX_PEGBOY40),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy41.wav",	TSFX_PEGBOY41),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy42.wav",	TSFX_PEGBOY42),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Pegboy43.wav",	TSFX_PEGBOY43),
#endif

#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Towners\\Priest00.wav",	TSFX_PRIEST0),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Priest01.wav",	TSFX_PRIEST1),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Priest02.wav",	TSFX_PRIEST2),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Priest03.wav",	TSFX_PRIEST3),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Priest04.wav",	TSFX_PRIEST4),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Priest05.wav",	TSFX_PRIEST5),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Priest06.wav",	TSFX_PRIEST6),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Priest07.wav",	TSFX_PRIEST7),
#endif

#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt00.wav",	TSFX_STORY0),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt01.wav",	TSFX_STORY1),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt02.wav",	TSFX_STORY2),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt03.wav",	TSFX_STORY3),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt04.wav",	TSFX_STORY4),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt05.wav",	TSFX_STORY5),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt06.wav",	TSFX_STORY6),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt07.wav",	TSFX_STORY7),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt08.wav",	TSFX_STORY8),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt09.wav",	TSFX_STORY9),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt10.wav",	TSFX_STORY10),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt11.wav",	TSFX_STORY11),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt12.wav",	TSFX_STORY12),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt13.wav",	TSFX_STORY13),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt14.wav",	TSFX_STORY14),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt15.wav",	TSFX_STORY15),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt16.wav",	TSFX_STORY16),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt17.wav",	TSFX_STORY17),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt18.wav",	TSFX_STORY18),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt19.wav",	TSFX_STORY19),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt20.wav",	TSFX_STORY20),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt21.wav",	TSFX_STORY21),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt22.wav",	TSFX_STORY22),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt23.wav",	TSFX_STORY23),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt24.wav",	TSFX_STORY24),
#endif
	// greeting -- hello my friend...
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt25.wav",	TSFX_STORY25),
#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt26.wav",	TSFX_STORY26),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt27.wav",	TSFX_STORY27),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt28.wav",	TSFX_STORY28),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt29.wav",	TSFX_STORY29),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt30.wav",	TSFX_STORY30),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt31.wav",	TSFX_STORY31),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt32.wav",	TSFX_STORY32),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt33.wav",	TSFX_STORY33),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt34.wav",	TSFX_STORY34),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt35.wav",	TSFX_STORY35),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt36.wav",	TSFX_STORY36),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt37.wav",	TSFX_STORY37),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Storyt38.wav",	TSFX_STORY38),
#endif

	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown00.wav",	TSFX_TAVERN0),
#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown01.wav",	TSFX_TAVERN1),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown02.wav",	TSFX_TAVERN2),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown03.wav",	TSFX_TAVERN3),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown04.wav",	TSFX_TAVERN4),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown05.wav",	TSFX_TAVERN5),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown06.wav",	TSFX_TAVERN6),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown07.wav",	TSFX_TAVERN7),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown08.wav",	TSFX_TAVERN8),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown09.wav",	TSFX_TAVERN9),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown10.wav",	TSFX_TAVERN10),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown11.wav",	TSFX_TAVERN11),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown12.wav",	TSFX_TAVERN12),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown13.wav",	TSFX_TAVERN13),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown14.wav",	TSFX_TAVERN14),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown15.wav",	TSFX_TAVERN15),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown16.wav",	TSFX_TAVERN16),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown17.wav",	TSFX_TAVERN17),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown18.wav",	TSFX_TAVERN18),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown19.wav",	TSFX_TAVERN19),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown20.wav",	TSFX_TAVERN20),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown21.wav",	TSFX_TAVERN21),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown22.wav",	TSFX_TAVERN22),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown23.wav",	TSFX_TAVERN23),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown24.wav",	TSFX_TAVERN24),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown25.wav",	TSFX_TAVERN25),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown26.wav",	TSFX_TAVERN26),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown27.wav",	TSFX_TAVERN27),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown28.wav",	TSFX_TAVERN28),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown29.wav",	TSFX_TAVERN29),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown30.wav",	TSFX_TAVERN30),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown31.wav",	TSFX_TAVERN31),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown32.wav",	TSFX_TAVERN32),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown33.wav",	TSFX_TAVERN33),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown34.wav",	TSFX_TAVERN34),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown35.wav",	TSFX_TAVERN35),
#endif
	// greeting -- greetings good master...
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown36.wav",	TSFX_TAVERN36),
#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown37.wav",	TSFX_TAVERN37),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown38.wav",	TSFX_TAVERN38),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown39.wav",	TSFX_TAVERN39),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown40.wav",	TSFX_TAVERN40),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown41.wav",	TSFX_TAVERN41),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown42.wav",	TSFX_TAVERN42),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown43.wav",	TSFX_TAVERN43),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown44.wav",	TSFX_TAVERN44),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Tavown45.wav",	TSFX_TAVERN45),
#endif

#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch01.wav",	TSFX_WITCH1),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch02.wav",	TSFX_WITCH2),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch03.wav",	TSFX_WITCH3),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch04.wav",	TSFX_WITCH4),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch05.wav",	TSFX_WITCH5),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch06.wav",	TSFX_WITCH6),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch07.wav",	TSFX_WITCH7),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch08.wav",	TSFX_WITCH8),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch09.wav",	TSFX_WITCH9),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch10.wav",	TSFX_WITCH10),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch11.wav",	TSFX_WITCH11),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch12.wav",	TSFX_WITCH12),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch13.wav",	TSFX_WITCH13),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch14.wav",	TSFX_WITCH14),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch15.wav",	TSFX_WITCH15),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch16.wav",	TSFX_WITCH16),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch17.wav",	TSFX_WITCH17),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch18.wav",	TSFX_WITCH18),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch19.wav",	TSFX_WITCH19),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch20.wav",	TSFX_WITCH20),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch21.wav",	TSFX_WITCH21),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch22.wav",	TSFX_WITCH22),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch23.wav",	TSFX_WITCH23),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch24.wav",	TSFX_WITCH24),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch25.wav",	TSFX_WITCH25),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch26.wav",	TSFX_WITCH26),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch27.wav",	TSFX_WITCH27),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch28.wav",	TSFX_WITCH28),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch29.wav",	TSFX_WITCH29),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch30.wav",	TSFX_WITCH30),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch31.wav",	TSFX_WITCH31),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch32.wav",	TSFX_WITCH32),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch33.wav",	TSFX_WITCH33),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch34.wav",	TSFX_WITCH34),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch35.wav",	TSFX_WITCH35),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch36.wav",	TSFX_WITCH36),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch37.wav",	TSFX_WITCH37),
#endif
	// greeting -- I sense a soul in search of answers
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch38.wav",	TSFX_WITCH38),
#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch39.wav",	TSFX_WITCH39),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch40.wav",	TSFX_WITCH40),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch41.wav",	TSFX_WITCH41),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch42.wav",	TSFX_WITCH42),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch43.wav",	TSFX_WITCH43),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch44.wav",	TSFX_WITCH44),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch45.wav",	TSFX_WITCH45),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch46.wav",	TSFX_WITCH46),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch47.wav",	TSFX_WITCH47),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch48.wav",	TSFX_WITCH48),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch49.wav",	TSFX_WITCH49),
	SFX(sfx_STREAM,			"Sfx\\Towners\\Witch50.wav",	TSFX_WITCH50),
#endif

#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Towners\\Wound01.wav",	TSFX_WOUND),
#endif

	// Mage
#if !IS_VERSION(SHAREWARE)
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage01.wav",	PS_MAGE1),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage02.wav",	PS_MAGE2),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage03.wav",	PS_MAGE3),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage04.wav",	PS_MAGE4),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage05.wav",	PS_MAGE5),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage06.wav",	PS_MAGE6),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage07.wav",	PS_MAGE7),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage08.wav",	PS_MAGE8),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage09.wav",	PS_MAGE9),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage10.wav",	PS_MAGE10),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage11.wav",	PS_MAGE11),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage12.wav",	PS_MAGE12),
	sSFX(0,					"Sfx\\Sorceror\\Mage13.wav",	PS_MAGE13),
	sSFX(0,					"Sfx\\Sorceror\\Mage14.wav",	PS_MAGE14),
	sSFX(0,					"Sfx\\Sorceror\\Mage15.wav",	PS_MAGE15),
	sSFX(0,					"Sfx\\Sorceror\\Mage16.wav",	PS_MAGE16),
	sSFX(0,					"Sfx\\Sorceror\\Mage17.wav",	PS_MAGE17),
	sSFX(0,					"Sfx\\Sorceror\\Mage18.wav",	PS_MAGE18),
	sSFX(0,					"Sfx\\Sorceror\\Mage19.wav",	PS_MAGE19),
	sSFX(0,					"Sfx\\Sorceror\\Mage20.wav",	PS_MAGE20),
	sSFX(0,					"Sfx\\Sorceror\\Mage21.wav",	PS_MAGE21),
	sSFX(0,					"Sfx\\Sorceror\\Mage22.wav",	PS_MAGE22),
	sSFX(0,					"Sfx\\Sorceror\\Mage23.wav",	PS_MAGE23),
	sSFX(0,					"Sfx\\Sorceror\\Mage24.wav",	PS_MAGE24),
	sSFX(0,					"Sfx\\Sorceror\\Mage25.wav",	PS_MAGE25),
	sSFX(0,					"Sfx\\Sorceror\\Mage26.wav",	PS_MAGE26),
	sSFX(0,					"Sfx\\Sorceror\\Mage27.wav",	PS_MAGE27),
	sSFX(0,					"Sfx\\Sorceror\\Mage28.wav",	PS_MAGE28),
	sSFX(0,					"Sfx\\Sorceror\\Mage29.wav",	PS_MAGE29),
	sSFX(0,					"Sfx\\Sorceror\\Mage30.wav",	PS_MAGE30),
	sSFX(0,					"Sfx\\Sorceror\\Mage31.wav",	PS_MAGE31),
	sSFX(0,					"Sfx\\Sorceror\\Mage32.wav",	PS_MAGE32),
	sSFX(0,					"Sfx\\Sorceror\\Mage33.wav",	PS_MAGE33),
	sSFX(0,					"Sfx\\Sorceror\\Mage34.wav",	PS_MAGE34),
	sSFX(0,					"Sfx\\Sorceror\\Mage35.wav",	PS_MAGE35),
	sSFX(0,					"Sfx\\Sorceror\\Mage36.wav",	PS_MAGE36),
	sSFX(0,					"Sfx\\Sorceror\\Mage37.wav",	PS_MAGE37),
	sSFX(0,					"Sfx\\Sorceror\\Mage38.wav",	PS_MAGE38),
	sSFX(0,					"Sfx\\Sorceror\\Mage39.wav",	PS_MAGE39),
	sSFX(0,					"Sfx\\Sorceror\\Mage40.wav",	PS_MAGE40),
	sSFX(0,					"Sfx\\Sorceror\\Mage41.wav",	PS_MAGE41),
	sSFX(0,					"Sfx\\Sorceror\\Mage42.wav",	PS_MAGE42),
	sSFX(0,					"Sfx\\Sorceror\\Mage43.wav",	PS_MAGE43),
	sSFX(0,					"Sfx\\Sorceror\\Mage44.wav",	PS_MAGE44),
	sSFX(0,					"Sfx\\Sorceror\\Mage45.wav",	PS_MAGE45),
	sSFX(0,					"Sfx\\Sorceror\\Mage46.wav",	PS_MAGE46),
	sSFX(0,					"Sfx\\Sorceror\\Mage47.wav",	PS_MAGE47),
	sSFX(0,					"Sfx\\Sorceror\\Mage48.wav",	PS_MAGE48),
	sSFX(0,					"Sfx\\Sorceror\\Mage49.wav",	PS_MAGE49),
	sSFX(0,					"Sfx\\Sorceror\\Mage50.wav",	PS_MAGE50),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage51.wav",	PS_MAGE51),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage52.wav",	PS_MAGE52),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage53.wav",	PS_MAGE53),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage54.wav",	PS_MAGE54),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage55.wav",	PS_MAGE55),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage56.wav",	PS_MAGE56),
	sSFX(0,					"Sfx\\Sorceror\\Mage57.wav",	PS_MAGE57),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage58.wav",	PS_MAGE58),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage59.wav",	PS_MAGE59),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage60.wav",	PS_MAGE60),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage61.wav",	PS_MAGE61),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage62.wav",	PS_MAGE62),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage63.wav",	PS_MAGE63),
	sSFX(0,					"Sfx\\Sorceror\\Mage64.wav",	PS_MAGE64),
	sSFX(0,					"Sfx\\Sorceror\\Mage65.wav",	PS_MAGE65),
	sSFX(0,					"Sfx\\Sorceror\\Mage66.wav",	PS_MAGE66),
	sSFX(0,					"Sfx\\Sorceror\\Mage67.wav",	PS_MAGE67),
	sSFX(0,					"Sfx\\Sorceror\\Mage68.wav",	PS_MAGE68),
	sSFX(0,					"Sfx\\Sorceror\\Mage69.wav",	PS_MAGE69),
	sSFX(0,					"Sfx\\Sorceror\\Mage69b.wav",	PS_MAGE69B),
	sSFX(0,					"Sfx\\Sorceror\\Mage70.wav",	PS_MAGE70),
	sSFX(0,					"Sfx\\Sorceror\\Mage71.wav",	PS_MAGE71),
	sSFX(0,					"Sfx\\Sorceror\\Mage72.wav",	PS_MAGE72),
	sSFX(0,					"Sfx\\Sorceror\\Mage73.wav",	PS_MAGE73),
	sSFX(0,					"Sfx\\Sorceror\\Mage74.wav",	PS_MAGE74),
	sSFX(0,					"Sfx\\Sorceror\\Mage75.wav",	PS_MAGE75),
	sSFX(0,					"Sfx\\Sorceror\\Mage76.wav",	PS_MAGE76),
	sSFX(0,					"Sfx\\Sorceror\\Mage77.wav",	PS_MAGE77),
	sSFX(0,					"Sfx\\Sorceror\\Mage78.wav",	PS_MAGE78),
	sSFX(0,					"Sfx\\Sorceror\\Mage79.wav",	PS_MAGE79),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage80.wav",	PS_MAGE80),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage81.wav",	PS_MAGE81),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage82.wav",	PS_MAGE82),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage83.wav",	PS_MAGE83),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage84.wav",	PS_MAGE84),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage85.wav",	PS_MAGE85),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage86.wav",	PS_MAGE86),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage87.wav",	PS_MAGE87),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage88.wav",	PS_MAGE88),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage89.wav",	PS_MAGE89),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage90.wav",	PS_MAGE90),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage91.wav",	PS_MAGE91),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage92.wav",	PS_MAGE92),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage93.wav",	PS_MAGE93),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage94.wav",	PS_MAGE94),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage95.wav",	PS_MAGE95),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage96.wav",	PS_MAGE96),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage97.wav",	PS_MAGE97),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage98.wav",	PS_MAGE98),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage99.wav",	PS_MAGE99),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage100.wav",	PS_MAGE100),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage101.wav",	PS_MAGE101),
	sSFX(sfx_STREAM,		"Sfx\\Sorceror\\Mage102.wav",	PS_MAGE102),
#endif

	// Rogue
#if !IS_VERSION(SHAREWARE)
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue01.wav",		PS_ROGUE1),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue02.wav",		PS_ROGUE2),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue03.wav",		PS_ROGUE3),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue04.wav",		PS_ROGUE4),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue05.wav",		PS_ROGUE5),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue06.wav",		PS_ROGUE6),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue07.wav",		PS_ROGUE7),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue08.wav",		PS_ROGUE8),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue09.wav",		PS_ROGUE9),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue10.wav",		PS_ROGUE10),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue11.wav",		PS_ROGUE11),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue12.wav",		PS_ROGUE12),
	rSFX(0,					"Sfx\\Rogue\\Rogue13.wav",		PS_ROGUE13),
	rSFX(0,					"Sfx\\Rogue\\Rogue14.wav",		PS_ROGUE14),
	rSFX(0,					"Sfx\\Rogue\\Rogue15.wav",		PS_ROGUE15),
	rSFX(0,					"Sfx\\Rogue\\Rogue16.wav",		PS_ROGUE16),
	rSFX(0,					"Sfx\\Rogue\\Rogue17.wav",		PS_ROGUE17),
	rSFX(0,					"Sfx\\Rogue\\Rogue18.wav",		PS_ROGUE18),
	rSFX(0,					"Sfx\\Rogue\\Rogue19.wav",		PS_ROGUE19),
	rSFX(0,					"Sfx\\Rogue\\Rogue20.wav",		PS_ROGUE20),
	rSFX(0,					"Sfx\\Rogue\\Rogue21.wav",		PS_ROGUE21),
	rSFX(0,					"Sfx\\Rogue\\Rogue22.wav",		PS_ROGUE22),
	rSFX(0,					"Sfx\\Rogue\\Rogue23.wav",		PS_ROGUE23),
	rSFX(0,					"Sfx\\Rogue\\Rogue24.wav",		PS_ROGUE24),
	rSFX(0,					"Sfx\\Rogue\\Rogue25.wav",		PS_ROGUE25),
	rSFX(0,					"Sfx\\Rogue\\Rogue26.wav",		PS_ROGUE26),
	rSFX(0,					"Sfx\\Rogue\\Rogue27.wav",		PS_ROGUE27),
	rSFX(0,					"Sfx\\Rogue\\Rogue28.wav",		PS_ROGUE28),
	rSFX(0,					"Sfx\\Rogue\\Rogue29.wav",		PS_ROGUE29),
	rSFX(0,					"Sfx\\Rogue\\Rogue30.wav",		PS_ROGUE30),
	rSFX(0,					"Sfx\\Rogue\\Rogue31.wav",		PS_ROGUE31),
	rSFX(0,					"Sfx\\Rogue\\Rogue32.wav",		PS_ROGUE32),
	rSFX(0,					"Sfx\\Rogue\\Rogue33.wav",		PS_ROGUE33),
	rSFX(0,					"Sfx\\Rogue\\Rogue34.wav",		PS_ROGUE34),
	rSFX(0,					"Sfx\\Rogue\\Rogue35.wav",		PS_ROGUE35),
	rSFX(0,					"Sfx\\Rogue\\Rogue36.wav",		PS_ROGUE36),
	rSFX(0,					"Sfx\\Rogue\\Rogue37.wav",		PS_ROGUE37),
	rSFX(0,					"Sfx\\Rogue\\Rogue38.wav",		PS_ROGUE38),
	rSFX(0,					"Sfx\\Rogue\\Rogue39.wav",		PS_ROGUE39),
	rSFX(0,					"Sfx\\Rogue\\Rogue40.wav",		PS_ROGUE40),
	rSFX(0,					"Sfx\\Rogue\\Rogue41.wav",		PS_ROGUE41),
	rSFX(0,					"Sfx\\Rogue\\Rogue42.wav",		PS_ROGUE42),
	rSFX(0,					"Sfx\\Rogue\\Rogue43.wav",		PS_ROGUE43),
	rSFX(0,					"Sfx\\Rogue\\Rogue44.wav",		PS_ROGUE44),
	rSFX(0,					"Sfx\\Rogue\\Rogue45.wav",		PS_ROGUE45),
	rSFX(0,					"Sfx\\Rogue\\Rogue46.wav",		PS_ROGUE46),
	rSFX(0,					"Sfx\\Rogue\\Rogue47.wav",		PS_ROGUE47),
	rSFX(0,					"Sfx\\Rogue\\Rogue48.wav",		PS_ROGUE48),
	rSFX(0,					"Sfx\\Rogue\\Rogue49.wav",		PS_ROGUE49),
	rSFX(0,					"Sfx\\Rogue\\Rogue50.wav",		PS_ROGUE50),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue51.wav",		PS_ROGUE51),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue52.wav",		PS_ROGUE52),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue53.wav",		PS_ROGUE53),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue54.wav",		PS_ROGUE54),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue55.wav",		PS_ROGUE55),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue56.wav",		PS_ROGUE56),
	rSFX(0,					"Sfx\\Rogue\\Rogue57.wav",		PS_ROGUE57),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue58.wav",		PS_ROGUE58),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue59.wav",		PS_ROGUE59),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue60.wav",		PS_ROGUE60),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue61.wav",		PS_ROGUE61),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue62.wav",		PS_ROGUE62),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue63.wav",		PS_ROGUE63),
	rSFX(0,					"Sfx\\Rogue\\Rogue64.wav",		PS_ROGUE64),
	rSFX(0,					"Sfx\\Rogue\\Rogue65.wav",		PS_ROGUE65),
	rSFX(0,					"Sfx\\Rogue\\Rogue66.wav",		PS_ROGUE66),
	rSFX(0,					"Sfx\\Rogue\\Rogue67.wav",		PS_ROGUE67),
	rSFX(0,					"Sfx\\Rogue\\Rogue68.wav",		PS_ROGUE68),
	rSFX(0,					"Sfx\\Rogue\\Rogue69.wav",		PS_ROGUE69),
	rSFX(0,					"Sfx\\Rogue\\Rogue69b.wav",	PS_ROGUE69B),
	rSFX(0,					"Sfx\\Rogue\\Rogue70.wav",		PS_ROGUE70),
	rSFX(0,					"Sfx\\Rogue\\Rogue71.wav",		PS_ROGUE71),
	rSFX(0,					"Sfx\\Rogue\\Rogue72.wav",		PS_ROGUE72),
	rSFX(0,					"Sfx\\Rogue\\Rogue73.wav",		PS_ROGUE73),
	rSFX(0,					"Sfx\\Rogue\\Rogue74.wav",		PS_ROGUE74),
	rSFX(0,					"Sfx\\Rogue\\Rogue75.wav",		PS_ROGUE75),
	rSFX(0,					"Sfx\\Rogue\\Rogue76.wav",		PS_ROGUE76),
	rSFX(0,					"Sfx\\Rogue\\Rogue77.wav",		PS_ROGUE77),
	rSFX(0,					"Sfx\\Rogue\\Rogue78.wav",		PS_ROGUE78),
	rSFX(0,					"Sfx\\Rogue\\Rogue79.wav",		PS_ROGUE79),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue80.wav",		PS_ROGUE80),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue81.wav",		PS_ROGUE81),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue82.wav",		PS_ROGUE82),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue83.wav",		PS_ROGUE83),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue84.wav",		PS_ROGUE84),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue85.wav",		PS_ROGUE85),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue86.wav",		PS_ROGUE86),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue87.wav",		PS_ROGUE87),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue88.wav",		PS_ROGUE88),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue89.wav",		PS_ROGUE89),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue90.wav",		PS_ROGUE90),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue91.wav",		PS_ROGUE91),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue92.wav",		PS_ROGUE92),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue93.wav",		PS_ROGUE93),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue94.wav",		PS_ROGUE94),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue95.wav",		PS_ROGUE95),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue96.wav",		PS_ROGUE96),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue97.wav",		PS_ROGUE97),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue98.wav",		PS_ROGUE98),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue99.wav",		PS_ROGUE99),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue100.wav",	PS_ROGUE100),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue101.wav",	PS_ROGUE101),
	rSFX(sfx_STREAM,		"Sfx\\Rogue\\Rogue102.wav",	PS_ROGUE102),
#endif

	// Warrior
#if !IS_VERSION(SHAREWARE)
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior01.wav",	PS_WARR1),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior02.wav",	PS_WARR2),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior03.wav",	PS_WARR3),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior04.wav",	PS_WARR4),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior05.wav",	PS_WARR5),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior06.wav",	PS_WARR6),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior07.wav",	PS_WARR7),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior08.wav",	PS_WARR8),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior09.wav",	PS_WARR9),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior10.wav",	PS_WARR10),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior11.wav",	PS_WARR11),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior12.wav",	PS_WARR12),
#endif
	wSFX(0,					"Sfx\\Warrior\\Warior13.wav",	PS_WARR13),
	wSFX(0,					"Sfx\\Warrior\\Warior14.wav",	PS_WARR14),
	wSFX(0,					"Sfx\\Warrior\\Wario14b.wav",	PS_WARR14B),
	wSFX(0,					"Sfx\\Warrior\\Wario14c.wav",	PS_WARR14C),
	wSFX(0,					"Sfx\\Warrior\\Warior15.wav",	PS_WARR15),
	wSFX(0,					"Sfx\\Warrior\\Wario15b.wav",	PS_WARR15B),
	wSFX(0,					"Sfx\\Warrior\\Wario15c.wav",	PS_WARR15C),
	wSFX(0,					"Sfx\\Warrior\\Warior16.wav",	PS_WARR16),
	wSFX(0,					"Sfx\\Warrior\\Wario16b.wav",	PS_WARR16B),
	wSFX(0,					"Sfx\\Warrior\\Wario16c.wav",	PS_WARR16C),
	wSFX(0,					"Sfx\\Warrior\\Warior17.wav",	PS_WARR17),
	wSFX(0,					"Sfx\\Warrior\\Warior18.wav",	PS_WARR18),
	wSFX(0,					"Sfx\\Warrior\\Warior19.wav",	PS_WARR19),
	wSFX(0,					"Sfx\\Warrior\\Warior20.wav",	PS_WARR20),
	wSFX(0,					"Sfx\\Warrior\\Warior21.wav",	PS_WARR21),
	wSFX(0,					"Sfx\\Warrior\\Warior22.wav",	PS_WARR22),
	wSFX(0,					"Sfx\\Warrior\\Warior23.wav",	PS_WARR23),
	wSFX(0,					"Sfx\\Warrior\\Warior24.wav",	PS_WARR24),
	wSFX(0,					"Sfx\\Warrior\\Warior25.wav",	PS_WARR25),
	wSFX(0,					"Sfx\\Warrior\\Warior26.wav",	PS_WARR26),
	wSFX(0,					"Sfx\\Warrior\\Warior27.wav",	PS_WARR27),
	wSFX(0,					"Sfx\\Warrior\\Warior28.wav",	PS_WARR28),
	wSFX(0,					"Sfx\\Warrior\\Warior29.wav",	PS_WARR29),
	wSFX(0,					"Sfx\\Warrior\\Warior30.wav",	PS_WARR30),
	wSFX(0,					"Sfx\\Warrior\\Warior31.wav",	PS_WARR31),
	wSFX(0,					"Sfx\\Warrior\\Warior32.wav",	PS_WARR32),
	wSFX(0,					"Sfx\\Warrior\\Warior33.wav",	PS_WARR33),
	wSFX(0,					"Sfx\\Warrior\\Warior34.wav",	PS_WARR34),
	wSFX(0,					"Sfx\\Warrior\\Warior35.wav",	PS_WARR35),
	wSFX(0,					"Sfx\\Warrior\\Warior36.wav",	PS_WARR36),
	wSFX(0,					"Sfx\\Warrior\\Warior37.wav",	PS_WARR37),
	wSFX(0,					"Sfx\\Warrior\\Warior38.wav",	PS_WARR38),
	wSFX(0,					"Sfx\\Warrior\\Warior39.wav",	PS_WARR39),
	wSFX(0,					"Sfx\\Warrior\\Warior40.wav",	PS_WARR40),
	wSFX(0,					"Sfx\\Warrior\\Warior41.wav",	PS_WARR41),
	wSFX(0,					"Sfx\\Warrior\\Warior42.wav",	PS_WARR42),
	wSFX(0,					"Sfx\\Warrior\\Warior43.wav",	PS_WARR43),
	wSFX(0,					"Sfx\\Warrior\\Warior44.wav",	PS_WARR44),
	wSFX(0,					"Sfx\\Warrior\\Warior45.wav",	PS_WARR45),
	wSFX(0,					"Sfx\\Warrior\\Warior46.wav",	PS_WARR46),
	wSFX(0,					"Sfx\\Warrior\\Warior47.wav",	PS_WARR47),
	wSFX(0,					"Sfx\\Warrior\\Warior48.wav",	PS_WARR48),
	wSFX(0,					"Sfx\\Warrior\\Warior49.wav",	PS_WARR49),
	wSFX(0,					"Sfx\\Warrior\\Warior50.wav",	PS_WARR50),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior51.wav",	PS_WARR51),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior52.wav",	PS_WARR52),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior53.wav",	PS_WARR53),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior54.wav",	PS_WARR54),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior55.wav",	PS_WARR55),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior56.wav",	PS_WARR56),
	wSFX(0,					"Sfx\\Warrior\\Warior57.wav",	PS_WARR57),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior58.wav",	PS_WARR58),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior59.wav",	PS_WARR59),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior60.wav",	PS_WARR60),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior61.wav",	PS_WARR61),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior62.wav",	PS_WARR62),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior63.wav",	PS_WARR63),
	wSFX(0,					"Sfx\\Warrior\\Warior64.wav",	PS_WARR64),
	wSFX(0,					"Sfx\\Warrior\\Warior65.wav",	PS_WARR65),
	wSFX(0,					"Sfx\\Warrior\\Warior66.wav",	PS_WARR66),
	wSFX(0,					"Sfx\\Warrior\\Warior67.wav",	PS_WARR67),
	wSFX(0,					"Sfx\\Warrior\\Warior68.wav",	PS_WARR68),
	wSFX(0,					"Sfx\\Warrior\\Warior69.wav",	PS_WARR69),
	wSFX(0,					"Sfx\\Warrior\\Wario69b.wav",	PS_WARR69B),
	wSFX(0,					"Sfx\\Warrior\\Warior70.wav",	PS_WARR70),
	wSFX(0,					"Sfx\\Warrior\\Warior71.wav",	PS_WARR71),
	wSFX(0,					"Sfx\\Warrior\\Warior72.wav",	PS_WARR72),
	wSFX(0,					"Sfx\\Warrior\\Warior73.wav",	PS_WARR73),
	wSFX(0,					"Sfx\\Warrior\\Warior74.wav",	PS_WARR74),
	wSFX(0,					"Sfx\\Warrior\\Warior75.wav",	PS_WARR75),
	wSFX(0,					"Sfx\\Warrior\\Warior76.wav",	PS_WARR76),
	wSFX(0,					"Sfx\\Warrior\\Warior77.wav",	PS_WARR77),
	wSFX(0,					"Sfx\\Warrior\\Warior78.wav",	PS_WARR78),
	wSFX(0,					"Sfx\\Warrior\\Warior79.wav",	PS_WARR79),
#if !IS_VERSION(SHAREWARE)
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior80.wav",	PS_WARR80),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior81.wav",	PS_WARR81),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior82.wav",	PS_WARR82),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior83.wav",	PS_WARR83),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior84.wav",	PS_WARR84),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior85.wav",	PS_WARR85),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior86.wav",	PS_WARR86),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior87.wav",	PS_WARR87),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior88.wav",	PS_WARR88),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior89.wav",	PS_WARR89),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior90.wav",	PS_WARR90),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior91.wav",	PS_WARR91),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior92.wav",	PS_WARR92),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior93.wav",	PS_WARR93),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior94.wav",	PS_WARR94),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior95.wav",	PS_WARR95),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Wario95b.wav",	PS_WARR95B),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Wario95c.wav",	PS_WARR95C),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Wario95d.wav",	PS_WARR95D),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Wario95e.wav",	PS_WARR95E),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Wario95f.wav",	PS_WARR95F),
	// unused wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior96.wav",	PS_WARR96),
#endif
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Wario96b.wav",	PS_WARR96B),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Wario97.wav",	PS_WARR97),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Wario98.wav",	PS_WARR98),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Warior99.wav",	PS_WARR99),
#if !IS_VERSION(SHAREWARE)
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Wario100.wav",	PS_WARR100),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Wario101.wav",	PS_WARR101),
	wSFX(sfx_STREAM,		"Sfx\\Warrior\\Wario102.wav",	PS_WARR102),
#endif

//	Silence "Sfx\\Misc\\blank.wav"
	// GWP Fix this: New sounds!
	// Monk
#if !IS_VERSION(SHAREWARE)
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk01.wav",	PS_MONK1),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK2),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK3),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK4),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK5),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK6),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK7),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk08.wav",	PS_MONK8),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk09.wav",	PS_MONK9),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk10.wav",	PS_MONK10),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk11.wav",	PS_MONK11),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk12.wav",	PS_MONK12),
	mSFX(0,					"Sfx\\Monk\\Monk13.wav",	PS_MONK13),
	mSFX(0,					"Sfx\\Monk\\Monk14.wav",	PS_MONK14),
	mSFX(0,					"Sfx\\Monk\\Monk15.wav",	PS_MONK15),
	mSFX(0,					"Sfx\\Monk\\Monk16.wav",	PS_MONK16),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK17),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK18),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK19),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK20),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK21),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK22),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK23),
	mSFX(0,					"Sfx\\Monk\\Monk24.wav",	PS_MONK24),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK25),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK26),
	mSFX(0,					"Sfx\\Monk\\Monk27.wav",	PS_MONK27),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK28),
	mSFX(0,					"Sfx\\Monk\\Monk29.wav",	PS_MONK29),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK30),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK31),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK32),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK33),
	mSFX(0,					"Sfx\\Monk\\Monk34.wav",	PS_MONK34),
	mSFX(0,					"Sfx\\Monk\\Monk35.wav",	PS_MONK35),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK36),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK37),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK38),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK39),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK40),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK41),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK42),
	mSFX(0,					"Sfx\\Monk\\Monk43.wav",	PS_MONK43),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK44),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK45),
	mSFX(0,					"Sfx\\Monk\\Monk46.wav",	PS_MONK46),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK47),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK48),
	mSFX(0,					"Sfx\\Monk\\Monk49.wav",	PS_MONK49),
	mSFX(0,					"Sfx\\Monk\\Monk50.wav",	PS_MONK50),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK51),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk52.wav",	PS_MONK52),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK53),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk54.wav",	PS_MONK54),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk55.wav",	PS_MONK55),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk56.wav",	PS_MONK56),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK57),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK58),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK59),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK60),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk61.wav",	PS_MONK61),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk62.wav",	PS_MONK62),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK63),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK64),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK65),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK66),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK67),
	mSFX(0,					"Sfx\\Monk\\Monk68.wav",	PS_MONK68),
	mSFX(0,					"Sfx\\Monk\\Monk69.wav",	PS_MONK69),
	mSFX(0,					"Sfx\\Monk\\Monk69b.wav",	PS_MONK69B),
	mSFX(0,					"Sfx\\Monk\\Monk70.wav",	PS_MONK70),
	mSFX(0,					"Sfx\\Monk\\Monk71.wav",	PS_MONK71),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK72),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK73),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK74),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK75),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK76),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK77),
	mSFX(0,					"Sfx\\Misc\\blank.wav",	PS_MONK78),
	mSFX(0,					"Sfx\\Monk\\Monk79.wav",	PS_MONK79),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk80.wav",	PS_MONK80),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK81),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk82.wav",	PS_MONK82),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk83.wav",	PS_MONK83),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK84),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK85),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK86),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk87.wav",	PS_MONK87),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk88.wav",	PS_MONK88),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk89.wav",	PS_MONK89),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK90),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk91.wav",	PS_MONK91),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk92.wav",	PS_MONK92),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK93),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk94.wav",	PS_MONK94),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk95.wav",	PS_MONK95),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk96.wav",	PS_MONK96),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk97.wav",	PS_MONK97),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk98.wav",	PS_MONK98),
	mSFX(sfx_STREAM,		"Sfx\\Monk\\Monk99.wav",	PS_MONK99),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK100),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK101),
	mSFX(sfx_STREAM,		"Sfx\\Misc\\blank.wav",	PS_MONK102),
#endif
	
	// Narrator
#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Narrator\\Nar01.wav",	PS_NAR1),
	SFX(sfx_STREAM,			"Sfx\\Narrator\\Nar02.wav",	PS_NAR2),
	SFX(sfx_STREAM,			"Sfx\\Narrator\\Nar03.wav",	PS_NAR3),
	SFX(sfx_STREAM,			"Sfx\\Narrator\\Nar04.wav",	PS_NAR4),
	SFX(sfx_STREAM,			"Sfx\\Narrator\\Nar05.wav",	PS_NAR5),
	SFX(sfx_STREAM,			"Sfx\\Narrator\\Nar06.wav",	PS_NAR6),
	SFX(sfx_STREAM,			"Sfx\\Narrator\\Nar07.wav",	PS_NAR7),
	SFX(sfx_STREAM,			"Sfx\\Narrator\\Nar08.wav",	PS_NAR8),
	SFX(sfx_STREAM,			"Sfx\\Narrator\\Nar09.wav",	PS_NAR9),

	// Level 16 intro
	SFX(sfx_STREAM,			"Sfx\\Misc\\Lvl16int.wav",	PS_DIABLVLINT),

#endif

	// unique monster sounds
#if !IS_VERSION(SHAREWARE)
	SFX(sfx_STREAM,			"Sfx\\Monsters\\Butcher.wav",	USFX_CLEAVER),
	SFX(sfx_STREAM,			"Sfx\\Monsters\\Garbud01.wav",	USFX_GARBUD1),
	SFX(sfx_STREAM,			"Sfx\\Monsters\\Garbud02.wav",	USFX_GARBUD2),
	SFX(sfx_STREAM,			"Sfx\\Monsters\\Garbud03.wav",	USFX_GARBUD3),
	SFX(sfx_STREAM,			"Sfx\\Monsters\\Garbud04.wav",	USFX_GARBUD4),
	SFX(sfx_STREAM,			"Sfx\\Monsters\\Izual01.wav",	USFX_IZUAL1),
	SFX(sfx_STREAM,			"Sfx\\Monsters\\Lach01.wav",	USFX_LACH1),
	SFX(sfx_STREAM,			"Sfx\\Monsters\\Lach02.wav",	USFX_LACH2),
	SFX(sfx_STREAM,			"Sfx\\Monsters\\Lach03.wav",	USFX_LACH3),
	SFX(sfx_STREAM,			"Sfx\\Monsters\\Laz01.wav",		USFX_LAZ1),
	SFX(sfx_STREAM,			"Sfx\\Monsters\\Laz02.wav",		USFX_LAZ2),
	SFX(sfx_STREAM,			"Sfx\\Monsters\\Sking01.wav",	USFX_SKING1),
	SFX(sfx_STREAM,			"Sfx\\Monsters\\Snot01.wav",	USFX_SNOT1),
	SFX(sfx_STREAM,			"Sfx\\Monsters\\Snot02.wav",	USFX_SNOT2),
	SFX(sfx_STREAM,			"Sfx\\Monsters\\Snot03.wav",	USFX_SNOT3),
	SFX(sfx_STREAM,			"Sfx\\Monsters\\Warlrd01.wav",	USFX_WARLRD1),
	SFX(sfx_STREAM,			"Sfx\\Monsters\\Wlock01.wav",	USFX_WLOCK1),
	SFX(sfx_STREAM,			"Sfx\\Monsters\\Zhar01.wav",	USFX_ZHAR1),
	SFX(sfx_STREAM,			"Sfx\\Monsters\\Zhar02.wav",	USFX_ZHAR2),
	SFX(sfx_STREAM,			"Sfx\\Monsters\\DiabloD.wav",	USFX_DIABLOD),

// JKEQUEST	Enter new sound cues here
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\Farmer1.wav",	HSFX_FARMER1),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\Farmer2.wav",	HSFX_FARMER2),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\Farmer2A.wav",	HSFX_FARMER2A),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\Farmer3.wav",	HSFX_FARMER3),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\Farmer4.wav",	HSFX_FARMER4),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\Farmer5.wav",	HSFX_FARMER5),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\Farmer6.wav",	HSFX_FARMER6),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\Farmer7.wav",	HSFX_FARMER7),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\Farmer8.wav",	HSFX_FARMER8),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\Farmer9.wav",	HSFX_FARMER9),

	SFX(sfx_STREAM,			"Sfx\\Hellfire\\TEDDYBR1.wav",	HSFX_THEO1),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\TEDDYBR2.wav",	HSFX_THEO2),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\TEDDYBR3.wav",	HSFX_THEO3),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\TEDDYBR4.wav",	HSFX_THEO4),

	SFX(sfx_STREAM,			"Sfx\\Hellfire\\DEFILER1.wav",	HSFX_DEFILER1),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\DEFILER2.wav",	HSFX_DEFILER2),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\DEFILER3.wav",	HSFX_DEFILER3),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\DEFILER4.wav",	HSFX_DEFILER4),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\DEFILER8.wav",	HSFX_DEFILER5),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\DEFILER6.wav",	HSFX_DEFILER6),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\DEFILER7.wav",	HSFX_DEFILER7),

	SFX(sfx_STREAM,			"Sfx\\Hellfire\\NAKRUL1.wav",	HSFX_NA_KRUL1),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\NAKRUL2.wav",	HSFX_NA_KRUL2),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\NAKRUL3.wav",	HSFX_NA_KRUL3),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\NAKRUL4.wav",	HSFX_NA_KRUL4),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\NAKRUL5.wav",	HSFX_NA_KRUL5),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\NAKRUL6.wav",	HSFX_NA_KRUL6),

	SFX(sfx_STREAM,			"Sfx\\Hellfire\\NARATR3.wav",	HSFX_CORNERSTONE1),

	SFX(sfx_STREAM,			"Sfx\\Hellfire\\COWSUT1.wav",	HSFX_COWSUIT1),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\COWSUT2.wav",	HSFX_COWSUIT2),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\COWSUT3.wav",	HSFX_COWSUIT3),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\COWSUT4.wav",	HSFX_COWSUIT4),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\COWSUT4A.wav",	HSFX_COWSUIT4A),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\COWSUT5.wav",	HSFX_COWSUIT5),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\COWSUT6.wav",	HSFX_COWSUIT6),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\COWSUT7.wav",	HSFX_COWSUIT7),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\COWSUT8.wav",	HSFX_COWSUIT8),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\COWSUT9.wav",	HSFX_COWSUIT9),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\COWSUT10.wav",	HSFX_COWSUIT10),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\COWSUT11.wav",	HSFX_COWSUIT11),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\COWSUT12.wav",	HSFX_COWSUIT12),

	SFX(sfx_STREAM,			"Sfx\\Hellfire\\Skljrn1.wav",	HSFX_SKULLJRNL1),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\Naratr6.wav",	HSFX_SKULLJRNL2),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\Naratr7.wav",	HSFX_SKULLJRNL3),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\Naratr8.wav",	HSFX_SKULLJRNL4),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\Naratr5.wav",	HSFX_SKULLJRNL5),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\Naratr9.wav",	HSFX_SKULLJRNL6),
	SFX(sfx_STREAM,			"Sfx\\Hellfire\\Naratr4.wav",	HSFX_SKULLJRNL7),

	SFX(sfx_STREAM,			"Sfx\\Hellfire\\TRADER1.wav",	HSFX_TRADER1),


	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Cropen.wav",		CR_DOOROPEN),
	SFX(sfx_ALLOWMULTIPLE,	"Sfx\\Items\\Crclos.wav",		CR_DOORCLOS),



#endif
};

// Until there are actual sounds, don't hog the memory by loading duplicates.
#if !IS_VERSION(SHAREWARE)
#define	PS_BARD1		PS_ROGUE1	
#define	PS_BARD2		PS_ROGUE2	
#define	PS_BARD3		PS_ROGUE3	
#define	PS_BARD4		PS_ROGUE4	
#define	PS_BARD5		PS_ROGUE5	
#define	PS_BARD6		PS_ROGUE6	
#define	PS_BARD7		PS_ROGUE7	
#define	PS_BARD8		PS_ROGUE8	
#define	PS_BARD9		PS_ROGUE9	
#define	PS_BARD10		PS_ROGUE10	
#define	PS_BARD11		PS_ROGUE11	
#define	PS_BARD12		PS_ROGUE12	
#define	PS_BARD13		PS_ROGUE13	
#define	PS_BARD14		PS_ROGUE14	
#define	PS_BARD15		PS_ROGUE15	
#define	PS_BARD16		PS_ROGUE16	
#define	PS_BARD17		PS_ROGUE17	
#define	PS_BARD18		PS_ROGUE18	
#define	PS_BARD19		PS_ROGUE19	
#define	PS_BARD20		PS_ROGUE20	
#define	PS_BARD21		PS_ROGUE21	
#define	PS_BARD22		PS_ROGUE22	
#define	PS_BARD23		PS_ROGUE23	
#define	PS_BARD24		PS_ROGUE24	
#define	PS_BARD25		PS_ROGUE25	
#define	PS_BARD26		PS_ROGUE26	
#define	PS_BARD27		PS_ROGUE27	
#define	PS_BARD28		PS_ROGUE28	
#define	PS_BARD29		PS_ROGUE29	
#define	PS_BARD30		PS_ROGUE30	
#define	PS_BARD31		PS_ROGUE31	
#define	PS_BARD32		PS_ROGUE32	
#define	PS_BARD33		PS_ROGUE33	
#define	PS_BARD34		PS_ROGUE34	
#define	PS_BARD35		PS_ROGUE35	
#define	PS_BARD36		PS_ROGUE36	
#define	PS_BARD37		PS_ROGUE37	
#define	PS_BARD38		PS_ROGUE38	
#define	PS_BARD39		PS_ROGUE39	
#define	PS_BARD40		PS_ROGUE40	
#define	PS_BARD41		PS_ROGUE41	
#define	PS_BARD42		PS_ROGUE42	
#define	PS_BARD43		PS_ROGUE43	
#define	PS_BARD44		PS_ROGUE44	
#define	PS_BARD45		PS_ROGUE45	
#define	PS_BARD46		PS_ROGUE46	
#define	PS_BARD47		PS_ROGUE47	
#define	PS_BARD48		PS_ROGUE48	
#define	PS_BARD49		PS_ROGUE49	
#define	PS_BARD50		PS_ROGUE50	
#define	PS_BARD51		PS_ROGUE51	
#define	PS_BARD52		PS_ROGUE52	
#define	PS_BARD53		PS_ROGUE53	
#define	PS_BARD54		PS_ROGUE54	
#define	PS_BARD55		PS_ROGUE55	
#define	PS_BARD56		PS_ROGUE56	
#define	PS_BARD57		PS_ROGUE57	
#define	PS_BARD58		PS_ROGUE58	
#define	PS_BARD59		PS_ROGUE59	
#define	PS_BARD60		PS_ROGUE60	
#define	PS_BARD61		PS_ROGUE61	
#define	PS_BARD62		PS_ROGUE62	
#define	PS_BARD63		PS_ROGUE63	
#define	PS_BARD64		PS_ROGUE64	
#define	PS_BARD65		PS_ROGUE65	
#define	PS_BARD66		PS_ROGUE66	
#define	PS_BARD67		PS_ROGUE67	
#define	PS_BARD68		PS_ROGUE68	
#define	PS_BARD69		PS_ROGUE69
#define	PS_BARD69B		PS_ROGUE69B	
#define	PS_BARD70		PS_ROGUE70	
#define	PS_BARD71		PS_ROGUE71	
#define	PS_BARD72		PS_ROGUE72	
#define	PS_BARD73		PS_ROGUE73	
#define	PS_BARD74		PS_ROGUE74	
#define	PS_BARD75		PS_ROGUE75	
#define	PS_BARD76		PS_ROGUE76	
#define	PS_BARD77		PS_ROGUE77	
#define	PS_BARD78		PS_ROGUE78	
#define	PS_BARD79		PS_ROGUE79	
#define	PS_BARD80		PS_ROGUE80	
#define	PS_BARD81		PS_ROGUE81	
#define	PS_BARD82		PS_ROGUE82	
#define	PS_BARD83		PS_ROGUE83	
#define	PS_BARD84		PS_ROGUE84	
#define	PS_BARD85		PS_ROGUE85	
#define	PS_BARD86		PS_ROGUE86	
#define	PS_BARD87		PS_ROGUE87	
#define	PS_BARD88		PS_ROGUE88	
#define	PS_BARD89		PS_ROGUE89	
#define	PS_BARD90		PS_ROGUE90	
#define	PS_BARD91		PS_ROGUE91	
#define	PS_BARD92		PS_ROGUE92	
#define	PS_BARD93		PS_ROGUE93	
#define	PS_BARD94		PS_ROGUE94	
#define	PS_BARD95		PS_ROGUE95	
#define	PS_BARD96		PS_ROGUE96	
#define	PS_BARD97		PS_ROGUE97	
#define	PS_BARD98		PS_ROGUE98	
#define	PS_BARD99		PS_ROGUE99	
#define	PS_BARD100		PS_ROGUE100	
#define	PS_BARD101		PS_ROGUE101	
#define	PS_BARD102		PS_ROGUE102	

#define	PS_BARBARIAN1		PS_WARR1	
#define	PS_BARBARIAN2		PS_WARR2	
#define	PS_BARBARIAN3		PS_WARR3	
#define	PS_BARBARIAN4		PS_WARR4	
#define	PS_BARBARIAN5		PS_WARR5	
#define	PS_BARBARIAN6		PS_WARR6	
#define	PS_BARBARIAN7		PS_WARR7	
#define	PS_BARBARIAN8		PS_WARR8	
#define	PS_BARBARIAN9		PS_WARR9	
#define	PS_BARBARIAN10		PS_WARR10	
#define	PS_BARBARIAN11		PS_WARR11	
#define	PS_BARBARIAN12		PS_WARR12	
#define	PS_BARBARIAN13		PS_WARR13	
#define	PS_BARBARIAN14		PS_WARR14	
#define	PS_BARBARIAN15		PS_WARR15	
#define	PS_BARBARIAN16		PS_WARR16	
#define	PS_BARBARIAN17		PS_WARR17	
#define	PS_BARBARIAN18		PS_WARR18	
#define	PS_BARBARIAN19		PS_WARR19	
#define	PS_BARBARIAN20		PS_WARR20	
#define	PS_BARBARIAN21		PS_WARR21	
#define	PS_BARBARIAN22		PS_WARR22	
#define	PS_BARBARIAN23		PS_WARR23	
#define	PS_BARBARIAN24		PS_WARR24	
#define	PS_BARBARIAN25		PS_WARR25	
#define	PS_BARBARIAN26		PS_WARR26	
#define	PS_BARBARIAN27		PS_WARR27	
#define	PS_BARBARIAN28		PS_WARR28	
#define	PS_BARBARIAN29		PS_WARR29	
#define	PS_BARBARIAN30		PS_WARR30	
#define	PS_BARBARIAN31		PS_WARR31	
#define	PS_BARBARIAN32		PS_WARR32	
#define	PS_BARBARIAN33		PS_WARR33	
#define	PS_BARBARIAN34		PS_WARR34	
#define	PS_BARBARIAN35		PS_WARR35	
#define	PS_BARBARIAN36		PS_WARR36	
#define	PS_BARBARIAN37		PS_WARR37	
#define	PS_BARBARIAN38		PS_WARR38	
#define	PS_BARBARIAN39		PS_WARR39	
#define	PS_BARBARIAN40		PS_WARR40	
#define	PS_BARBARIAN41		PS_WARR41	
#define	PS_BARBARIAN42		PS_WARR42	
#define	PS_BARBARIAN43		PS_WARR43	
#define	PS_BARBARIAN44		PS_WARR44	
#define	PS_BARBARIAN45		PS_WARR45	
#define	PS_BARBARIAN46		PS_WARR46	
#define	PS_BARBARIAN47		PS_WARR47	
#define	PS_BARBARIAN48		PS_WARR48	
#define	PS_BARBARIAN49		PS_WARR49	
#define	PS_BARBARIAN50		PS_WARR50	
#define	PS_BARBARIAN51		PS_WARR51	
#define	PS_BARBARIAN52		PS_WARR52	
#define	PS_BARBARIAN53		PS_WARR53	
#define	PS_BARBARIAN54		PS_WARR54	
#define	PS_BARBARIAN55		PS_WARR55	
#define	PS_BARBARIAN56		PS_WARR56	
#define	PS_BARBARIAN57		PS_WARR57	
#define	PS_BARBARIAN58		PS_WARR58	
#define	PS_BARBARIAN59		PS_WARR59	
#define	PS_BARBARIAN60		PS_WARR60	
#define	PS_BARBARIAN61		PS_WARR61	
#define	PS_BARBARIAN62		PS_WARR62	
#define	PS_BARBARIAN63		PS_WARR63	
#define	PS_BARBARIAN64		PS_WARR64	
#define	PS_BARBARIAN65		PS_WARR65	
#define	PS_BARBARIAN66		PS_WARR66	
#define	PS_BARBARIAN67		PS_WARR67	
#define	PS_BARBARIAN68		PS_WARR68	
#define	PS_BARBARIAN69		PS_WARR69
#define	PS_BARBARIAN69B		PS_WARR69B	
#define	PS_BARBARIAN70		PS_WARR70	
#define	PS_BARBARIAN71		PS_WARR71	
#define	PS_BARBARIAN72		PS_WARR72	
#define	PS_BARBARIAN73		PS_WARR73	
#define	PS_BARBARIAN74		PS_WARR74	
#define	PS_BARBARIAN75		PS_WARR75	
#define	PS_BARBARIAN76		PS_WARR76	
#define	PS_BARBARIAN77		PS_WARR77	
#define	PS_BARBARIAN78		PS_WARR78	
#define	PS_BARBARIAN79		PS_WARR79	
#define	PS_BARBARIAN80		PS_WARR80	
#define	PS_BARBARIAN81		PS_WARR81	
#define	PS_BARBARIAN82		PS_WARR82	
#define	PS_BARBARIAN83		PS_WARR83	
#define	PS_BARBARIAN84		PS_WARR84	
#define	PS_BARBARIAN85		PS_WARR85	
#define	PS_BARBARIAN86		PS_WARR86	
#define	PS_BARBARIAN87		PS_WARR87	
#define	PS_BARBARIAN88		PS_WARR88	
#define	PS_BARBARIAN89		PS_WARR89	
#define	PS_BARBARIAN90		PS_WARR90	
#define	PS_BARBARIAN91		PS_WARR91	
#define	PS_BARBARIAN92		PS_WARR92	
#define	PS_BARBARIAN93		PS_WARR93	
#define	PS_BARBARIAN94		PS_WARR94	
#define	PS_BARBARIAN95		PS_WARR95	
#define	PS_BARBARIAN96		PS_WARR96B
#define	PS_BARBARIAN97		PS_WARR97	
#define	PS_BARBARIAN98		PS_WARR98	
#define	PS_BARBARIAN99		PS_WARR99	
#define	PS_BARBARIAN100		PS_WARR100	
#define	PS_BARBARIAN101		PS_WARR101	
#define	PS_BARBARIAN102		PS_WARR102	

#endif	// SHAREWARE

#undef SFX
#undef rSFX
#undef wSFX
#undef sSFX
#undef mSFX
