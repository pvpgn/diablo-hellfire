/*-----------------------------------------------------------------------**
** 								Diablo
**
** 						   Missiles data file
**
** 				(C)1996 Condor, Inc. All rights reserved.
**
**-----------------------------------------------------------------------**
**	$Header: /Diablo/MISDAT.CPP 2     1/23/97 12:21p Jmorin $
**-----------------------------------------------------------------------**
**
** 							 File Routines
**-----------------------------------------------------------------------*/

#include	"diablo.h"
#pragma hdrstop
#include    "effects.h"
#include    "missiles.h"
#include    "misdat.h"

void	AddLArrow(int, int, int, int, int, int, char, int, int);
void	AddArrow(int, int, int, int, int, int, char, int, int);
void	AddRndTeleport(int, int, int, int, int, int, char, int, int);
void	AddFirebolt(int, int, int, int, int, int, char, int, int);
void	AddMagmaball(int, int, int, int, int, int, char, int, int);
void	AddTeleport(int, int, int, int, int, int, char, int, int);
void	AddLightball(int, int, int, int, int, int, char, int, int);
void	AddLightwall(int, int, int, int, int, int, char, int, int);
void	AddFirewall(int, int, int, int, int, int, char, int, int);
void	AddFireball(int, int, int, int, int, int, char, int, int);
void	AddLightctrl(int, int, int, int, int, int, char, int, int);
void	AddLightning(int, int, int, int, int, int, char, int, int);
void	AddMisexp(int, int, int, int, int, int, char, int, int);
void	AddTown(int, int, int, int, int, int, char, int, int);
void	AddFlash(int, int, int, int, int, int, char, int, int);
void	AddFlash2(int, int, int, int, int, int, char, int, int);
void	AddManashield(int, int, int, int, int, int, char, int, int);
void	AddFiremove(int, int, int, int, int, int, char, int, int);
void	AddGuardian(int, int, int, int, int, int, char, int, int);
void	AddChain(int, int, int, int, int, int, char, int, int);
//void	AddChainball(int, int, int, int, int, int, char, int, int);
void	AddBlood(int, int, int, int, int, int, char, int, int);
void	AddRage(int, int, int, int, int, int, char, int, int);
void	AddBone(int, int, int, int, int, int, char, int, int);
void	AddMetal(int, int, int, int, int, int, char, int, int);
void	AddRhino(int, int, int, int, int, int, char, int, int);
void	AddFireman(int, int, int, int, int, int, char, int, int);
void	AddFlare(int, int, int, int, int, int, char, int, int);
//void	AddDoom(int, int, int, int, int, int, char, int, int);
void	AddFireonly(int, int, int, int, int, int, char, int, int);
void	AddStone(int, int, int, int, int, int, char, int, int);
void	AddBloodR(int, int, int, int, int, int, char, int, int);
void	AddSpurt(int, int, int, int, int, int, char, int, int);
void	AddBoom(int, int, int, int, int, int, char, int, int);
void	AddHeal(int, int, int, int, int, int, char, int, int);
void	AddMana(int, int, int, int, int, int, char, int, int);
void	AddFMana(int, int, int, int, int, int, char, int, int);
void	AddHealOther(int, int, int, int, int, int, char, int, int);
void	AddIdentify(int, int, int, int, int, int, char, int, int);
void	AddFirewallC(int, int, int, int, int, int, char, int, int);
void	AddInfra(int, int, int, int, int, int, char, int, int);
void	AddWave(int, int, int, int, int, int, char, int, int);
void	AddNova(int, int, int, int, int, int, char, int, int);
void	AddBoil(int, int, int, int, int, int, char, int, int);
void	AddRepair(int, int, int, int, int, int, char, int, int);
void	AddRecharge(int, int, int, int, int, int, char, int, int);
void	AddDisarm(int, int, int, int, int, int, char, int, int);
void	AddApoca(int, int, int, int, int, int, char, int, int);
void	AddFlame(int, int, int, int, int, int, char, int, int);
void	AddFlamec(int, int, int, int, int, int, char, int, int);
void	AddKrull(int, int, int, int, int, int, char, int, int);
void	AddCbolt(int, int, int, int, int, int, char, int, int);
void	AddHbolt(int, int, int, int, int, int, char, int, int);
void	AddResurrect(int, int, int, int, int, int, char, int, int);
void	AddResurrectBeam(int, int, int, int, int, int, char, int, int);
void	AddTelekinesis(int, int, int, int, int, int, char, int, int);
void	AddAcid(int, int, int, int, int, int, char, int, int);
void	AddAcidpud(int, int, int, int, int, int, char, int, int);
void	AddGolem(int, int, int, int, int, int, char, int, int);
void	AddElement(int, int, int, int, int, int, char, int, int);
void	AddEther(int, int, int, int, int, int, char, int, int);
void	AddBoneSpirit(int, int, int, int, int, int, char, int, int);
void	AddWeapexp(int, int, int, int, int, int, char, int, int);
void	AddRportal(int, int, int, int, int, int, char, int, int);
void	AddDiabApoca(int, int, int, int, int, int, char, int, int);
void	AddSpecialArrow(int, int, int, int, int, int, char, int, int);
void	AddFBArrow(int, int, int, int, int, int, char, int, int);
void	AddLTArrow(int, int, int, int, int, int, char, int, int);
void	AddCBArrow(int, int, int, int, int, int, char, int, int);
void	AddHBArrow(int, int, int, int, int, int, char, int, int);
void	AddTeleStairs(int, int, int, int, int, int, char, int, int);
void	AddReflect(int, int, int, int, int, int, char, int, int);
void	AddBerserk(int, int, int, int, int, int, char, int, int);
void	AddFlameBox(int, int, int, int, int, int, char, int, int);
void	AddDisEnchant(int, int, int, int, int, int, char, int, int);
void	AddManaRemove(int, int, int, int, int, int, char, int, int);
void	AddShowMagicItems(int, int, int, int, int, int, char, int, int);
void	AddAura(int, int, int, int, int, int, char, int, int);
void	AddAura2(int, int, int, int, int, int, char, int, int);
void	AddSpiralFireBall(int, int, int, int, int, int, char, int, int);
void	AddRuneOfFire(int, int, int, int, int, int, char, int, int);
void	AddRuneOfLight(int, int, int, int, int, int, char, int, int);
void	AddRuneOfNova(int, int, int, int, int, int, char, int, int);
void	AddRuneOfImmolation(int, int, int, int, int, int, char, int, int);
void	AddRuneOfStone(int, int, int, int, int, int, char, int, int);
void	AddBigExplosion(int, int, int, int, int, int, char, int, int);
void	AddHorkSpawn(int, int, int, int, int, int, char, int, int);
void	AddRandom(int, int, int, int, int, int, char, int, int);
void	AddReallyBigExp(int, int, int, int, int, int, char, int, int);

