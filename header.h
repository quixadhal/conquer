/*conquest is copyrighted 1986 by Ed Barlow.*/
/*  MODIFICATION OF THIS FILE IMPLIES THAT THE MODIFIER WILL ACCEPT
 	A LIMITED USE COPYRIGHT AS FOLLOWS:

		1) This software is copyrighted and protected by law.  The
		sole owner of this software, which hereafter is known as
		"conquest" is Edward M. Barlow, who hereby grants you a 
		personal, non-exclusive right to use this software.
 		2) conquest may not be redistributed in any form.  Any
		requests for new software shall, for now, be the perogative
		of the author.
 		3) loss or damage caused by this software shall not be
		the responsibility of the author.
		4) Ed Barlow shall be notified of modifications to this 
		software via electronic mail (mhuxd!smile) and, if there
		is no response, via US mail to:

			Ed Barlow
			974 Delchester Rd
			Newtown Sq. PA, 19073

		5) you agree to use your best efforts to see that any user
		of conquest complies with the terms and conditions stated above.
*/

/*MODIFICATION IS REQUIRED OF THE FOLLOWING DEFINE STATEMENTS*/

/*file name definitions */
#define EXEFILE		"/c03/smile/private/mygame/lib/.execute"
#define DATAFILE 	"/c03/smile/private/mygame/lib/.data"
#define MSGFILE		"/c03/smile/private/mygame/lib/.messages"
#define NPCSFILE 	"/c03/smile/private/mygame/lib/.nations"
#define HELPFILE 	"/c03/smile/private/mygame/lib/.help"
#define NEWSFILE 	"/c03/smile/private/mygame/lib/.news"

#define OWNER	"Ed Barlow"	/*administrators name*/

/*MODIFICATION IS OPTIONAL ON THE FOLLOWING*/
#define PWATER 55 	/*percentage of water in the world*/

#define MAPX 48    	/*Number of X Sectors on Map (divisible by 8 prefered)*/
                        /*  if you change see MAXX and MAXY*/
#define MAPY 48		/*Number of Y Sectors on Map (divisible by 8 prefered)*/
                        /*  if you change see MAXX and MAXY*/
/*the below three variables *must* be based on the above two*/
/*MAXX=MAPX/8, MAXY=MAPY/8, NUMAREAS=MAXX*MAXY*/
#define MAXX 6     	/*Number of X areas on map*/
#define MAXY 6        	/*Number of Y areas on map*/ 
#define NUMAREAS 36   	/*total # areas, MAXX*MAXY*/

/*ALL THESE MUST BE COMMENTED OUT IF YOU WANT TO EXCLUDE THE C CODE*/
#define LZARD 1		/*defined if lizard npc's exist at beginning*/
#define MONSTER 1	/*defined if pirates/barbarians/nomads exist*/
#define NPC 1		/*defined if non player country exists at beginning*/
#define CMOVE 1		/*Comment this line out if you do not wish the computer
			  to move Player nations if they forget to move*/
#define SYS5	1	/*comment this line out if on a BSD system*/
			/*--not that it does anything at the moment*/

#define MAXNTN 28	/*number of nations (including unowned & npc ntns)*/
			/*but not including pirates...*/
#define NTOTAL		32 /*MAXNTN+4*/
#define NLIZARD		28 /*MAXNTN+0*/
#define NNOMAD		29 /*MAXNTN+1*/
#define NBARBARIAN 	30 /*MAXNTN+2*/
#define NPIRATE		31 /*MAXNTN+3*/

#define LANDSEE 2	/*how far you can see on land, by armies, by navies*/
#define NAVYSEE 1	/*making these numbers large takes CPU time*/
#define ARMYSEE 2

/*Below taxation rates are in thousand dollars (US 1985) per 100 humans per 
 *point of whatever -- ie 1 person in veg of 9 means produce $70x9=630K
 *thousand dollars */

#define TAXDEFAULT	5555	/*percent taxes*/
#define TAXFOOD		100
#define TAXIRON		100
#define TAXGOLD		100
/*city and capital tax rates based on people only (no multipleier)*/
#define TAXCAP		1000
#define TAXCITY		750

#define SHIPMAINT 	1000	/*ship mainatinance cost*/
#define SOLDMAINT 	40	/*soldier mainatinance cost*/

