/* conquer : Copyright (c) 1988 by Ed Barlow. */
/* MODIFICATION OF THIS FILE IMPLIES THAT THE MODIFIER WILL ACCEPT
	A LIMITED USE COPYRIGHT AS FOLLOWS:

	1) This software is copyrighted and protected by law.  The
		sole owner of this software, which hereafter is known as
		"conquer" is Edward M. Barlow, who hereby grants you a
		personal, non-exclusive right to use this software.
	2) conquer may not be redistributed in any form.  Any requests for
		new software shall, for now, be the perogative of the author.
	3) loss or damage caused by this software shall not be
		the responsibility of the author.
	4) Ed Barlow shall be notified of enhancements to this software via
		electronic mail and, if there is no response, via US mail to:

			Ed Barlow
			562 Clubhouse Dr,
			Middletown NJ 07748

		My home phone is 201-671-2896.  Use it sparingly and call
		before 11PM if it is important.
	5) you agree to use your best efforts to see that any user
		of conquer complies with the terms and conditions stated above.
	6) The above copyright notice will not be tampered with in any form.
*/

/* Special thanks to Brian Beuning, Adam Bryant, and any others who helped
	me by making enhancements and bug reports */

/*--------------PLEASE MODIFY DATA FILE NAMES IN misc.c--------------------*/

/*-------MODIFICATION IS REQUIRED OF THE FOLLOWING 2 DEFINE STATEMENTS-----*/
#define OWNER	"Ed Barlow"	/*administrators name*/
#define LOGIN	"smile"		/*administrators login id !!!IMPORTANT!!!*/
                                /*only the administrative UID may update...*/


/*-------------------MODIFICATION IS OPTIONAL ON THE FOLLOWING-------------*/
#define VERSION "version 3.0"   /* version number of the game */
#define SYSV		/*comment this line out if on a BSD machine*/
#define PWATER 45	/*percentage of water in the world*/
#define MAPX 48 	/*Number of X Sectors on Map (divisible by 8)*/
#define MAPY 48		/*Number of Y Sectors on Map (divisible by 8)*/
			/*MAPX and MAPY must be >= 24*/
#define MAXNTN 28	/*number of nations (including player & npc ntns)*/
			/*but not including pirates...*/
#define MAXPTS 	40	/*points to buy stuff in beginning with*/
#define MAXARM		50	/*maximum number of armies per nation*/
#define MAXNAVY		16	/*maximum number of fleets per nation*/

/*THE FOLLOWING SHOULD BE COMMENTED OUT IF YOU DO NOT WANT THE FUNCTION*/
/*#define HIDELOC		/*defined if news is not to report sectors*/
#define OGOD		/*defined if you wish to enhance god mode*/
			/* this gives features like god passwords... */
#define DERVDESG	/*allow DERVISH to redesignate in a DESERT/ICE */
#define LZARD		/*defined if lizard npc's exist at beginning*/
#define MONSTER	/*defined if pirates/barbarians/nomads exist*/
#define CHEAT		/* npcs will cheat to keep up */
#define STORMS		/* have storms strike fleets */
#define VULCANIZE	/* add in volcano eruptions.... */
#define PVULCAN 20      /* % chance of eruption each round (see above)*/
/* #define ORCTAKE      /* define if want orcs to takeover orc NPCS */
#define TAKEPRICE 500000L /* price for orc takeover (see above) in jewels */
#define PMOUNT 20       /* % of land that is mountains */
#define PSTORM 3        /* % chance that a storm will strike a fleet */
			/* unless it is in harbor */
#define NPC		/*defined if non player country exists at beginning*/
#define CMOVE		/*Defined if you wish the computer to move
                          Player nations if they forget to move*/
#define BEEP		/*defined if you wish terminal to beep*/
#define HILIGHT		/*defined if your terminal supports inverse video*/
#define RANEVENT	/*defined if random events possible (not fully tested)*/
#define NORANDEVENTS 3  /*number of random events on average turn*/

/*---BELOW THIS POINT ARE PARAMETERS YOU MIGHT OPTIONALLY WISH TO CHANGE---*/

#define LANDSEE 2	/*how far you can see on land, by armies, by navies*/
#define NAVYSEE 1	/*making these numbers large takes CPU time*/
#define ARMYSEE 2

/*Below taxation rates are in thousand dollars (US 1985) per 100 humans per
 *point of whatever -- ie 1 person in veg of 9 means produce $70x9=630K
 *thousand dollars */
#define TAXFOOD		40L
#define TAXIRON		60L
#define TAXGOLD		70L
/*City and Capitol tax rates based on people only (no multipleier)*/
#define TAXCAP		1000L
#define TAXCITY		750L

#define SHIPMAINT	5000L	/*ship mainatinance cost*/

#define TOMANYPEOPLE	3500L	/* too many people in sector - 1/2 repro */
				/* and 1/2 production - not in cities/capitol*/
#define TOMUCHMINED	100000L	/* 100% chance of droping mine value of sector*/
				/* actual chance is prorated */
#define DESFOOD		4	/* food value of sector to redesignate it */

/* min soldiers to take sector */
#define TAKESECTOR	max(75,(ntn[country].tmil/150))

#define	FINDPERCENT	15	/* percent to find gold/iron in sector */
#define DESCOST		5000L	/*cost to redesignatie*/
				/*this is also the iron cost for cities*/
#define FORTCOST	1000L	/*cost to build a fort point*/
#define WARSHPCOST	80000L	/*cost to build one warship*/
#define MERSHPCOST	80000L	/*cost to build one merchant*/
#define	SHIPCREW	50	/* full strength crew on a ship */
#define SHIPHOLD	300	/*number of humans a merchant can hold*/
#define CITYLIMIT	8	/*% of ntns people in sector before => city*/
#define CITYPERCENT	20	/* percent of npc pop able to live in cities */
/* note that militia are not considered military below */
#define MILRATIO	8	/* ratio civ:mil for non player countries*/
#define MILINCAP	10	/*ratio (mil in cap):mil for NPCs*/
#define	MILINCITY	10	/* militia in city/cap = people/MILINCITY */
#define NPCTOOFAR	15	/* npcs should not stray this far from cap */
#define BRIBE		100000L	/*amount of gold to bribe one level*/
#define IRONORE		7	/*iron per soldier needed for +1% weapons*/

/*--------------------Extended command definitions--------------------------*/
#define EXT_CMD '\033'		/* ESC to start extended command */

/*--------------------DO NOT TO ALTER BELOW THIS LINE-----------------------*/

#define MAXX		(MAPX / 8)	/*Number of X areas on map*/
#define MAXY		(MAPY / 8)	/*Number of Y areas on map*/
#define NUMAREAS	(MAXX * MAXY)	/*total # areas, MAXX*MAXY*/
#define NTOTAL		(MAXNTN +4) /*MAXNTN+4*/
#define NLIZARD		(MAXNTN )   /*MAXNTN+0*/
#define NNOMAD		(MAXNTN +1) /*MAXNTN+1*/
#define NBARBARIAN	(MAXNTN +2) /*MAXNTN+2*/
#define NPIRATE		(MAXNTN +3) /*MAXNTN+3*/
