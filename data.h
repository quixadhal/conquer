/* conquer : Copyright (c) 1988 by Ed Barlow. */

/*--------I DO BELIEVE IT IS NECESSARY TO ALTER THIS FILE-------------------*/
#define	FAIL	1		/* fail return to shell */
#define	SUCCESS	0		/* successful return to shell */
#define	BIG	500000000L	/* BIGGER THAN ANYTHING SHOULD BE */

#include <curses.h>
/* sometimes curses.h defines TRUE ...but doesn't */
/* check if already defined */
#ifndef TRUE
#define	TRUE		1
#define	FALSE		0
#endif

/* these defines are for types of magic */
#define	M_MIL		1
#define	M_CIV		2
#define	M_MGK		3

/*simple contour map definitions*/
#define	WATER		(*(ele+0))
#define	PEAK		(*(ele+1))
#define	MOUNTAIN	(*(ele+2))
#define	HILL		(*(ele+3))
#define	CLEAR		(*(ele+4))

/*racial types*/
#define	GOD		'-'
#define	ORC		'O'
#define	ELF		'E'
#define	DWARF		'D'
#define	LIZARD		'L'
#define	HUMAN		'H'
#define	PIRATE		'P'
#define	BARBARIAN	'B'
#define	NOMAD		'N'
#define	TUNKNOWN	'?'

/*designations*/
#ifdef 1
#define	DCITY		't'
#define	DCAPITOL	'c'
#define	DMINE		'm'
#define	DFARM		'f'
#define	DDEVASTATED	'x'
#define	DGOLDMINE	'$'
#define	DCASTLE		'!'
#define	DNODESIG	'-'
#else
#define	DCITY		(*(des+0))
#define	DCAPITOL	(*(des+1))
#define	DMINE		(*(des+2))
#define	DFARM		(*(des+3))
#define	DDEVASTATED	(*(des+4))
#define	DGOLDMINE	(*(des+5))
#define	DCASTLE		(*(des+6))
#define	DNODESIG	(*(des+7))
#endif

/* nation placement variables */
#define	GREAT		'G'
#define	FAIR		'F'
#define	RANDOM		'R'
#define	OOPS		'X'

/*vegetation types -- these are legal in designations too*/
#define	VOLCANO		(*(veg+0))
#define	DESERT		(*(veg+1))
#define	TUNDRA		(*(veg+2))
#define	BARREN		(*(veg+3))
#define	LT_VEG		(*(veg+4))
#define	GOOD		(*(veg+5))
#define	WOOD		(*(veg+6))
#define	FOREST		(*(veg+7))
#define	JUNGLE		(*(veg+8))
#define	SWAMP		(*(veg+9))
#define	ICE		(*(veg+10))
#define	NONE		(*(veg+11))

/*Diplomacy Variables*/
#define	JIHAD		7
#define	WAR		6
#define	HOSTILE		5
#define	NEUTRAL		4
#define	FRIENDLY	3
#define	ALLIED		2
#define	CONFEDERACY	1
#define	UNMET		0

/*army status*/
#define	MARCH		1	/*March */
#define	SCOUT		2	/*Scouting--will not engage enemy if possible*/
#define	ATTACK		3	/*Attack anybody (Hostile+) within 2 sectors*/
#define	DEFEND		4	/*Defend */
#define	GARRISON	5	/*Garrison--for a town or Capitol */
#define TRADED		6	/*Indicates an army that has been traded*/

struct s_sector
{
	char	designation;	/*designation of sector*/
	char	altitude;	/*sector altitude */
	char	vegetation;	/*sector vegetation	*/
	short	owner;		/*nation id of owner, MAXNTN+? is SPECIAL*/
	int	people;		/*civilians in sector*/
	short	i_people;	/* initial civilians in sector */
	unsigned char	gold;		/*gold production ability*/
	unsigned char	fortress;	/*fortification level (0 to 9)*/
	unsigned char	iron;		/*amount of iron produced by sector*/
};
#define	SOWN	sct[xcurs+xoffset][ycurs+yoffset].owner
#define	XREAL	(xcurs+xoffset)
#define	YREAL	(ycurs+yoffset)