#define DESCOST		300	/*cost to redesignatie*/
				/*this is also the iron cost for cities*/
#define FORTCOST	1000	/*cost to build a fort point*/
#define ENLISTCOST 	90	/*cost to enlist one troop*/
#define WARSHPCOST 	3000	/*cost to build one warship*/
#define MERSHPCOST 	2000	/*cost to build one merchant*/

#define SHIPHOLD  	300	/*number of humans a ship can hold*/

#define MAXPTS    	30 	/*points to buy stuff with*/
#define SECTSCORE 	1	/*SECTSCORE points per sector*/
#define CITYLIMIT 	8	/*% of ntns people in sector before => city*/

#define MAXARM 		32	/*maximum number of armies per nation*/
#define MAXNAVY		16	/*maximum number of fleets per nation*/
#define MILRATIO 	5	/*ratio mil:civ for non player countries*/
#define MILINCAP  	5	/*ratio (mil in cap):mil for NPCs*/

#define JWL_MGK		100000L	/*jewels needed to use magic*/
#define BRIBE		100000L	/*amount of gold to bribe one level*/
#define IRONORE		7	/*iron per soldier needed for +1% weapons%/


/*-----------------DO NOT ALTER BELOW THIS LINE-------------------*/

#include <curses.h>
#define VERSION 1     /*version number of the game*/
#define FLAT 1		/*1 if world is flat 0 if round*/

/*simple contour map definitions*/
#define WATER 	'~'
#define PEAK 	'#'	
#define MOUNTAIN '^'
#define HILL 	'%'	
#define CLEAR 	'-'

/*racial types*/
#define GOD 	'-'
#define ORC 	'O'
#define ELF 	'E'
#define DWARF 	'D'
#define LIZARD 	'L'
#define HUMAN 	'H'
#define PIRATE  'P'
#define BARBARIAN 'B'
#define NOMAD  	'N'
#define UNKNOWN '?'

/*designations*/
#define DCITY		'c'
#define DCAPITOL	'C'
#define DMINE		'm'
#define DFARM		'f'
#define DDEVASTATED	'x'
#define DGOLDMINE	'$'
#define DCASTLE		'!'
#define DNODESIG	'-'

#define GREAT	'G'
#define FAIR	'F'
#define RANDOM	'R'
#define OOPS	'X'

/*vegitation types --  these are legal in designations too*/
#define VOLCANO 'V'
#define DESERT 	'D'
#define WASTE  	'W'
#define BARREN 	'4'
#define LT_VEG 	'6'    
#define GOOD 	'9'
#define WOOD 	'7'
#define FORREST '3'
#define JUNGLE 	'J'	
#define SWAMP 	'S'
#define ICE 	'I'	
#define NONE 	'~'

/*Diplomacy Variables*/
#define JIHAD 		7
#define WAR 		6
#define HOSTILE		5
#define NEUTRAL		4
#define FRIENDLY	3
#define ALLIED 		2
#define CONFEDERACY 	1
#define UNMET		0

/*army status*/
# define MARCH 		1	/*March */
# define SCOUT		2	/*Scouting--will not engage enemy if possible*/
# define ATTACK 	3	/*Attack anybody (Hostile+) within 2 sectors*/
# define DEFEND		4	/*Defend */
# define GARRISON 	5	/*Garrison--for a city or Capital*/


struct s_sector
{
	char  designation;/*designation of sector*/
	char  altitude;  /*sector altitude */
	char  vegitation; /*sector vegitation  */
	short owner;      /*nation id of owner, MAXNTN+? is SPECIAL*/
	int   people;     /*civilians in sector*/
	short gold;	  /*gold production ability*/
	short fortress;   /*fortification level (0 to 9)*/
	short iron;	  /*amount of iron produced by sector*/
};
#define XREAL 	xcurs+xoffset
#define YREAL 	ycurs+yoffset
#define SOWN 	sct[xcurs+xoffset][ycurs+yoffset].owner

struct navy
{
	short warships;
	short merchant;
	short xloc;
	short yloc;
	short smove;
};

struct army
{
	short xloc;
	short yloc;
	short smove;
	short sold; 
	short stat;
};

