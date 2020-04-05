/*-----------------------------------------------------------------------**
** 								Diablo
**
**						 Control panel file
**
** 				(C)1995 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/ERROR.CPP 1     1/22/97 2:06p Dgartner $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include	"error.h"
#include	"engine.h"
#include	"control.h"
#include	"items.h"
#include	"stores.h"
#include	"scrollrt.h"

/*-----------------------------------------------------------------------**
** Message strings
**-----------------------------------------------------------------------*/

char *MsgStrings[] = {
"",
"No automap available in town",
"No multiplayer functions in demo",
"Direct Sound Creation Failed",
"Not available in shareware version",
"Not enough space to save",
"No Pause in town",
"Copying to a hard disk is recommended",

"Multiplayer sync problem",
"No pause in multiplayer",
"Loading...",
"Saving...",

//Shrine messages
"Some are weakened as one grows strong",			//12 Mysterious
"New strength is forged through destruction",		//13 Hidden
"Those who defend seldom attack",					//14 Gloomy
"The sword of justice is swift and sharp",			//15 Weird
"While the spirit is vigilant the body thrives",	//16 Magical
"The powers of mana refocused renews",				//17 Stone
"Time cannot diminish the power of steel",			//18 Religious
"Magic is not always what it seems to be",			//19 Enchanted
"What once was opened now is closed",				//20 Thaumaturgic
"Intensity comes at the cost of wisdom",			//21 Fascinating
"Arcane power brings destruction",					//22 Cryptic
"That which cannot be held cannot be harmed",		//23 Supernatural
"Crimson and Azure become as the sun",				//24 Eldritch
"Knowledge and wisdom at the cost of self",			//25 Eerie
"Drink and be refreshed",							//26 Divine
"Wherever you go, there you are",					//27 Holy
"Energy comes at the cost of wisdom",				//28 Sacred
"Riches abound when least expected",				//29 Spiritual
"Where avarice fails, patience gains reward",		//30 Spooky
"Blessed by a benevolent companion!",				//31 Spooky Multi
"The hands of men may be guided by fate",			//32 Abandoned
"Strength is bolstered by heavenly faith",			//33 Creepy
"The essence of life flows from within",			//34 Quiet
"The way is made clear when viewed from above",		//35 Secluded
"Salvation comes at the cost of wisdom",			//36 Ornate
"Mysteries are revealed in the light of reason",	//37 Glimmering
"Those who are last may yet be first",				//38 Tainted
"Generosity brings its own rewards",				//39 Tainted Multi

//Shortcut messages
"You must be at least level 8 to use this.",		//40
"You must be at least level 13 to use this.",		//41
"You must be at least level 17 to use this.",		//42
"Arcane knowledge gained!",							//43

// New Shrines
"That which does not kill you...",					//44 Oily
"Knowledge is power.",								//45 Glowing
"Give and you shall receive.",						//46 Mendicants.
"Some experience is gained by touch.",				//47 Edisons
"There's no place like home.",		  				//48 Town
"Spirtual energy is restored.",		   				//49 Energy
"You feel more agile.",								//50 Time Morning
"You feel stronger.",								//51 Time Afternoon.
"You feel wiser.",									//52 Time Evening.
"You feel refreshed.",								//53 Time Night.
"That which can break will.",						//54 Murphy's
};

/*-----------------------------------------------------------------------**
** Local defines
**-----------------------------------------------------------------------*/

#define MSGCNT 70

char msgflag;
char msgdelay;
char msgtable[80];
char msgcnt = 0;

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void InitDiabloMsg(char e)
{
	int i;

	for (i = 0; i < msgcnt; i++) {
		if (msgtable[i] == e) return;
	}

	msgtable[msgcnt] = e;
	if (msgcnt < 80) msgcnt++;
	msgflag = msgtable[0];
	msgdelay = MSGCNT;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void ClrDiabloMsg()
{
	for (int i = 0; i < 80; i++) msgtable[i] = MSG_NONE;
	msgflag = MSG_NONE;
	msgcnt = 0;
}

/*-----------------------------------------------------------------------**
**-----------------------------------------------------------------------*/

void DrawDiabloMsg()
{
	int		i, x, y;
	long	boffset;
	int		sl,tw;

	// Draw error box
	DrawCel(165, 318, pSTextSlidCels, 1, 12);
	DrawCel(591, 318, pSTextSlidCels, 4, 12);
	DrawCel(165, 366, pSTextSlidCels, 2, 12);
	DrawCel(591, 366, pSTextSlidCels, 3, 12);

	x = 173;
	for (i = 0; i < 35; i++) {
		DrawCel(x, 318, pSTextSlidCels, 5, 12);
		DrawCel(x, 366, pSTextSlidCels, 7, 12);
		x += 12;
	}
	y = 330;
	for (i = 0; i < 3; i++) {
		DrawCel(165, y, pSTextSlidCels, 6, 12);
		DrawCel(591, y, pSTextSlidCels, 8, 12);
		y += 12;
	}

	app_assert(gpBuffer);
	__asm {
		mov		edi,dword ptr [gpBuffer]
		add		edi,278952			

		xor		eax,eax
		mov		edx,27
_YLp:	mov		ecx,216						// (width+6)/2
_XLp1:	stosb
		inc		edi
		loop	_XLp1
		sub		edi,1200					// ((width+6)/2) + 768
		mov		ecx,216						// (width+6)/2
_XLp2:	inc		edi
		stosb
		loop	_XLp2
		sub		edi,1200					// ((width+6)/2) + 768
		dec		edx
		jnz		_YLp
	}

	strcpy(tempstr, MsgStrings[msgflag]);

	boffset = nBuffWTbl[342] + 165; 
	sl = strlen(tempstr);
	tw = 0;
	for (i = 0; i < sl; i++) {
		BYTE c = char2print(tempstr[i]);
		c = fonttrans[c];
		tw += fontkern[c]+1;
	}
	if (tw < 442) boffset += (442 - tw) >> 1;
	for (i = 0; i < sl; i++) {
		BYTE c = char2print(tempstr[i]);
		c = fonttrans[c];
		if (c) DrawPanelFont(boffset, c, ICOLOR_GOLD);	
		boffset += fontkern[c]+1;
	}

	if (msgdelay > 0) msgdelay--;

	if (msgdelay == 0) {
		msgcnt--;
		msgdelay = MSGCNT;
		if (msgcnt == 0) msgflag = MSG_NONE;
		else msgflag = msgtable[msgcnt];
	}
}
