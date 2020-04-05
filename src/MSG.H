//******************************************************************
// msg.h
//******************************************************************

enum {
	CMD_STAND = 0,
	CMD_WALKXY,
	CMD_ACK_PLRINFO,
	CMD_ADDSTR,
	CMD_ADDMAG,
	CMD_ADDDEX,
	CMD_ADDVIT,
	CMD_SBSPELL,
	CMD_GETITEM,
	CMD_AGETITEM,
	CMD_PUTITEM,
	CMD_RESPAWNITEM,
	CMD_ATTACKXY,
	CMD_RATTACKXY,
	CMD_SPELLXY,
	CMD_TSPELLXY,
	CMD_OPOBJXY,
	CMD_DISARMXY,
	CMD_ATTACKID,
	CMD_ATTACKPID,
	CMD_RATTACKID,
	CMD_RATTACKPID,
	CMD_SPELLID,
	CMD_SPELLPID,
	CMD_TSPELLID,
	CMD_TSPELLPID,
	CMD_RESURRECT,
	CMD_OPOBJT,
	CMD_KNOCKBACK,
	CMD_TALKXY,
	CMD_NEWLVL,
	CMD_WARP,
	CMD_CHEAT_EXPERIENCE,
	CMD_CHEAT_SPELL_LEVEL,
	CMD_DEBUG,
	CMD_SYNCDATA,
	CMD_MONSTDEATH,
	CMD_MONSTDAMAGE,
	CMD_PLRDEAD,
	CMD_REQUESTGITEM,
	CMD_REQUESTAGITEM,
	CMD_GOTOGETITEM,
	CMD_GOTOAGETITEM,
	CMD_OPENDOOR,
	CMD_CLOSEDOOR,
	CMD_OPERATEOBJ,
	CMD_PLROPOBJ,
	CMD_BREAKOBJ,
	CMD_CHANGEPLRITEMS,
	CMD_DELPLRITEMS,
	CMD_PLRDAMAGE,
	CMD_PLRLEVEL,
	CMD_DROPITEM,
	CMD_PLAYER_JOINLEVEL,
	CMD_SEND_PLRINFO,
	CMD_SATTACKXY,
	CMD_ACTIVATEPORTAL,
	CMD_DEACTIVATEPORTAL,
	CMD_DLEVEL_0,
	CMD_DLEVEL_1,
	CMD_DLEVEL_2,
	CMD_DLEVEL_3,
	CMD_DLEVEL_4,
	CMD_DLEVEL_5,
	CMD_DLEVEL_6,
	CMD_DLEVEL_7,
	CMD_DLEVEL_8,
	CMD_DLEVEL_9,
	CMD_DLEVEL_10,
	CMD_DLEVEL_11,
	CMD_DLEVEL_12,
	CMD_DLEVEL_13,
	CMD_DLEVEL_14,
	CMD_DLEVEL_15,
	CMD_DLEVEL_16,
	CMD_DLEVEL_17,
	CMD_DLEVEL_18,
	CMD_DLEVEL_19,
	CMD_DLEVEL_20,
	CMD_DLEVEL_21,
	CMD_DLEVEL_22,
	CMD_DLEVEL_23,
	CMD_DLEVEL_24,
	CMD_DLEVEL_JUNK,
	CMD_DLEVEL_END,
	CMD_HEALOTHER,
	CMD_STRING,
	CMD_SETSTR,
	CMD_SETMAG,
	CMD_SETDEX,
	CMD_SETVIT,
	CMD_RETOWN,
	CMD_SPELLXYD,
	CMD_ITEMEXTRA,
	CMD_SYNCPUTITEM,
	CMD_KILLGOLEM,
	CMD_SYNCQUEST,
	CMD_ENDSHIELD,
	CMD_AWAKEGOLEM,
	CMD_ENDREFLECT,
	CMD_OPEN_NAKRUL,
	CMD_OPEN_NEST,
	CMD_OPEN_CRYPT,

	// thse are commands which are never sent
	FAKE_CMD_SETID,
	FAKE_CMD_DROPID,
	NUM_CMDS
};


//******************************************************************
// net commands
//******************************************************************
#pragma pack(push,1)
typedef struct TCmd {
	BYTE bCmd;
} TCmd;
void NetSendCmd(BOOL bHiPri,BYTE bCmd);

void NetSendCmdLoc(BOOL bHiPri,BYTE bCmd,BYTE x,BYTE y);
void NetSendCmdLocParam1(BOOL bHiPri,BYTE bCmd,BYTE x,BYTE y,WORD wParam1);
void NetSendCmdLocParam2(BOOL bHiPri,BYTE bCmd,BYTE x,BYTE y,WORD wParam1,WORD wParam2);
void NetSendCmdLocParam3(BOOL bHiPri,BYTE bCmd,BYTE x,BYTE y,WORD wParam1,WORD wParam2,WORD wParam3);