#define	SCREEN_X_SIZE	( (COLS - 21) / 2 )
#define	SCREEN_Y_SIZE	( LINES - 5 )

#define	PASSLTH		7	/*one less than the characters in the password*/
#define	NAMELTH		9	/*one less than the characters in the name*/
#define	LEADERLTH	9	/*one less than the characters in the leader*/

struct navy
{
	short warships;
	short merchant;
	short xloc;
	short yloc;
	short smove;
	short crew;
	short armynum;
};

#define	A_MILITIA	0
#define	A_GOBLIN	1
#define	A_ORC		2
#define	A_INFANTRY	3
#define	A_SAILOR	4
#define	A_MARINES	5
#define	A_ARCHER	6
#define	A_URUK		7
#define	A_NINJA		8
#define	A_PHALANX	9
#define	A_OLOG		10
#define	A_LEGION	11
#define	A_DRAGOON	12
#define	A_MERCENARY	13
#define	A_TROLL		14
#define	A_ELITE		15
#define	A_LT_CAV	16
#define	A_CAVALRY	17
#define	A_CATAPULT	18
#define	A_SEIGE		19
#define	A_ROC		20
#define	A_KNIGHT	21
#define	A_GRIFFON	22
#define	A_ELEPHANT	23
#define	NOUNITTYPES	23	/*number of unit types*/

#define	MINLEADER	124	/* min value of a leader -1*/
#define	A_LEADER	124
#define	MAXLEADER	124	/* max value for leader */

#define	MINMONSTER	225	/* min value of a monster -1*/
#define	SPIRIT		225
#define	ASSASSIN	226
#define	DJINNI		227
#define	GARGOYLE	228
#define	WRAITH		229
#define	HERO		230
#define	CENTAUR		231
#define	GIANT		232
#define	SUPERHERO	233
#define	MUMMY		234
#define	ELEMENTAL	235
#define	MINOTAUR	236
#define	DEMON		237
#define	BALROG		238
#define	DRAGON		239
#define	MAXMONSTER	239

struct army
{
	short unittyp;
	short xloc;
	short yloc;
	short smove;
	int sold;
	short stat;
};

struct	nation		/* player nation stats */
{
	char	name[NAMELTH+1];/* name */
	char	passwd[PASSLTH+1];/* password */
	char	leader[LEADERLTH+1];/* leader title */
	char	race;		/* national race (integer--see header.h)*/
	short	class;		/* national class*/
	char	location;	/* location variable gfr */
	char	mark;		/* unique mark for nation*/
	short	capx;		/* Capitol x coord */
	short	capy;		/* Capitol y coord */
	short	active;		/* activity level of nation, not sure if
				i have used this variable consistently:
				1 if PC NATION,
				2+Aggressiveness if NPC,
				0 if inactive,
				999 to represents MONSTER (pirate...)*/
	short	aplus;		/* attack plus of all soldiers*/
	short	dplus;		/* attack plus of all soldiers*/
	short	maxmove;	/* maximum movement of soldiers*/
	short	repro;		/* reproduction rate of nation*/
	long	score;		/* score */
	long	tgold;		/* gold in treasury */
	long	jewels;		/* raw amount of gold in treasury */
	long	tmil;		/* total military */
	long	tciv;		/* total civilians */
	long	tiron;		/* total real iron in nation*/
	long	tfood;		/* total food in nation*/
	long	powers;
	short	spellpts;	/* spell points */
	short	tsctrs;		/* total number sectors */
	short	tships;		/* number warships */
	struct	army arm[MAXARM];
	struct	navy nvy[MAXNAVY];
	short	dstatus[MAXNTN+4];	/*diplomatic status*/
};

