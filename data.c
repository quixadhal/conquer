/*conquer : Copyright (c) 1988 by Ed Barlow.
 *  I spent a long time writing this code & I hope that you respect this.
 *  I give permission to alter the code, but not to copy or redistribute
 *  it without my explicit permission.  If you alter the code,
 *  please document changes and send me a copy, so all can have it.
 *  This code, to the best of my knowledge works well,  but it is my first
 *  'C' program and should be treated as such.  I disclaim any
 *  responsibility for the codes actions (use at your own risk).  I guess
 *  I am saying "Happy gaming", and am trying not to get sued in the process.
 *                                                Ed
 */

/* THIS FILE CONTAINS STRING DATA DEFINITIONS */

/*include files*/
#include "header.h"
#include "data.h"

/*initialization data*/
/*Movement costs*/
char *ele=       "~#^%-0";
/* BELOW ARE MOVEMENT COSTS FOR TERRAIN */
char *HElecost=  "EE321E";
char *OElecost=  "EE211E";
char *EElecost=  "EE631E";
char *DElecost=  "EE211E";
char *elename[]= {  "WATER","PEAK", "MOUNTAIN", "HILL", "FLAT","ERROR"};

/* BELOW ARE MOVEMENT COSTS FOR VEG */
char *veg=       "vdyblgwfjsi~0";
char *HVegcost=  "83210000232EE";	/* Human */
char *OVegcost=  "52100012517EE";	/* Orcs  */
char *EVegcost=  "86221000017EE";
char *DVegcost=  "57100013577EE";
int vegfood[]=	{ 0,0,0,4,6,9,7,3,0,0,0,0 };
char *vegname[]= { "VOLCANO", "DESERT", "TUNDRA", "BARREN", "LT VEG",
"GOOD", "WOOD", "FOREST", "JUNGLE", "SWAMP", "ICE", "NONE"};
char *des=       "tcmfx$!-0";
char *desname[]= {"TOWN", "CAPITOL", "MINE", "FARM", "DEVASTATED", "GOLDMINE",
"CASTLE", "NODESIG", "PEAK", "WATER"};

char *Class[]= { "NPC", "king", "emperor", "wizard", "priest", "pirate",
"trader", "tyrant", "demon", "dragon", "shadow"};
char *races[]= { "GOD","ORC","ELF","DWARF","LIZARD",
"HUMAN","PIRATE","BARBARIAN","NOMAD","UNKNOWN"};
char *diploname[]= { "UNMET", "CONFDRCY", "ALLIED", "FRIENDLY",
"NEUTRAL", "HOSTILE", "WAR", "JIHAD"};
char *soldname[]= { "","MARCH","SCOUT","ATTACK","DEFEND","GARRISON","TRADED"};

char *unittype[]= {
"Militia", "Goblin", "Orc", "Infantry", "Sailor", "Marines", "Archer",
"Uruk-Hai", "Ninja", "Phalanx", "Olog-Hai", "Legion", "Dragoon", "Mercenary",
"Troll", "Elite", "Lt_cav", "Cavalry", "Catapult", "Seige", "Roc",
"Knight", "Griffon","Elephant",
"Leader",
"Spirit", "Assasin", "Efreeti", "Gargoyle", "Wraith",
"Hero", "Centaur", "Giant", "Superhero", "Mummy",
"Elemental", "Minotarur", "Demon", "Balrog", "Dragon"
};

/*short unit types...for printing in upper right corner*/
char *shunittype[]= {
"mlta", "Gob", "Orc", "Inf", "Sail", "XMar", "Arch",
"Uruk", "Ninj", "Phax", "olog", "Legn", "Drag", "Merc",
"Trol", "Elit", "lCav", "Cav", "cat", "sge", "Roc",
"Kni", "grif","ele",
"Lea",
"spir", "Assn", "efr", "Garg", "Wra",
"Hero", "Cent", "gt", "Shro", "Mumm",
"Elem", "mino", "dem", "Bal", "Drag"
};
int unitminsth[]=		/* min strength of a unit */
{
1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1,
1, 1, 1,
1,
50, 50, 50, 75, 75,
100, 50, 150, 150, 150,
175, 150, 500, 500, 1000
};