void	MI_Dummy(int);
void	MI_Manashield(int);
void	MI_SetManashield(int);
void	MI_LArrow(int);
void	MI_Arrow(int);
void	MI_Firebolt(int);
void	MI_Lightball(int);
void	MI_Lightwall(int);
void	MI_Firewall(int);
void	MI_Fireball(int);
void	MI_Lightctrl(int);
void	MI_Lightning(int);
void	MI_Misexp(int);
void	MI_Town(int);
void	MI_Flash(int);
void	MI_Flash2(int);
void	MI_Manashield(int);
void	MI_Firemove(int);
void	MI_Guardian(int);
void	MI_Chain(int);
//void	MI_Chainball(int);
void	MI_Blood(int);
void	MI_Rage(int);
void	MI_Rhino(int);
void	MI_Fireman(int);
void	MI_Misexp(int);
void	MI_Teleport(int);
//void	MI_Doom(int);
void	MI_Stone(int);
void	MI_Boom(int);
void	MI_FirewallC(int);
void	MI_LightwallC(int);
void	MI_Infra(int);
void	MI_Apoca(int);
void	MI_Wave(int);
void	MI_Nova(int);
void	MI_FireNova(int);
void	MI_Boil(int);
void	MI_Flame(int);
void	MI_Flamec(int);
void	MI_Krull(int);
void	MI_Cbolt(int);
void	MI_Hbolt(int);
void	MI_Acid(int);
void	MI_Acidsplat(int);
void	MI_Acidpud(int);
void	MI_Ether(int);
void	MI_Element(int);
void	MI_ResurrectBeam(int);
void	MI_Weapexp(int);
void	MI_Rportal(int);
void	MI_Golem(int);
void	MI_Bonespirit(int);
void	MI_SpecialArrow(int);
void	MI_LTArrow(int);
void	MI_FlameBox(int);
void	MI_LightBox(int);
void	MI_ShowMagicItems(int);
void	MI_Aura(int);
void	MI_Aura2(int);
void	MI_SpiralFireBall(int);
void	MI_Rune(int);
void	MI_BigExplosion(int);
void	MI_HorkSpawn(int);
void	MI_Reflect(int);


