
/*--------I DO BELIEVE IT IS NECESSARY TO ALTER THIS FILE-------------------*/

#include <curses.h>
#define FLAT 1		/*1 if world is flat 0 if round - not implemented*/
/*#define NPCWATER	Can NPC's move on water*/

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

/*vegetation types --  these are legal in designations too*/
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
	char  vegetation; /*sector vegetation  */
	short owner;      /*nation id of owner, MAXNTN+? is SPECIAL*/
	int   people;     /*civilians in sector*/
	short gold;	  /*gold production ability*/
	short fortress;   /*fortification level (0 to 9)*/
	short iron;	  /*amount of iron produced by sector*/
};
#define SOWN 	sct[xcurs+xoffset][ycurs+yoffset].owner
#define XREAL 	(xcurs+xoffset)
#define YREAL 	(ycurs+yoffset)

#define SCREEN_X_SIZE	( (COLS - 21) / 2 )
#define SCREEN_Y_SIZE	( LINES - 5 )

#define PASSLTH 7     /*one less than the characters in the password*/
#define NAMELTH 9     /*one less than the characters in the name*/
#define LEADERLTH 9   /*one less than the characters in the leader*/

/*IN THE FUTURE ARMIES AND NAVIES SHOULD BE IN THE SAME STRUCTURE (UNITS)
*units could include cavalry, militia, navies, armies, garrisons, and leaders
*struct unit
*{
*	short unittype
*	short type1
*	short type2
*	short type3
*	short xloc
*	short yloc
*	short smove
*	short status
*}
*/

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
	int sold; 
	short stat;
};

struct	 nation    /* player nation stats */
    	{
    	char   name[NAMELTH+1];/* name */
    	char   passwd[PASSLTH+1];/* password */
    	char   leader[LEADERLTH+1];/* leader title */
    	char   race;	/* national race (integer--see header.h)*/
    	short  class;	/* national class*/
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
    	long   tgold;	/* gold in treasury */
    	long   jewels;	/* raw amount of gold in treasury */
    	long   tmil;	/* total military */
    	long   tciv;	/* total civilians */
    	long   tiron;	/* total real iron in nation*/
    	long   tfood;	/* total food in nation*/
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

extern  struct s_sector sct[MAPX][MAPY];
extern  struct nation ntn[MAXNTN+4];
extern  short movecost[MAPX][MAPY];
/*is sector occupied, if MAXNTN+1 2+ armies occupy*/
extern	char	occ[ MAPX ][ MAPY ];

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
#define DESTRY 25

#define DESTROY fprintf(fexe,"DESTROY \t%d \t%d \t%hd \t0 \t0 \t%s\n",DESTRY,save,country,"null")
#define CHGMGK fprintf(fexe,"L_MAGIC \t%d \t%hd \t%ld \t%d \t0 \t%s\n",CHG_MGK,country,ntn[country].powers,x,"null")
#define I_APLUS fprintf(fexe,"INC_APLUS \t%d \t%hd \t0 \t0 \t0 \t%s\n",INCAPLUS,country,"null")
#define I_DPLUS fprintf(fexe,"INC_DPLUS \t%d \t%hd \t0 \t0 \t0 \t%s\n",INCDPLUS,country,"null")
#define AADJSTAT fprintf(fexe,"A_STAT \t%d \t%hd \t%d \t%d \t0 \t%s\n",XASTAT,country,armynum,ntn[country].arm[armynum].stat,"null")
#define AADJMEN  fprintf(fexe,"L_ADJMEN \t%d \t%hd \t%d \t%d \t0 \t%s\n",XAMEN ,country,armynum,ntn[country].arm[armynum].sold,"null")
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

#define	WARRIOR		0x000001
#define	CAPTAIN		0x000002
#define	WARLORD		0x000004
#define	MI_MONST	0x000008
#define	AV_MONST	0x000010
#define	MA_MONST	0x000020
#define	SPY		0x000040
#define	KNOWALL		0x000080
#define	DERVISH		0x000100
#define	DESTROYER	0x000200
#define	HIDDEN		0x000400
#define	THE_VOID	0x000800
#define	ARCHITECT	0x001000
#define	VAMPIRE		0x002000
#define	HEALER		0x004000
#define MINER		0x008000
#define URBAN		0x010000
#define STEEL		0x020000
#define ARCHER		0x040000
#define CAVALRY		0x080000
#define BREEDER		0x100000

#define MAXPOWER	21 /* number of powers */

/*my first macro: which will return 1 if the nation has that power*/
#define magic(NATION,POWER)	((ntn[NATION].powers&(POWER))!=0)  

#define ONMAP	(x>=0 && y>=0 && x<MAPX && y<MAPY)

#ifndef HILIGHT
#define standout()		
#endif
  
#ifdef BEEP
#define beep()		putchar('\007')
#else
#define beep()		
#endif
  
#ifdef SRANDOM
#define rand()  	random()
#define srand(x)	srandom(x)
#endif 

#define FOODTHRESH 3
#define GOLDTHRESH 10
#define JEWELTHRESH 10
	
#define SALT "aa"

/*intialize subroutine types*/
char *crypt();
void exit();
int rand();

extern char *strcpy(), *strncpy(), *strcat(), *strncat();

/*file name definitions */
extern	char	*exefile;
extern	char	*datafile;
extern	char	*msgfile;
extern	char	*npcsfile;
extern	char	*helpfile;
extern	char	*newsfile;