struct	 nation    /* player nation stats */
    	{
    	char   name[10];/* name */
    	char   passwd[8];/* password */
    	char   leader[10];/* leader title */
    	char   race;	/* national race (integer--see header.h)*/
    	short  class;	/* national class*/
/*location could be easily changed out*/
    	char   location;/* location variable gfr */
	char   mark;	/* unique mark for nation*/
    	short  capx;	/* capital x coord */
    	short  capy;	/* capital y coord */
    	short  active;	/* activity level of nation, but I am not sure if
				i have used this variable consistently:
			 	1 if PC NATION, 
				2+Aggressiveness if NPC, 
				0 if inactive,
				999 to represents MONSTER (pirate...)*/
    	short  aplus;	/* attack plus of all soldiers*/
    	short  dplus;	/* attack plus of all soldiers*/
    	short  maxmove;	/* maximum movement of soldiers*/
    	short  repro;	/* reproduction rate of nation*/
    	short  score;	/* score */
    	int    tgold;	/* gold in treasury */
    	int    jewels;	/* raw amount of gold in treasury */
    	int    tmil;	/* total military */
    	int    tciv;	/* total civilians */
    	int    tiron;	/* total real iron in nation*/
    	int    tfood;	/* total food in nation*/
	long   powers;
    	short  tsctrs;	/* total number sectors */
    	short  tships;	/* number warships */
	struct army arm[MAXARM];
	struct navy nvy[MAXNAVY];
	short  dstatus[MAXNTN+4];	/*diplomatic status*/
};

#define AXLOC ntn[country].arm[armynum].xloc 
#define AYLOC ntn[country].arm[armynum].yloc 
#define AMOVE ntn[country].arm[armynum].smove 
#define ASOLD ntn[country].arm[armynum].sold 
#define ASTAT ntn[country].arm[armynum].stat 
#define NWAR  ntn[country].nvy[nvynum].warships
#define NMER  ntn[country].nvy[nvynum].merchant
#define NXLOC ntn[country].nvy[nvynum].xloc 
#define NYLOC ntn[country].nvy[nvynum].yloc 
#define NMOVE ntn[country].nvy[nvynum].smove

extern struct s_sector sct[MAPX][MAPY];
extern struct nation ntn[MAXNTN+4];
extern short movecost[MAPX][MAPY];

/*Movement costs*/
extern char *ele;
extern char *veg;
extern char *numbers;
extern char *races[];
extern char *Class[];
extern char *vegname[];
extern char *diploname[];
extern char *soldname[];

#define AORN 0
#define ARMY 1
#define NAVY 2

#define XASTAT 	1
#define XAMEN 	2
#define XALOC 	4
#define XNLOC 	5 
#define XNASHP 	6 
#define XECNAME	8 
#define XECPAS 	9 
#define XECMARK	10 
#define XSADES 	11 
#define XSACIV 	12 
#define XSIFORT	13 
#define XNAGOLD	14 
#define XAMOV 	15
#define XNMOV 	16 
#define XSAOWN 	17 
#define EDADJ	18
#define XNARGOLD 19
#define XNAIRON 20
#define INCAPLUS 22
#define INCDPLUS 23
#define CHG_MGK 24

