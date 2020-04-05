//****************************************************************************
//	CreaHero.cpp
//	Diablo UI create new character popup dialog
//	
//	By Frank Pearce
//	created 9.20.96
//****************************************************************************


#include "pch.h"


//****************************************************************************
//****************************************************************************
	// default starting values when this dll was created
	static TUIDEFSTATS sgStats[UI_NUM_CLASSES] = {
		{ 30, 10, 20, 25},		// starting stats for a warrior
		{ 25, 20, 30, 20},		// starting stats for a rogue
		{ 15, 35, 25, 20},		// starting stats for a sorcerer
		{ 25, 15, 25, 20},		// starting stats for a monk
		{ 20, 20, 25, 20},		// starting stats for a bard
		{ 40,  0, 20, 25},		// starting stats for a barbarian
	};


//****************************************************************************
//*
//*  EXPORTED FUNCTIONS
//*
//****************************************************************************


//****************************************************************************
//****************************************************************************
BOOL CALLBACK UiGetDefaultStats(int heroclass, TPUIDEFSTATS defaultstats) {
	if (defaultstats) {
		defaultstats->strength  = sgStats[heroclass].strength;
		defaultstats->magic		= sgStats[heroclass].magic;
		defaultstats->dexterity	= sgStats[heroclass].dexterity;
		defaultstats->vitality	= sgStats[heroclass].vitality;
		return 1;
	}

	return 0;
}


