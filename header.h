/*conquer is copyrighted 1986 by Ed Barlow.*/
/*  MODIFICATION OF THIS FILE IMPLIES THAT THE MODIFIER WILL ACCEPT
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

	5) you agree to use your best efforts to see that any user
		of conquer complies with the terms and conditions stated above.
	6) The above copyright notice will not be tampered with in any form.
*/

/* Special thanks to Brian Beuning and any others who helped me by
      making enhancements and bug reports */

/*--------------PLEASE MODIFY DATA FILE NAMES IN misc.c--------------------*/

/*-------MODIFICATION IS REQUIRED OF THE FOLLOWING 3 DEFINE STATEMENTS-----*/
#define OWNER	"Ed Barlow"	/*administrators name*/
#define LOGIN	"smile"		/*administrators login id !!!IMPORTANT!!!*/
                                /*only the administrative UID may update...*/
#define VERSION "version 2.0"     /*version number of the game*/

/* directory where default data files will be stored */
/* SO ONE GAME CAN EXIST IN DEFAULTDIR, OTHER GAMES IN OTHER DIRECTORIES */
#define DEFAULTDIR "/d7/c7913/smile/oldgame"

/*-------------------MODIFICATION IS OPTIONAL ON THE FOLLOWING-------------*/
#define SYSV 		/*comment this line out if on a BSD machine*/
#define PWATER 45 	/*percentage of water in the world*/
#define MAPX 48    	/*Number of X Sectors on Map (divisible by 8)*/
#define MAPY 48		/*Number of Y Sectors on Map (divisible by 8)*/
#define MAXNTN 28	/*number of nations (including player & npc ntns)*/
			/*but not including pirates...*/
#define MAXPTS    	30 	/*points to buy stuff in beginning with*/
#define MAXARM 		50	/*maximum number of armies per nation*/
#define MAXNAVY		16	/*maximum number of fleets per nation*/

/*THE FOLLOWING SHOULD BE COMMENTED OUT IF YOU DO NOT WANT THE FUNCTION*/
#define LZARD 		/*defined if lizard npc's exist at beginning*/
#define MONSTER 	/*defined if pirates/barbarians/nomads exist*/
#define NPC 		/*defined if non player country exists at beginning*/
#define CMOVE 		/*Defined if you wish the computer to move 
                          Player nations if they forget to move*/
#define BEEP		/*defined if you wish terminal to beep*/
#define HILIGHT		/*defined if your terminal supports inverse video*/
#define SYS5 		/*defined if SYS V. comment out if on a BSD system*/
/*#define SRANDOM*/	/*defined if srandom()/random() functions are
 			   resident on your system */
#define RANEVENT	/*defined if random events possible (not fully tested)*/
#define NORANDEVENTS 3  /*number of random events on average turn*/

/*---BELOW THIS POINT ARE PARAMETERS YOU MIGHT OPTIONALLY WISH TO CHANGE---*/

#define LANDSEE 2	/*how far you can see on land, by armies, by navies*/
#define NAVYSEE 1	/*making these numbers large takes CPU time*/
#define ARMYSEE 2

/*Below taxation rates are in thousand dollars (US 1985) per 100 humans per 
 *point of whatever -- ie 1 person in veg of 9 means produce $70x9=630K
 *thousand dollars */
#define TAXFOOD		100
#define TAXIRON		100
#define TAXGOLD		100
/*city and capital tax rates based on people only (no multipleier)*/
#define TAXCAP		1000
#define TAXCITY		750

#define SHIPMAINT 	1000	/*ship mainatinance cost*/
#define SOLDMAINT 	40	/*soldier mainatinance cost*/
#define DESCOST		300L	/*cost to redesignatie*/
				/*this is also the iron cost for cities*/
#define FORTCOST	1000	/*cost to build a fort point*/
#define ENLISTCOST 	90	/*cost to enlist one troop*/
#define WARSHPCOST 	10000	/*cost to build one warship*/
#define MERSHPCOST 	20000	/*cost to build one merchant*/
#define SHIPHOLD  	300	/*number of humans a merchant can hold*/
#define CITYLIMIT 	8	/*% of ntns people in sector before => city*/
#define MILRATIO 	5	/*ratio mil:civ for non player countries*/
#define MILINCAP  	5	/*ratio (mil in cap):mil for NPCs*/
#define JWL_MGK		100000L	/*jewels needed to use magic*/
#define BRIBE		100000L	/*amount of gold to bribe one level*/
#define IRONORE		7	/*iron per soldier needed for +1% weapons*/


/*--------------------DO NOT TO ALTER BELOW THIS LINE-----------------------*/

#define MAXX		(MAPX / 8)	/*Number of X areas on map*/
#define MAXY 		(MAPY / 8)	/*Number of Y areas on map*/ 
#define NUMAREAS	(MAXX * MAXY)	/*total # areas, MAXX*MAXY*/
#define NTOTAL		(MAXNTN +4) /*MAXNTN+4*/
#define NLIZARD		(MAXNTN )   /*MAXNTN+0*/
#define NNOMAD		(MAXNTN +1) /*MAXNTN+1*/
#define NBARBARIAN 	(MAXNTN +2) /*MAXNTN+2*/
#define NPIRATE		(MAXNTN +3) /*MAXNTN+3*/