/*-----------------------------------------------------------------------*
** Global Variables
**-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*
**-----------------------------------------------------------------------*/
MissileData missiledata[NUMBER_OF_MISSILE_TYPES] =
{
	{ MIT_ARROW,			// #defines name of missile for ease of use
	AddArrow,				// procedure for creating one of these missiles
	MI_Arrow,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_WEAP,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_ARROW,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_FIREBOLT,			// #defines name of missile for ease of use
	AddFirebolt,			// procedure for creating one of these missiles
	MI_Firebolt,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_FIRE,				// missile resistance type (fire, light, misc, none)
	MF_FIREBOLT,			// file# of missile gfx table
	LS_FBOLT1,				// missile launch sound
	LS_FIRIMP2 },			// missile impact sound

	{ MIT_GUARDIAN,			// #defines name of missile for ease of use
	AddGuardian,			// procedure for creating one of these missiles
	MI_Guardian,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_GUARDIAN,			// file# of missile gfx table
	LS_GUARD,				// missile launch sound
	LS_GUARDLAN },			// missile impact sound

	{ MIT_PHASE,			// #defines name of missile for ease of use
	AddRndTeleport,			// procedure for creating one of these missiles
	MI_Teleport,			// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	LS_TELEPORT,			// missile launch sound
	-1 },					// missile impact sound

	{ MIT_LIGHTBALL,		// #defines name of missile for ease of use
	AddLightball,			// procedure for creating one of these missiles
	MI_Lightball,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_LGHT,				// missile resistance type (fire, light, misc, none)
	MF_LIGHTNING,			// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_FIREWALL,			// #defines name of missile for ease of use
	AddFirewall,			// procedure for creating one of these missiles
	MI_Firewall,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_FIRE,				// missile resistance type (fire, light, misc, none)
	MF_FIRE,				// file# of missile gfx table
	LS_WALLLOOP,			// missile launch sound
	LS_FIRIMP2 },			// missile impact sound

	{ MIT_FIREBALL,			// #defines name of missile for ease of use
	AddFireball,			// procedure for creating one of these missiles
	MI_Fireball,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_FIRE,				// missile resistance type (fire, light, misc, none)
	MF_FIREBOLT,			// file# of missile gfx table
	LS_FBOLT1,				// missile launch sound
	LS_FIRIMP2 },			// missile impact sound

	{ MIT_LIGHTCTRL,		// #defines name of missile for ease of use
	AddLightctrl,			// procedure for creating one of these missiles
	MI_Lightctrl,			// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_LGHT,				// missile resistance type (fire, light, misc, none)
	MF_LIGHTNING,			// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_LIGHTNING,		// #defines name of missile for ease of use
	AddLightning,			// procedure for creating one of these missiles
	MI_Lightning,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_LGHT,				// missile resistance type (fire, light, misc, none)
	MF_LIGHTNING,			// file# of missile gfx table
	LS_LNING1,				// missile launch sound
	LS_ELECIMP1 },			// missile impact sound

	{ MIT_MISEXP,			// #defines name of missile for ease of use
	AddMisexp,				// procedure for creating one of these missiles
	MI_Misexp,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_EXP1,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_TOWN,				// #defines name of missile for ease of use
	AddTown,				// procedure for creating one of these missiles
	MI_Town,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_MISC,				// missile resistance type (fire, light, misc, none)
	MF_TOWN,				// file# of missile gfx table
	LS_SENTINEL,			// missile launch sound
	LS_ELEMENTL },			// missile impact sound

	{ MIT_FLASH,			// #defines name of missile for ease of use
	AddFlash,				// procedure for creating one of these missiles
	MI_Flash,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_MISC,				// missile resistance type (fire, light, misc, none)
	MF_FLASH1,				// file# of missile gfx table
	LS_NOVA,				// missile launch sound
	LS_ELECIMP1 },			// missile impact sound

	{ MIT_FLASH2,			// #defines name of missile for ease of use
	AddFlash2,				// procedure for creating one of these missiles
	MI_Flash2,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_MISC,				// missile resistance type (fire, light, misc, none)
	MF_FLASH2,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_MANASHIELD,		// #defines name of missile for ease of use
	AddManashield,			// procedure for creating one of these missiles
	MI_SetManashield,		// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_MISC,				// missile resistance type (fire, light, misc, none)
	MF_MANASHLD,			// file# of missile gfx table
	LS_MSHIELD,				// missile launch sound
	-1 },					// missile impact sound

	{ MIT_FIREMOVE,			// #defines name of missile for ease of use
	AddFiremove,			// procedure for creating one of these missiles
	MI_Firemove,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_FIRE,				// missile resistance type (fire, light, misc, none)
	MF_FIRE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_CHAIN,			// #defines name of missile for ease of use
	AddChain,				// procedure for creating one of these missiles
	MI_Chain,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_LGHT,				// missile resistance type (fire, light, misc, none)
	MF_LIGHTNING,			// file# of missile gfx table
	LS_LNING1,				// missile launch sound
	LS_ELECIMP1 },			// missile impact sound

	{ MIT_CHAINBALL,		// #defines name of missile for ease of use
	NULL,					// procedure for creating one of these missiles
	NULL,					// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_LGHT,				// missile resistance type (fire, light, misc, none)
	MF_LIGHTNING,			// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_BLOOD,			// #defines name of missile for ease of use
	AddBlood,				// procedure for creating one of these missiles
	MI_Blood,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_BLOOD,				// file# of missile gfx table
	LS_BLODSTAR,			// missile launch sound
	LS_BLSIMPT },					// missile impact sound

	{ MIT_BONE,				// #defines name of missile for ease of use
	AddBone,				// procedure for creating one of these missiles
	MI_Blood,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_BONE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_METAL,			// #defines name of missile for ease of use
	AddMetal,				// procedure for creating one of these missiles
	MI_Blood,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_METAL,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_RHINO,			// #defines name of missile for ease of use
	AddRhino,				// procedure for creating one of these missiles
	MI_Rhino,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_MAGMABALL,		// #defines name of missile for ease of use
	AddMagmaball,			// procedure for creating one of these missiles
	MI_Firebolt,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_FIRE,				// missile resistance type (fire, light, misc, none)
	MF_MAGBALL,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_THINLIGHTCTRL,	// #defines name of missile for ease of use
	AddLightctrl,			// procedure for creating one of these missiles
	MI_Lightctrl,			// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_LGHT,				// missile resistance type (fire, light, misc, none)
	MF_THINLIGHT,			// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_THINLIGHT,		// #defines name of missile for ease of use
	AddLightning,			// procedure for creating one of these missiles
	MI_Lightning,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_LGHT,				// missile resistance type (fire, light, misc, none)
	MF_THINLIGHT,			// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_FLARE,			// #defines name of missile for ease of use
	AddFlare,				// procedure for creating one of these missiles
	MI_Firebolt,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_MISC,				// missile resistance type (fire, light, misc, none)
	MF_FLARE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_FLAREXP,			// #defines name of missile for ease of use
	AddMisexp,				// procedure for creating one of these missiles
	MI_Misexp,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_MISC,				// missile resistance type (fire, light, misc, none)
	MF_FLAREXP,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_TELE,				// #defines name of missile for ease of use
	AddTeleport,			// procedure for creating one of these missiles
	MI_Teleport,			// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	LS_ELEMENTL,			// missile launch sound
	-1 },					// missile impact sound

	{ MIT_FARROW,			// #defines name of missile for ease of use
	AddLArrow,				// procedure for creating one of these missiles
	MI_LArrow,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_WEAP,				// missile type (weapon, spell, none)
	MIMT_FIRE,				// missile resistance type (fire, light, misc, none)
	MF_FARROW,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_DOOM,				// #defines name of missile for ease of use
	NULL,					// procedure for creating one of these missiles
	NULL,					// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_MISC,				// missile resistance type (fire, light, misc, none)
	MF_DOOM,				// file# of missile gfx table
	LS_DSERP,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_FIREONLY,			// #defines name of missile for ease of use
	AddFireonly,			// procedure for creating one of these missiles
	MI_Firewall,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_FIRE,				// missile resistance type (fire, light, misc, none)
	MF_FIRE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_STONE,			// #defines name of missile for ease of use
	AddStone,				// procedure for creating one of these missiles
	MI_Stone,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_MISC,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	LS_SCURIMP,				// missile launch sound
	-1 },					// missile impact sound

	{ MIT_BLOODR,			// #defines name of missile for ease of use
	AddBloodR,				// procedure for creating one of these missiles
	MI_Dummy,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_INVIS,			// #defines name of missile for ease of use
	NULL,					// procedure for creating one of these missiles
	NULL,					// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	LS_INVISIBL,			// missile launch sound
	-1 },					// missile impact sound

	{ MIT_GOLEM,			// #defines name of missile for ease of use
	AddGolem,				// procedure for creating one of these missiles
	MI_Golem,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	LS_GOLUM,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_ETHER,			// #defines name of missile for ease of use
	AddEther,				// procedure for creating one of these missiles
	MI_Ether,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_ETHER,				// file# of missile gfx table
	LS_ETHEREAL,			// missile launch sound
	-1 },					// missile impact sound

	{ MIT_SPURT,			// #defines name of missile for ease of use
	AddSpurt,				// procedure for creating one of these missiles
	MI_Blood,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_SPURT,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_BOOM,				// #defines name of missile for ease of use
	AddBoom,				// procedure for creating one of these missiles
	MI_Boom,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_BOOM,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_HEAL,				// #defines name of missile for ease of use
	AddHeal,				// procedure for creating one of these missiles
	MI_Dummy,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_FIREWALLC,		// #defines name of missile for ease of use
	AddFirewallC,			// procedure for creating one of these missiles
	MI_FirewallC,			// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_FIRE,				// missile resistance type (fire, light, misc, none)
	MF_FIRE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_INFRA,			// #defines name of missile for ease of use
	AddInfra,				// procedure for creating one of these missiles
	MI_Infra,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	LS_INFRAVIS,			// missile launch sound
	-1 },					// missile impact sound

	{ MIT_IDENTIFY,			// #defines name of missile for ease of use
	AddIdentify,			// procedure for creating one of these missiles
	MI_Dummy,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_WAVE,				// #defines name of missile for ease of use
	AddWave,				// procedure for creating one of these missiles
	MI_Wave,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_FIRE,				// missile resistance type (fire, light, misc, none)
	MF_FIRE,				// file# of missile gfx table
	LS_FLAMWAVE,			// missile launch sound
	-1 },					// missile impact sound

	{ MIT_NOVA,				// #defines name of missile for ease of use
	AddNova,				// procedure for creating one of these missiles
	MI_Nova,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_LGHT,				// missile resistance type (fire, light, misc, none)
	MF_LIGHTNING,			// file# of missile gfx table
	LS_NOVA,				// missile launch sound
	-1 },					// missile impact sound

#if 0
	{ MIT_BLDBOIL,			// #defines name of missile for ease of use
	AddBoil,				// procedure for creating one of these missiles
	MI_Boil,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	LS_BLODBOIL },			// missile impact sound
#else
	{ MIT_RAGE,		   		// #defines name of missile for ease of use
	AddRage,				// procedure for creating one of these missiles
	MI_Rage,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound
#endif

	{ MIT_APOCA,			// #defines name of missile for ease of use
	AddApoca,				// procedure for creating one of these missiles
	MI_Apoca,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_MISC,				// missile resistance type (fire, light, misc, none)
	MF_BOOM,				// file# of missile gfx table
	LS_APOC,				// missile launch sound
	-1 },					// missile impact sound

	{ MIT_REPAIR,			// #defines name of missile for ease of use
	AddRepair,				// procedure for creating one of these missiles
	MI_Dummy,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_RECHARGE,			// #defines name of missile for ease of use
	AddRecharge,			// procedure for creating one of these missiles
	MI_Dummy,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_DISARM,			// #defines name of missile for ease of use
	AddDisarm,				// procedure for creating one of these missiles
	MI_Dummy,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	LS_TRAPDIS,				// missile launch sound
	-1 },					// missile impact sound

	{ MIT_FLAME,			// #defines name of missile for ease of use
	AddFlame,				// procedure for creating one of these missiles
	MI_Flame,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_FIRE,				// missile resistance type (fire, light, misc, none)
	MF_FLAMES,				// file# of missile gfx table
	//MF_LIGHTNING,				// file# of missile gfx table
	LS_SPOUTSTR,			// missile launch sound
	-1 },					// missile impact sound

	{ MIT_FLAMEC,			// #defines name of missile for ease of use
	AddFlamec,				// procedure for creating one of these missiles
	MI_Flamec,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_FIRE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_FIREMAN,			// #defines name of missile for ease of use
	AddFireman,				// procedure for creating one of these missiles
	MI_Fireman,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_KRULL,			// #defines name of missile for ease of use
	AddKrull,				// procedure for creating one of these missiles
	MI_Krull,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_WEAP,				// missile type (weapon, spell, none)
	MIMT_FIRE,				// missile resistance type (fire, light, misc, none)
	MF_KRULL,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_CBOLT,			// #defines name of missile for ease of use
	AddCbolt,				// procedure for creating one of these missiles
	MI_Cbolt,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_LGHT,				// missile resistance type (fire, light, misc, none)
	MF_CBOLT,				// file# of missile gfx table
	LS_CBOLT,				// missile launch sound
	-1 },					// missile impact sound

	{ MIT_HBOLT,			// #defines name of missile for ease of use
	AddHbolt,				// procedure for creating one of these missiles
	MI_Hbolt,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_HBOLT,				// file# of missile gfx table
	LS_HOLYBOLT,			// missile launch sound
	LS_ELECIMP1 },			// missile impact sound

	{ MIT_RESURRECT,		// #defines name of missile for ease of use
	AddResurrect,			// procedure for creating one of these missiles
	MI_Dummy,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_MISC,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	LS_RESUR },				// missile impact sound

	{ MIT_TELEKINESIS,		// #defines name of missile for ease of use
	AddTelekinesis,			// procedure for creating one of these missiles
	MI_Dummy,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	LS_ETHEREAL,			// missile launch sound
	-1 },					// missile impact sound

	{ MIT_LARROW,			// #defines name of missile for ease of use
	AddLArrow,				// procedure for creating one of these missiles
	MI_LArrow,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_WEAP,				// missile type (weapon, spell, none)
	MIMT_LGHT,				// missile resistance type (fire, light, misc, none)
	MF_LARROW,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_ACID,				// #defines name of missile for ease of use
	AddAcid,				// procedure for creating one of these missiles
	MI_Firebolt,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_ACID,				// missile resistance type (fire, light, misc, none)
	MF_ACID,				// file# of missile gfx table
	LS_ACID,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_ACIDSPLAT,			// #defines name of missile for ease of use
	AddMisexp,				// procedure for creating one of these missiles
	MI_Acidsplat,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_ACID,				// missile resistance type (fire, light, misc, none)
	MF_ACIDSPLAT,			// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_ACIDPUD,			// #defines name of missile for ease of use
	AddAcidpud,				// procedure for creating one of these missiles
	MI_Acidpud,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_ACID,				// missile resistance type (fire, light, misc, none)
	MF_ACIDPUD,				// file# of missile gfx table
	LS_PUDDLE,				// missile launch sound
	-1 },					// missile impact sound

	{ MIT_HEALOTHER,		// #defines name of missile for ease of use
	AddHealOther,			// procedure for creating one of these missiles
	MI_Dummy,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_ELEMENT,			// #defines name of missile for ease of use
	AddElement,				// procedure for creating one of these missiles
	MI_Element,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_FIRE,				// missile resistance type (fire, light, misc, none)
	MF_FIRERUN,				// file# of missile gfx table
	LS_ELEMENTL,			// missile launch sound
	-1 },					// missile impact sound

	{ MIT_RESURRECTBEAM,	// #defines name of missile for ease of use
	AddResurrectBeam,		// procedure for creating one of these missiles
	MI_ResurrectBeam,		// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_RESURRECT,			// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_BONESPIRIT,		// #defines name of missile for ease of use
	AddBoneSpirit,			// procedure for creating one of these missiles
	MI_Bonespirit,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_MISC,				// missile resistance type (fire, light, misc, none)
	MF_BONESPIRIT,			// file# of missile gfx table
	LS_BONESP,				// missile launch sound
	LS_BSIMPCT },			// missile impact sound

	{ MIT_WEAPEXP,			// #defines name of missile for ease of use
	AddWeapexp,				// procedure for creating one of these missiles
	MI_Weapexp,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_RPORTAL,			// #defines name of missile for ease of use
	AddRportal,				// procedure for creating one of these missiles
	MI_Rportal,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_RPORTAL,				// file# of missile gfx table
	LS_SENTINEL,			// missile launch sound
	LS_ELEMENTL },			// missile impact sound

	{ MIT_FIREPLAR,				// #defines name of missile for ease of use
	AddBoom,				// procedure for creating one of these missiles
	MI_Boom,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_FIREPLAR,			// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_DIABAPOCA,		// #defines name of missile for ease of use
	AddDiabApoca,			// procedure for creating one of these missiles
	MI_Dummy,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_MANA,				// #defines name of missile for ease of use
	AddMana,				// procedure for creating one of these missiles
	MI_Dummy,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_FMANA,			// #defines name of missile for ease of use
	AddFMana,				// procedure for creating one of these missiles
	MI_Dummy,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_LIGHTWALL,		// #defines name of missile for ease of use
	AddLightwall,			// procedure for creating one of these missiles
	MI_Lightwall,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_LGHT,				// missile resistance type (fire, light, misc, none)
	MF_LIGHTNING,			// file# of missile gfx table
	LS_LMAG,				// missile launch sound
	LS_ELECIMP1 },			// missile impact sound

	{ MIT_LIGHTWALLC,		// #defines name of missile for ease of use
	AddFirewallC,			// procedure for creating one of these missiles
	MI_LightwallC,			// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_LGHT,				// missile resistance type (fire, light, misc, none)
	MF_LIGHTNING,			// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_IMMOLATION,		// #defines name of missile for ease of use
	AddNova,				// procedure for creating one of these missiles
	MI_FireNova,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_FIRE,				// missile resistance type (fire, light, misc, none)
	MF_FIREBOLT,			// file# of missile gfx table
	LS_FBOLT1,				// missile launch sound
	LS_FIRIMP2 },			// missile impact sound
	
	{ MIT_SPECARROW,		// #defines name of missile for ease of use
	AddSpecialArrow,		// procedure for creating one of these missiles
	MI_SpecialArrow,		// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_WEAP,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_ARROW,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_FBARROW,			// #defines name of missile for ease of use
	AddFBArrow,				// procedure for creating one of these missiles
	MI_Fireball,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_FIRE,				// missile resistance type (fire, light, misc, none)
	MF_FIREBOLT,			// file# of missile gfx table
	PS_FB_BFIRE,			// missile launch sound
	LS_FIRIMP2 },			// missile impact sound
	
	{ MIT_LTARROW,			// #defines name of missile for ease of use
	AddLTArrow,				// procedure for creating one of these missiles
	MI_LTArrow,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_LGHT,				// missile resistance type (fire, light, misc, none)
	MF_LIGHTNING,			// file# of missile gfx table
	PS_FB_BFIRE,			// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_CBARROW,			// #defines name of missile for ease of use
	AddCBArrow,				// procedure for creating one of these missiles
	MI_Cbolt,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_LGHT,				// missile resistance type (fire, light, misc, none)
	MF_CBOLT,				// file# of missile gfx table
	LS_CBOLT,				// missile launch sound
	-1 },					// missile impact sound

	{ MIT_HBARROW,			// #defines name of missile for ease of use
	AddHBArrow,				// procedure for creating one of these missiles
	MI_Hbolt,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_HBOLT,				// file# of missile gfx table
	LS_HOLYBOLT,			// missile launch sound
	LS_ELECIMP1 },			// missile impact sound
	
	{ MIT_TELESTAIRS,		// #defines name of missile for ease of use
	AddTeleStairs,			// procedure for creating one of these missiles
	MI_Teleport,			// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	LS_ETHEREAL,			// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_REFLECT,			// #defines name of missile for ease of use
	AddReflect,				// procedure for creating one of these missiles
	MI_Reflect,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_REFLECTSHLD,			// file# of missile gfx table
	LS_MSHIELD,				// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_BERSERK,			// #defines name of missile for ease of use
	AddBerserk,				// procedure for creating one of these missiles
	MI_Dummy,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_FLAMEBOX,			// #defines name of missile for ease of use
	AddFlameBox,			// procedure for creating one of these missiles
	MI_FlameBox,			// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_FIRE,				// missile resistance type (fire, light, misc, none)
	MF_FIRE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_DISENCHANT,		// #defines name of missile for ease of use
	AddDisEnchant,			// procedure for creating one of these missiles
	MI_Dummy,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_MANAREMOVE,		// #defines name of missile for ease of use
	AddManaRemove,			// procedure for creating one of these missiles
	MI_Dummy,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	IS_CAST7,				// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_LIGHTBOX,			// #defines name of missile for ease of use
	AddFlameBox,			// procedure for creating one of these missiles
	MI_LightBox,			// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_LGHT,				// missile resistance type (fire, light, misc, none)
	MF_LIGHTNING,			// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_SHOWMAGITEMS,		// #defines name of missile for ease of use
	AddShowMagicItems,		// procedure for creating one of these missiles
	MI_ShowMagicItems,		// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_AURA,				// #defines name of missile for ease of use
	AddAura,				// procedure for creating one of these missiles
	MI_Aura,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_MISC,				// missile resistance type (fire, light, misc, none)
	MF_FLASH1,				// file# of missile gfx table
	-1,						// missile launch sound
	LS_ELECIMP1 },			// missile impact sound

	{ MIT_AURA2,				// #defines name of missile for ease of use
	AddAura2,				// procedure for creating one of these missiles
	MI_Aura2,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_MISC,				// missile resistance type (fire, light, misc, none)
	MF_FLASH2,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_SPIRALFIREBALL,	// #defines name of missile for ease of use
	AddSpiralFireBall,		// procedure for creating one of these missiles
	MI_SpiralFireBall,		// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_FIRE,				// missile resistance type (fire, light, misc, none)
	MF_FIREBOLT,			// file# of missile gfx table
	LS_FBOLT1,				// missile launch sound
	LS_FIRIMP2 },			// missile impact sound
	
	{ MIT_RUNEOFFIRE,		// #defines name of missile for ease of use
	AddRuneOfFire,			// procedure for creating one of these missiles
	MI_Rune,	   			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_RUNEHOTSPOT,		  	// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_RUNEOFLIGHT,		// #defines name of missile for ease of use
	AddRuneOfLight,			// procedure for creating one of these missiles
	MI_Rune,	   			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_RUNEHOTSPOT,			// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_RUNEOFNOVA,		// #defines name of missile for ease of use
	AddRuneOfNova,			// procedure for creating one of these missiles
	MI_Rune,	   			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_RUNEHOTSPOT,			// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_RUNEOFIMMOLATION,	// #defines name of missile for ease of use
	AddRuneOfImmolation,	// procedure for creating one of these missiles
	MI_Rune,	   			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_RUNEHOTSPOT,			// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_RUNEOFSTONE,		// #defines name of missile for ease of use
	AddRuneOfStone,			// procedure for creating one of these missiles
	MI_Rune,	   			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_RUNEHOTSPOT,			// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_BIGEXPLOSION,		// #defines name of missile for ease of use
	AddBigExplosion,		// procedure for creating one of these missiles
	MI_BigExplosion,		// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_FIRE,				// missile resistance type (fire, light, misc, none)
	MF_BIGEXP,				// file# of missile gfx table
	LS_BIGEXP,				// missile launch sound
	LS_BIGEXP },			// missile impact sound

	{ MIT_HORKSPAWN,		// #defines name of missile for ease of use
	AddHorkSpawn,			// procedure for creating one of these missiles
	MI_HorkSpawn,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_HORKSPAWN,			// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_RANDOM,			// #defines name of missile for ease of use
	AddRandom,				// procedure for creating one of these missiles
	MI_Dummy,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_OPENNEST,			// #defines name of missile for ease of use
	AddReallyBigExp,		// procedure for creating one of these missiles
	MI_Dummy,				// procedure to run each frame
	FALSE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_NONE,				// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_ORANGEFLARE,  	// #defines name of missile for ease of use
	AddFlare,				// procedure for creating one of these missiles
	MI_Firebolt,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_MISC,				// missile resistance type (fire, light, misc, none)
	MF_ORANGEFLARE,			// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_BLUEFLARE,		// #defines name of missile for ease of use
	AddFlare,				// procedure for creating one of these missiles
	MI_Firebolt,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_MISC,				// missile resistance type (fire, light, misc, none)
	MF_BLUE2FLARE, 			// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_REDFLARE,	  		// #defines name of missile for ease of use
	AddFlare,				// procedure for creating one of these missiles
	MI_Firebolt,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_MISC,				// missile resistance type (fire, light, misc, none)
	MF_REDFLARE,			// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_YELLOWFLARE,		// #defines name of missile for ease of use
	AddFlare,				// procedure for creating one of these missiles
	MI_Firebolt,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_MISC,				// missile resistance type (fire, light, misc, none)
	MF_YELLOWFLARE,			// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_BLUE2FLARE,		// #defines name of missile for ease of use
	AddFlare,				// procedure for creating one of these missiles
	MI_Firebolt,			// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_SPL,				// missile type (weapon, spell, none)
	MIMT_MISC,				// missile resistance type (fire, light, misc, none)
	MF_BLUE2FLARE, 			// file# of missile gfx table
	-1,						// missile launch sound
	-1 },					// missile impact sound

	{ MIT_YELLOWEXPLOSION,	// #defines name of missile for ease of use
	AddMisexp,				// procedure for creating one of these missiles
	MI_Misexp,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_YELLOWEXPLOSION,			// file# of missile gfx table
	LS_FIRIMP2,				// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_REDEXPLOSION,		// #defines name of missile for ease of use
	AddMisexp,				// procedure for creating one of these missiles
	MI_Misexp,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_REDEXPLOSION,		// file# of missile gfx table
	LS_FIRIMP2,				// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_BLUEEXPLOSION,	// #defines name of missile for ease of use
	AddMisexp,				// procedure for creating one of these missiles
	MI_Misexp,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_BLUEEXPLOSION,		// file# of missile gfx table
	LS_FIRIMP2,				// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_BLUE2EXPLOSION,	// #defines name of missile for ease of use
	AddMisexp,				// procedure for creating one of these missiles
	MI_Misexp,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_BLUE2EXPLOSION,		// file# of missile gfx table
	LS_FIRIMP2,				// missile launch sound
	-1 },					// missile impact sound
	
	{ MIT_ORANGEEXPLOSION,	// #defines name of missile for ease of use
	AddMisexp,				// procedure for creating one of these missiles
	MI_Misexp,				// procedure to run each frame
	TRUE,					// draw missile flag
	MIS_NONE,				// missile type (weapon, spell, none)
	MIMT_NONE,				// missile resistance type (fire, light, misc, none)
	MF_ORANGEEXPLOSION,		// file# of missile gfx table
	LS_FIRIMP2,				// missile launch sound
	-1 },					// missile impact sound
	
};


MisFileData misfiledata[] =
{
	{ MF_ARROW,				// #defines name of missile file for ease of use
	1,						// amount of files
	"Arrows",				// path for cels
	MFF_STATIC,					// flags, e.g., MFF_MONSTONLY
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{ 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{ 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_FIREBOLT,			// #defines name of missile file for ease of use
	16,						// amount of files
	"Fireba",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },		// data pointers to anim tables
	{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },		// anim delay amount
	{ 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 },		// number of anim frames
	{ 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96 },		// anim width
	{ 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 } },	// anim width2

	{ MF_GUARDIAN,			// #defines name of missile file for ease of use
	3,						// amount of files
	"Guard",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{  1,  1,  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{ 15, 14,  3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 96, 96, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{ 16, 16, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },		// anim width2

	{ MF_LIGHTNING,			// #defines name of missile file for ease of use
	1,						// amount of files
	"Lghning",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{ 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_FIRE,				// #defines name of missile file for ease of use
	2,						// amount of files
	"Firewal",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },		// data pointers to anim tables
	{   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },		// anim delay amount
	{  13,  11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },		// number of anim frames
	{ 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },		// anim width
	{  32,  32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },	// anim width2

	{ MF_EXP1,				// #defines name of missile file for ease of use
	1,						// amount of files
	"MagBlos",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_TOWN,				// #defines name of missile file for ease of use
	2,						// amount of files
	"Portal",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{  0,  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{ 16, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 96, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{ 16, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_FLASH1,			// #defines name of missile file for ease of use
	1,						// amount of files
	"Bluexfr",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 160, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_FLASH2,			// #defines name of missile file for ease of use
	1,						// amount of files
	"Bluexbk",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 160, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_MANASHLD,			// #defines name of missile file for ease of use
	1,						// amount of files
	"Manashld",				// path for cels
	MFF_STATIC,					// flags, e.g., MFF_MONSTONLY
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{ 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_BLOOD,				// #defines name of missile file for ease of use
	4,						// amount of files
	"Blood",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{ 0,    0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },		// data pointers to anim tables
	{ 0,    0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },		// anim delay amount
	{ 15,   8,   8,   8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },		// number of anim frames
	{ 96, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },		// anim width
	{ 16,  32,  32,  32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },	// anim width2

	{ MF_BONE,				// #defines name of missile file for ease of use
	3,						// amount of files
	"Bone",					// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },		// data pointers to anim tables
	{   2,   2,   2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },		// anim delay amount
	{   8,   8,   8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },		// number of anim frames
	{ 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },		// anim width
	{  32,  32,  32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },		// anim width2

	{ MF_METAL,				// #defines name of missile file for ease of use
	3,						// amount of files
	"Metlhit",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{  2,  2,  2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{ 10, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 96, 96, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{ 16, 16, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },		// anim width2

	{ MF_FARROW,			// #defines name of missile file for ease of use
	16,						// amount of files
	"Farrow",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },		// data pointers to anim tables
	{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },		// anim delay amount
	{  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4 },		// number of anim frames
	{ 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96 },		// anim width
	{ 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 } },	// anim width2

	{ MF_DOOM,				// #defines name of missile file for ease of use
	9,						// amount of files
	"Doom",					// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0 },	// data pointers to anim tables
	{  1,  1,  1,  1,  1,  1,  1,  1,  1, 0, 0, 0, 0, 0, 0, 0 },	// anim delay amount
	{ 15, 15, 15, 15, 15, 15, 15, 15, 15, 0, 0, 0, 0, 0, 0, 0 },	// number of anim frames
	{ 96, 96, 96, 96, 96, 96, 96, 96, 96, 0, 0, 0, 0, 0, 0, 0 },	// anim width
	{ 16, 16, 16, 16, 16, 16, 16, 16, 16, 0, 0, 0, 0, 0, 0, 0 } },	// anim width2

	{ MF_GOLEM,				// #defines name of missile file for ease of use
	1,						// amount of files
	" ",					// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_SPURT,				// #defines name of missile file for ease of use
	2,						// amount of files
	"Blodbur",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{   0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   2,   2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{   8,   8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  32,  32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },		// anim width2

	{ MF_BOOM,				// #defines name of missile file for ease of use
	1,						// amount of files
	"Newexp",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{ 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{ 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_STONE,				// #defines name of missile file for ease of use
	1,						// amount of files
	"Shatter1",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_BIGEXP,			// #defines name of missile file for ease of use
	1,						// amount of files
	"Bigexp",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 160, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_FLAMES,			// #defines name of missile file for ease of use
	1,						// amount of files
	"Inferno",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{  96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_THINLIGHT,			// #defines name of missile file for ease of use
	1,						// amount of files
	"Thinlght",				// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{ 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_FLARE,				// #defines name of missile file for ease of use
	1,						// amount of files
	"Flare",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_FLAREXP,			// #defines name of missile file for ease of use
	1,						// amount of files
	"Flareexp",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{   7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_MAGBALL,			// #defines name of missile file for ease of use
	8,						// amount of files
	"Magball",				// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{   0,   0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0 },		// data pointers to anim tables
	{   1,   1,   1,   1,   1,   1,   1,   1, 0, 0, 0, 0, 0, 0, 0, 0 },		// anim delay amount
	{  16,  16,  16,  16,  16,  16,  16,  16, 0, 0, 0, 0, 0, 0, 0, 0 },		// number of anim frames
	{ 128, 128, 128, 128, 128, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0 },		// anim width
	{  32,  32,  32,  32,  32,  32,  32,  32, 0, 0, 0, 0, 0, 0, 0, 0 } },	// anim width2

	{ MF_KRULL,				// #defines name of missile file for ease of use
	1,						// amount of files
	"Krull",				// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{  96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2


	{ MF_CBOLT,				// #defines name of missile file for ease of use
	1,						// amount of files
	"Miniltng",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_HBOLT,				// #defines name of missile file for ease of use
	16,						// amount of files
	"Holy",					// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{ 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 },		// number of anim frames
	{ 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96 },		// anim width
	{ 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 } },	// anim width2

	{ MF_HEXPL,				// #defines name of missile file for ease of use
	1,						// amount of files
	"Holyexpl",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{   8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 160, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_LARROW,			// #defines name of missile file for ease of use
	16,						// amount of files
	"Larrow",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },		// data pointers to anim tables
	{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },		// anim delay amount
	{  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4 },		// number of anim frames
	{ 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96 },		// anim width
	{ 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 } },	// anim width2

	{ MF_FAEXP,				// #defines name of missile file for ease of use
	1,						// amount of files
	"Firarwex",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{   6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{  64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_ACID,				// #defines name of missile file for ease of use
	16,						// amount of files
	"Acidbf",				// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{   8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 },			// number of anim frames
	{ 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96 },		// anim width
	{ 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 } },	// anim width2

	{ MF_ACIDSPLAT,			// #defines name of missile file for ease of use
	1,						// amount of files
	"Acidspla",				// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{   8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{  96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_ACIDPUD,			// #defines name of missile file for ease of use
	2,						// amount of files
	"Acidpud"	,			// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{   9, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{  96,96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  16,16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_ETHER,				// #defines name of missile file for ease of use
	1,						// amount of files
	"Ethrshld",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{ 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_FIRERUN,			// #defines name of missile file for ease of use
	8,						// amount of files
	"Firerun",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0 },		// data pointers to anim tables
	{  1,  1,  1,  1,  1,  1,  1,  1, 0, 0, 0, 0, 0, 0, 0, 0 },		// anim delay amount
	{ 12, 12, 12, 12, 12, 12, 12, 12, 0, 0, 0, 0, 0, 0, 0, 0 },		// number of anim frames
	{ 96, 96, 96, 96, 96, 96, 96, 96, 0, 0, 0, 0, 0, 0, 0, 0 },		// anim width
	{ 16, 16, 16, 16, 16, 16, 16, 16, 0, 0, 0, 0, 0, 0, 0, 0 } },	// anim width2

	{ MF_RESURRECT,			// #defines name of missile file for ease of use
	1,						// amount of files
	"Ressur1",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{ 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{ 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_BONESPIRIT,		// #defines name of missile file for ease of use
	9,						// amount of files
	"Sklball",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0 },		// data pointers to anim tables
	{  1,  1,  1,  1,  1,  1,  1,  1,  1, 0, 0, 0, 0, 0, 0, 0 },		// anim delay amount
	{ 16, 16, 16, 16, 16, 16, 16, 16,  8, 0, 0, 0, 0, 0, 0, 0 },		// number of anim frames
	{ 96, 96, 96, 96, 96, 96, 96, 96, 96, 0, 0, 0, 0, 0, 0, 0 },		// anim width
	{ 16, 16, 16, 16, 16, 16, 16, 16, 16, 0, 0, 0, 0, 0, 0, 0 } },	// anim width2

	{ MF_RPORTAL,			// #defines name of missile file for ease of use
	2,						// amount of files
	"Rportal",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{ 16, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 96, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{ 16, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_FIREPLAR,				// #defines name of missile file for ease of use
	1,						// amount of files
	"Fireplar",				// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{ 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 160,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{ 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_BFLARE,			// #defines name of missile file for ease of use
	1,						// amount of files
	"Scubmisb",				// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{  96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_BFLAREXP,			// #defines name of missile file for ease of use
	1,						// amount of files
	"Scbsexpb",				// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{   6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_CFLARE,			// #defines name of missile file for ease of use
	1,						// amount of files
	"Scubmisc",				// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{  96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_CFLAREXP,			// #defines name of missile file for ease of use
	1,						// amount of files
	"Scbsexpc",				// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{   6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_DFLARE,			// #defines name of missile file for ease of use
	1,						// amount of files
	"Scubmisd",				// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{  96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_DFLAREXP,			// #defines name of missile file for ease of use
	1,						// amount of files
	"Scbsexpd",				// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{   6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_HORKSPAWN,			// #defines name of missile file for ease of use
	8,						// amount of files
	"spawns",		   		// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{   9, 9, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{  96,96,96,96,96,96,96,96, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  16,16,16,16,16,16,16,16, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_REFLECTSHLD,			// #defines name of missile file for ease of use
	1,						// amount of files
	"reflect",				// path for cels
	MFF_STATIC,					// flags, e.g., MFF_MONSTONLY
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{160, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{160, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },			// anim width2

	{ MF_ORANGEFLARE,		// #defines name of missile file for ease of use
	16,						// amount of files
	"ms_ora",				// path for cels
	MFF_MONSTONLY,	    	// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15 },			// number of anim frames
	{  96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96 },			// anim width
	{  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 } },			// anim width2

	{ MF_BLUEFLARE,	  		// #defines name of missile file for ease of use
	16,						// amount of files
	"ms_bla",				// path for cels
	MFF_MONSTONLY,	   		// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15 },			// number of anim frames
	{  96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96 },			// anim width
	{  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 } },			// anim width2

	{ MF_REDFLARE,	    	// #defines name of missile file for ease of use
	16,						// amount of files
	"ms_reb",				// path for cels
	MFF_MONSTONLY,	    	// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15 },			// number of anim frames
	{  96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96 },			// anim width
	{  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 } },			// anim width2

	{ MF_YELLOWFLARE,	  	// #defines name of missile file for ease of use
	16,						// amount of files
	"ms_yeb",				// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15 },			// number of anim frames
	{  96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96 },			// anim width
	{  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 } },			// anim width2

	{ MF_RUNEHOTSPOT,		// #defines name of missile file for ease of use
	1,						// amount of files
	"rglows1",				// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{  96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{   8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },		// anim width2

	{ MF_YELLOWEXPLOSION,	// #defines name of missile file for ease of use
	1,						// amount of files
	"ex_yel2",				// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 220, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{ 78, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },		// anim width2

	{ MF_BLUEEXPLOSION, 	// #defines name of missile file for ease of use
	1,						// amount of files
	"ex_blu2",				// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 212, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  86, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },		// anim width2

	{ MF_REDEXPLOSION, 		// #defines name of missile file for ease of use
	1,						// amount of files
	"ex_red3",				// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 292, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{ 114, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },		// anim width2

	{ MF_BLUE2FLARE,	  	// #defines name of missile file for ease of use
	16,						// amount of files
	"ms_blb",				// path for cels
	MFF_MONSTONLY,	   		// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15 },			// number of anim frames
	{  96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96 },			// anim width
	{   8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 } },			// anim width2

	{ MF_ORANGEEXPLOSION, 	// #defines name of missile file for ease of use
	1,						// amount of files
	"ex_ora1",				// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{  96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  -12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },		// anim width2

	{ MF_BLUE2EXPLOSION, 	// #defines name of missile file for ease of use
	1,						// amount of files
	"ex_blu3",				// path for cels
	MFF_MONSTONLY,			// flags, e.g., MFF_MONSTONLY
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{ 292, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{ 114, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },		// anim width2

//  END OF LIST INDICATOR
	{ MF_NONE,				// #defines name of missile file for ease of use
	0,						// amount of files
	"",						// path for cels
	NULL,					// flags, e.g., MFF_MONSTONLY
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// data pointers to anim tables
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim delay amount
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// number of anim frames
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			// anim width
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }			// anim width2
};