void NetSendCmdParam1(BOOL bHiPri,BYTE bCmd,WORD wParam1);
void NetSendCmdParam2(BOOL bHiPri,BYTE bCmd,WORD wParam1,WORD wParam2);
void NetSendCmdParam3(BOOL bHiPri,BYTE bCmd,WORD wParam1,WORD wParam2, WORD wParam3 );

void NetSendCmdQuest(BOOL bHiPri,BYTE q);

void NetSendCmdGItem(BOOL bHiPri,BYTE bCmd,BYTE mast,BYTE pnum,BYTE bCursitem);
void NetSendCmdPItem(BOOL bHiPri,BYTE bCmd,BYTE x,BYTE y);
void NetSendCmdDItem(BOOL bHiPri,int ii);
void NetSendCmdChItem(BOOL bHiPri,BYTE bLoc);
void NetSendCmdDelItem(BOOL bHiPri,BYTE bLoc);

void NetSendCmdDamage(BOOL bHiPri,BYTE bPlr,DWORD dwDam);
void NetSendCmdMonstDamage(BOOL bHiPri,WORD bMonst,DWORD dwDam);

typedef struct TCmdPlrInfoHdr {
	BYTE bCmd;
	WORD wOffset;
	WORD wBytes;
} TCmdPlrInfoHdr;

// better not be bigger than this, the net will choke
#define MAX_SEND_STR_LEN	80
#define SEND_ALL_MASK	0xffffffff
void NetSendString(DWORD dwSendMask,const char * pszStr);


#pragma pack(pop)


//******************************************************************
// net syncing
//******************************************************************
#pragma pack(push,1)
typedef struct TSyncHeader {
	BYTE bCmd;
	BYTE bLevel;
	WORD wLen;
	BYTE bObjId;
	BYTE bObjCmd;
	BYTE bItemI;
	BYTE bItemX;
	BYTE bItemY;
	WORD wItemIndx;
	WORD wItemCI;
	int dwItemSeed;
	BYTE bItemId;
	BYTE bItemDur;
	BYTE bItemMDur;
	BYTE bItemCh;
	BYTE bItemMCh;
	WORD wItemVal;
	DWORD dwItemBuff;
	BYTE bPInvLoc;
	WORD wPInvIndx;
	WORD wPInvCI;
	int dwPInvSeed;
	// drb.patch1.start.02/10/97
	BYTE bPInvId;
	// drb.patch1.end.02/10/97
	WORD wPLToHit;
	WORD wMaxDam;
	BYTE bMinStr;
	BYTE bMinMag;
	BYTE bMinDex;
	BYTE bAC;
} TSyncHeader;

typedef struct TSyncMonster {
	BYTE _mndx;
	BYTE _mx;
	BYTE _my;
	BYTE _menemy;
	BYTE _mdelta;
} TSyncMonster;
#pragma pack(pop)


//******************************************************************
// msg sizes
//******************************************************************
#define MIN_MSG_SIZE 128
#define MAX_MSG_SIZE 512
extern DWORD gdwNormalMsgSize;	// MIN_MSG_SIZE..MAX_MSG_SIZE
extern DWORD gdwLargestMsgSize;	// MIN_MSG_SIZE..MAX_MSG_SIZE


//******************************************************************
// net packets (which hold net commands inside them)
//******************************************************************
#pragma pack(push,1)
typedef struct TPktHdr {
	BYTE px;
	BYTE py;
	BYTE targx;
	BYTE targy;
	DWORD php;
	DWORD pmhp;
	BYTE bstr;
	BYTE bmag;
	BYTE bdex;
	WORD wCheck;	// @@ debug -- delete this
	WORD wLen;		// @@ debug -- delete this
} TPktHdr;
typedef struct TPkt {
	TPktHdr hdr;
	BYTE body[MAX_MSG_SIZE - sizeof TPktHdr];
} TPkt;
#pragma pack(pop)


//******************************************************************
//******************************************************************
extern BOOL deltaload;
void DeltaSaveLevel();
void DeltaLoadLevel();
void DeltaSendAllLevels(int pnum);
DWORD ParseCmd(int pnum, const TCmd * pCmd);
void DeltaAddItem(int ii);

// PATCH1.JMM
#if _DEBUG
void __cdecl DROPLOG(const char * pszFmt,...);
void VerifyItemActiveList( void );
#else
#define VerifyItemActiveList( )
#define DROPLOG //
#endif
// ENDPATCH1