int unitattack[]=
{ -25, -15,   0,   0,   0,  25,   0,
5,  20,  10,  15,  20,  10,  20,
25,  20,  10,  30, -20, -20,  20,
50,  40, 50,
0,
0, 20, 10, 10, 10,
0, 10, 0, 15, 15,
5, 20, 50, 40, 50
};
#ifdef ADMIN
int unitdefend[]=
{
-25, -15,   0,   0,   0,  25,  10,
5,   0,  10,  15,  20,  10,  15,
15,  20,  10,  30,  20,  20,  30,
50,  50, 50,
0,
0, 20, 10, 10, 10,
0, 10, 0, 15, 15,
5, 20, 50, 40, 50
};
/*ten times the move rate of a unit*/
int unitmove[]=
{
 0, 10, 10, 10, 0, 0, 10,
10, 10, 10, 10, 10, 20, 10,
10, 13, 15, 20, 5, 5, 10,
20, 15, 5,
0,
10, 10, 15, 10, 10,
10, 15, 10, 10, 10,
15, 10, 10, 15, 20
};
#endif ADMIN
/*iron cost of a unit*/
int u_eniron[]=
{
  0,  80,  80, 100, 100,   0, 100,
150, 150, 150, 150, 150, 100,   0,
200, 200, 100, 300,1000,1000, 300,
600, 400, 600,
0,
0, 0, 0, 0, 0,
0, 0, 0, 0, 0,
0, 0, 0, 0, 0
};
/*enlist cost for a unit*/
int u_encost[]=
{
  40,  70,  90,  90,  90,  90,  90,	/* gold cost for enlistment */
 120, 130, 150, 200, 200, 250, 300,
 300, 300, 400, 600, 750, 750, 700,
1000,1200 , 1000,
0,
2, 2, 2, 2, 2,			/* spell points for enlistment */
2, 2, 5, 5, 5,
5, 5,10,10,15
};
/*maintenance cost for a unit*/
int unitmaint[]= {
 10,  20,  40,  40,  40,  40,  40,
 40,  80,  60,  80,  80, 100, 100,
120, 120, 200, 250, 300, 300, 350,
400, 500, 400,
0,			/* GOLD COST PER EQUIVALENT MAN FOR MONSTERS */
100, 40, 40, 30, 30,	/* AND 5 x JEWELS COST PER MAN */
10, 20, 70, 15 ,35,
55, 70, 60, 60, 50
};

char	*exefile =  "execute";
char	*datafile = "data";
char	*msgfile =  "messages";
#ifdef ADMIN
char	*npcsfile = "nations";
#endif ADMIN
char	*helpfile = "help";
char	*newsfile = "news";
char	*isonfile = "lock";
#ifdef TRADE
char	*tradefile = "commerce";
#endif TRADE

char	*pwrname[] = {
"WARRIOR", "CAPTAIN", "WARLORD", "ARCHER", "CAVALRY", "SAPPER",
"ARMOR", "AVIAN", "MI_MONST", "AV_MONST", "MA_MONST",
/*CIVILIAN POWERS */
"SLAVER", "DERVISH", "HIDDEN", "ARCHITECT", "HEALER", "MINER",
"BREEDER", "URBAN", "STEEL", "NINJA", "SAILOR", "DEMOCRACY", "ROADS",
/* MAGICAL SKILLS */
"THE_VOID", "KNOWALL", "DESTROYER", "VAMPIRE",
"SUMMON", "WYZARD", "SORCERER", "ERROR"
};

long	powers[] = {
	WARRIOR,	/*MILITARY POWERS => START=0*/
	CAPTAIN,	/*1*/
	WARLORD,	/*2*/
	ARCHER,		/*3*/
	CAVALRY,	/*4*/
	SAPPER,		/*5*/
	ARMOR,		/*6*/
	AVIAN,		/*7*/
	MI_MONST,	/*8*/
	AV_MONST,	/*9*/
	MA_MONST,	/*10th power; Mil power #11*/

	SLAVER,		/*CIVILIAN POWERS => START=11*/
	DERVISH,	/*12*/
	HIDDEN,		/*13*/
	ARCHITECT,	/*14*/
	HEALER,		/*15*/
	MINER,		/*16*/
	BREEDER,	/*17*/
	URBAN,		/*18*/
	STEEL,		/*19*/
	NINJA,		/*20*/
	SAILOR,		/*21*/
	DEMOCRACY,	/*22*/
	ROADS,		/* 13th CIVILIAN POWER (23rd total) */

	THE_VOID,	/* MAGICAL SKILLS => START=24 */
	KNOWALL,	/*25*/
	DESTROYER,	/*26*/
	VAMPIRE,	/*27*/
	SUMMON,		/*28*/
	WYZARD,		/*29*/
	SORCERER,	/*30th Entry => 31 total powers => End=7*/
	0
};

FILE *fnews, *fopen();
struct sprd_sht spread;