#define	ATYPE	ntn[country].arm[armynum].unittyp
#define	AXLOC	ntn[country].arm[armynum].xloc
#define	AYLOC	ntn[country].arm[armynum].yloc
#define	AMOVE	ntn[country].arm[armynum].smove
#define	ASOLD	ntn[country].arm[armynum].sold
#define	ASTAT	ntn[country].arm[armynum].stat
#define	NWAR	ntn[country].nvy[nvynum].warships
#define	NMER	ntn[country].nvy[nvynum].merchant
#define	NCREW	ntn[country].nvy[nvynum].crew
#define	NXLOC	ntn[country].nvy[nvynum].xloc
#define	NYLOC	ntn[country].nvy[nvynum].yloc
#define	NMOVE	ntn[country].nvy[nvynum].smove

extern	struct s_sector	sct[MAPX][MAPY];
extern	struct nation	ntn[MAXNTN+4];
extern	short	movecost[MAPX][MAPY];
/*is sector occupied, if MAXNTN+1 2+ armies occupy*/
extern	char	occ[ MAPX ][ MAPY ];

#define	AORN		0
#define	ARMY		1
#define	NAVY		2

#define	XASTAT		1
#define	XAMEN		2
#define	XALOC		4
#define	XNLOC		5
#define	XNASHP		6
#define	XNACREW		7
#define	XECNAME		8
#define	XECPAS		9
#define EDSPL		10
#define	XSADES		11
#define	XSACIV		12
#define	XSIFORT		13
#define	XNAGOLD		14
#define	XAMOV		15
#define	XNMOV		16
#define	XSAOWN		17
#define	EDADJ		18
#define	XNARGOLD	19
#define	XNAIRON		20
#define	INCAPLUS	22
#define	INCDPLUS	23
#define	CHG_MGK		24
#define	DESTRY		25

#define	DESTROY fprintf(fexe,"DESTROY \t%d \t%d \t%hd \t0 \t0 \t%s\n",DESTRY,save,country,"null")
#define	CHGMGK fprintf(fexe,"L_MAGIC \t%d \t%hd \t%ld \t%ld \t0 \t%s\n",CHG_MGK,country,ntn[country].powers,x,"null")
#define	I_APLUS fprintf(fexe,"INC_APLUS \t%d \t%hd \t0 \t0 \t0 \t%s\n",INCAPLUS,country,"null")
#define	I_DPLUS fprintf(fexe,"INC_DPLUS \t%d \t%hd \t0 \t0 \t0 \t%s\n",INCDPLUS,country,"null")
#define	AADJSTAT fprintf(fexe,"A_STAT \t%d \t%hd \t%d \t%d \t0 \t%s\n",XASTAT,country,armynum,ntn[country].arm[armynum].stat,"null")
#define	AADJMEN	fprintf(fexe,"L_ADJMEN \t%d \t%hd \t%d \t%d \t%d \t%s\n",XAMEN ,country,armynum,ASOLD,ATYPE,"null")
#define	NADJCRW	fprintf(fexe,"N_ASHP \t%d \t%hd \t%d \t%d \t%d \t%s\n",XNACREW,country,nvynum,ntn[country].nvy[nvynum].crew,ntn[country].nvy[nvynum].armynum,"null" )
#define	NADJSHP	fprintf(fexe,"N_ASHP \t%d \t%hd \t%d \t%d \t%d \t%s\n",XNASHP ,country,nvynum,ntn[country].nvy[nvynum].merchant,ntn[country].nvy[nvynum].warships,"null" )
#define	AADJLOC	fprintf(fexe,"A_LOC \t%d \t%hd \t%d \t%d \t%d \t%s\n",XALOC ,country,armynum,ntn[country].arm[armynum].xloc,ntn[country].arm[armynum].yloc,"null")
#define	NADJLOC	fprintf(fexe,"N_LOC \t%d \t%hd \t%d \t%d \t%d \t%s\n",XNLOC ,country,nvynum,ntn[country].nvy[nvynum].xloc,ntn[country].nvy[nvynum].yloc ,"null")
#define	AADJMOV	fprintf(fexe,"A_MOV \t%d \t%hd \t%d \t%d \t0 \t%s\n",XAMOV ,country,armynum,ntn[country].arm[armynum].smove,"null")
#define	NADJMOV	fprintf(fexe,"N_MOV \t%d \t%hd \t%d \t%d \t0 \t%s\n",XNMOV ,country,nvynum,ntn[country].nvy[nvynum].smove,"null")
#define	ECHGNAME fprintf(fexe,"E_CNAME \t%d \t%hd \t0 \t0 \t0 \t%s\n",XECNAME ,country,ntn[country].name)
#define	ECHGPAS	fprintf(fexe,"E_CPAS \t%d \t%hd \t0 \t0 \t0 \t%s\n",XECPAS ,country,crypt(string,SALT))
#define	SADJDES	fprintf(fexe,"S_ADES \t%d \t%hd \t0 \t%d \t%d \t%c\n",XSADES ,country,xcurs+xoffset,ycurs+yoffset,sct[xcurs+xoffset][ycurs+yoffset].designation)
#define	SADJDES2	fprintf(fexe,"S_ADES \t%d \t%hd \t0 \t%d \t%d \t%c\n",XSADES ,country,x,y,sct[x][y].designation)
#define	SADJCIV2	fprintf(fexe,"S_ACIV \t%d \t%hd \t%d \t%d \t%d \t%s\n",XSACIV ,country,sct[i][j].people,i,j,"null")
#define	SADJCIV	fprintf(fexe,"S_ACIV \t%d \t%hd \t%d \t%d \t%d \t%s\n",XSACIV ,country,sct[xcurs+xoffset][ycurs+yoffset].people,xcurs+xoffset,ycurs+yoffset,"null")
#define	INCFORT fprintf(fexe,"SIFORT \t%d \t%hd \t0 \t%d \t%d \t%s\n",XSIFORT ,country,xcurs+xoffset,ycurs+yoffset,"null")
#define	SADJOWN	fprintf(fexe,"S_AOWN \t%d \t%hd \t0 \t%d \t%d \t%s\n",XSAOWN ,country,xcurs+xoffset,ycurs+yoffset,"null")
#define	EADJDIP	fprintf(fexe,"E_ADJ \t%d \t%hd \t%d \t%d \t0 \t%s\n",EDADJ,country,nation,ntn[country].dstatus[nation],"null")
#define EDECSPL  fprintf(fexe,"E_SPL \t%d \t%hd \t%d \t%d \t0 \t%s\n",EDSPL,country,s_cost,0,"null")