#define CHGMGK fprintf(fexe,"MAGIC \t%d \t%hd \t%d \t%d \t0 \t%s\n",CHG_MGK,country,ntn[country].powers,x,"null")
#define I_APLUS fprintf(fexe,"INC_APLUS \t%d \t%hd \t0 \t0 \t0 \t%s\n",INCAPLUS,country,"null")
#define I_DPLUS fprintf(fexe,"INC_DPLUS \t%d \t%hd \t0 \t0 \t0 \t%s\n",INCDPLUS,country,"null")
#define AADJSTAT fprintf(fexe,"A_STAT \t%d \t%hd \t%d \t%d \t0 \t%s\n",XASTAT,country,armynum,ntn[country].arm[armynum].stat,"null")
#define AADJMEN  fprintf(fexe,"A_MEN \t%d \t%hd \t%d \t%d \t0 \t%s\n",XAMEN ,country,armynum,ntn[country].arm[armynum].sold,"null")
#define NADJSHP  fprintf(fexe,"N_ASHP \t%d \t%hd \t%d \t%d \t%d \t%s\n",XNASHP ,country,nvynum,ntn[country].nvy[nvynum].merchant,ntn[country].nvy[nvynum].warships,"null" )
#define AADJLOC  fprintf(fexe,"A_LOC \t%d \t%hd \t%d \t%d \t%d \t%s\n",XALOC ,country,armynum,ntn[country].arm[armynum].xloc,ntn[country].arm[armynum].yloc,"null")
#define NADJLOC  fprintf(fexe,"N_LOC \t%d \t%hd \t%d \t%d \t%d \t%s\n",XNLOC ,country,nvynum,ntn[country].nvy[nvynum].xloc,ntn[country].nvy[nvynum].yloc ,"null")
#define AADJMOV  fprintf(fexe,"A_MOV \t%d \t%hd \t%d \t%d \t0 \t%s\n",XAMOV ,country,armynum,ntn[country].arm[armynum].smove,"null")
#define NADJMOV  fprintf(fexe,"N_MOV \t%d \t%hd \t%d \t%d \t0 \t%s\n",XNMOV ,country,nvynum,ntn[country].nvy[nvynum].smove,"null")
#define ECHGNAME fprintf(fexe,"E_CNAME \t%d \t%hd \t0 \t0 \t0 \t%s\n",XECNAME ,country,ntn[country].name)
#define ECHGPAS  fprintf(fexe,"E_CPAS \t%d \t%hd \t0 \t0 \t0 \t%s\n",XECPAS ,country,crypt(string,SALT))
#define SADJDES  fprintf(fexe,"S_ADES \t%d \t%hd \t0 \t%d \t%d \t%c\n",XSADES ,country,xcurs+xoffset,ycurs+yoffset,sct[xcurs+xoffset][ycurs+yoffset].designation)
#define SADJDES2  fprintf(fexe,"S_ADES \t%d \t%hd \t0 \t%d \t%d \t%c\n",XSADES ,country,x,y,sct[x][y].designation)
#define SADJCIV2  fprintf(fexe,"S_ACIV \t%d \t%hd \t%d \t%d \t%d \t%s\n",XSACIV ,country,sct[i][j].people,i,j,"null")
#define SADJCIV  fprintf(fexe,"S_ACIV \t%d \t%hd \t%d \t%d \t%d \t%s\n",XSACIV ,country,sct[xcurs+xoffset][ycurs+yoffset].people,xcurs+xoffset,ycurs+yoffset,"null")
#define INCFORT fprintf(fexe,"SIFORT \t%d \t%hd \t0 \t%d \t%d \t%s\n",XSIFORT ,country,xcurs+xoffset,ycurs+yoffset,"null")
#define SADJOWN  fprintf(fexe,"S_AOWN \t%d \t%hd \t0 \t%d \t%d \t%s\n",XSAOWN ,country,xcurs+xoffset,ycurs+yoffset,"null")
#define EADJDIP  fprintf(fexe,"E_ADJ \t%d \t%hd \t%d \t%d \t0 \t%s\n",EDADJ,country,nation,ntn[country].dstatus[nation],"null")

#define	WARRIOR		2
#define	CAPTAIN		3
#define	WARLORD		5
#define	MI_MONST	7
#define	AV_MONST	11
#define	MA_MONST	13
#define	SPY		17
#define	KNOWALL		19
#define	DERVISH		23
/*I CAN CHANGE DESTROYER TO NON PRIME NUMBER IN NEXT GAME*/
#define	DESTROYER	29
#define	HIDDEN		31
#define	THE_VOID	37
#define	ARCHITECT	41
#define	VAMPIRE		43
#define	HEALER		47
#define MINER		53
#define URBAN		59
#define STEEL		61
#define ARCHER		67
#define CAVALRY		71
#define BREEDER		73
#define MAXPOWER	73	/*maximum power*/

#define NPCPOWERS	9	/*maximum number of powers for NPC*/
#define PCPOWERS	5	/*maximum number of powers for PC */

/*my first macro: which will return 1 if the nation has that power*/
#define magic(NATION,POWER)	!(ntn[NATION].powers%(POWER))  

#define beep()		putchar('')

#define FOODTHRESH 3
#define GOLDTHRESH 10
#define JEWELTHRESH 10
	
#define SALT "aa"
#define PASSLTH 7

/*intialize subroutine types*/
char *crypt();
void exit();
int rand();

extern char *strcpy(), *strncpy(), *strcat(), *strncat();