#define	S_MIL		0		/* position in powers array */
#define	WARRIOR		0x00000001L
#define	CAPTAIN		0x00000002L
#define	WARLORD		0x00000004L
#define	ARCHER		0x00000008L
#define	CAVALRY		0x00000010L
#define	SAPPER		0x00000020L
#define	ARMOR		0x00000040L
#define	AVIAN		0x00000080L
#define	MI_MONST	0x00000100L
#define	AV_MONST	0x00000200L
#define	MA_MONST	0x00000400L
#define	E_MIL		11
		/*CIVILIAN POWERS */
#define	S_CIV		11
#define	SLAVER		0x00000800L
#define	DERVISH		0x00001000L
#define	HIDDEN		0x00002000L
#define	ARCHITECT	0x00004000L
#define	HEALER		0x00008000L
#define	MINER		0x00010000L
#define	BREEDER		0x00020000L
#define	URBAN		0x00040000L
#define	STEEL		0x00080000L
#define	NINJA		0x00100000L
#define	SAILOR		0x00200000L
#define	DEMOCRACY	0x00400000L
#define	ROADS		0x00800000L
#define	E_CIV		13
			/* MAGICAL SKILLS */
#define	S_MGK		24
#define	THE_VOID	0x01000000L
#define	KNOWALL		0x02000000L
#define	DESTROYER	0x04000000L
#define	VAMPIRE		0x08000000L
#define	SUMMON		0x10000000L
#define	WYZARD		0x20000000L
#define	SORCERER	0x40000000L
#define	E_MGK		7
#define	MAXPOWER	31 /* number of powers */

/*my first macro: which will return 1 if the nation has that power*/
#define	magic(NATION,POWER)	((ntn[NATION].powers&(POWER))!=0)

#define	ONMAP	(x>=0 && y>=0 && x<MAPX && y<MAPY)

#ifndef HILIGHT
#define	standout()
#endif

#ifdef BEEP
#define	beep()		putc('\007',stderr)
#else
#define	beep()
#endif

#ifdef SYSV
#define	rand()		lrand48()
#define	srand(x)	srand48(x)
#else
#define	rand()		random()
#define	srand(x)	srandom(x)
#endif

#define	FOODTHRESH	3
#define	GOLDTHRESH	10
#define	JEWELTHRESH	10

#define	SALT "aa"	/* salt for crypt */


extern	char	*ele;
extern	char	*elename[];
extern	char	*veg;
extern	int	vegfood[];
extern	char	*vegname[];
extern	char	*numbers;
extern	char	*Class[];
extern	char	*races[];
extern	char	*diploname[];
extern	char	*soldname[];
extern	char	*unittype[];
extern	char	*shunittype[];
extern	int	unitminsth[];
extern	int	u_eniron[];
extern	int	u_encost[];
extern	int	unitmaint[];
extern	char	*des;
extern	char	*desname[];
extern	char	*pwrname[];
extern	long	powers[];

#ifdef ADMIN
extern	char	*npcsfile;
extern	int	unitattack[];
extern	int	unitdefend[];
extern	int	unitmove[];
#endif ADMIN

/*file name definitions */
extern	char	*exefile;
extern	char	*datafile;
extern	char	*msgfile;
extern	char	*helpfile;
extern	char	*newsfile;
extern	char	*isonfile;
#ifdef TRADE
extern	char	*tradefile;
#endif TRADE

#define	abrt() { \
fprintf(stderr,"\nSerious Error (File %s, Line %d) - Aborting\n",__FILE__,__LINE__); \
exit(FAIL); \
}

/* extern	everything else */
extern	long exenewmgk(), getmagic(), getmagic(), getmgkcost(), score_one();
extern	struct s_sector *rand_sector();

extern	void adjarm(),armymove(),armyrpt(),atkattr(),blowup();
extern	void budget(),change(),cheat(),coffmap(),combat(),construct();
extern	void defattr(),destroy(),diploscrn(),domagic(),draft(),erupt();
extern	void fight(),fill_edge(),flee(),fleetrpt(),hangup(),help();
extern	void highlight(),makemap(),makeside(),makeworld(),monster(),moveciv();
extern	void mymove(),navalcbt(),newdip(),newdisplay(),newlogin();
extern	void newspaper(),npcredes(),offmap(),place(),populate(),prep();
extern	void printele(),printnat(),printscore(),printveg(),pr_ntns(),produce();
extern	void readdata(),redesignate(),redomil(),reduce(),rmessage(),score();
extern	void see(),showscore(),update(),updmove(),verifydata(),verify_ntn();
extern	void verify_sct(),wmessage(),writedata(),getdstatus(),exit();
#ifdef TRADE
extern	void trade(),uptrade();
#endif TRADE

extern	char	*crypt(),*strcpy(),*strncpy(),*strcat(),*strncat();

#define	HI_OWN		0	/* hilight modes */
#define	HI_ARMY		1
#define	HI_NONE		2
#define	HI_YARM		3
#define	HI_MOVE		4

#define	DI_VEGE		1	/* display modes */
#define	DI_DESI		2
#define	DI_CONT		3
#define	DI_FOOD		4
#define	DI_NATI		5
#define	DI_RACE		6
#define	DI_MOVE		7
#define	DI_DEFE		8
#define	DI_PEOP		9
#define	DI_GOLD		10
#define	DI_IRON		11

extern	struct sprd_sht
{
	/* total product at end of turn */
	long food, gold, jewels, iron;
	/* revenue this turn */
	long revfood, revjewels, reviron, revcap, revcity;
	/* civilians in that type of sector */
	int ingold, iniron, infarm, incity, incap;
	int sectors, civilians;
} spread;
