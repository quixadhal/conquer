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

/*include files*/
#include <ctype.h>
#include "header.h"
#include "data.h"
#include <signal.h>
#include <fcntl.h>
#include  <pwd.h>

/*Declarations*/
char	*getpass();
struct	s_sector sct[MAPX][MAPY];
struct	nation ntn[NTOTAL];   /* player nation stats */
/*is sector occupied by an army?*/
char	occ[MAPX][MAPY];
short	movecost[MAPX][MAPY];
long	startgold=0;

/*offset of upper left hand corner*/
short	xoffset=0,yoffset=0;
/*current cursor postion (relative to 00 in upper corner)*/
/*	position is 2*x,y*/
short	xcurs=0,ycurs=0;
/*display state*/
short	dismode=2;
short	selector=0;  /*selector (y vbl) for which army/navy... is "picked"*/
short	pager=0;     /*pager for selector 0,1,2,3*/
/* nation id of owner*/
short	country=0;

FILE *fexe, *fopen();

int
main(argc,argv)
int argc;
char **argv;
{
	int geteuid(), getuid(), setuid();
	register int i;
	char *name;
	void srand();
	int getopt();
	long time();
	/*mflag = makeworld, a=add player, x=execute, p=print, h=help, s=score*/
	int mflag, aflag, xflag, pflag;
	char string[80];
	extern char *optarg;
	char defaultdir[256];
#ifndef OGOD
	int uid;
#endif OGOD
	struct passwd *getpwnam();

#ifdef OGOD
	if ((getuid())!=(getpwnam(LOGIN)->pw_uid))
	{
	     printf("Sorry -- you can not administrate conquer\n");
	     printf("you need to be logged in as %s\n",LOGIN);
	     exit(FAIL);
	}
#endif OGOD

	mflag = aflag = xflag = pflag = 0;
	srand((unsigned) time((long *) 0));
	strcpy(defaultdir, DEFAULTDIR);
	name = string;
	*name = 0;

	/* process the command line arguments */
	while((i=getopt(argc,argv,"maxpd:"))!=EOF) switch(i){
	/* process the command line arguments */
	case 'm':  /* make a new world*/
		mflag++;
		break;
	case 'a': /* anyone with password can add player*/
		aflag++;
		break;
	case 'x': /* execute program*/
		xflag++;
		break;
	case 'p': /* print the map*/
		pflag++;
		break;
	case 'd':
		strcpy(defaultdir, optarg);
		break;
	case '?': /*  print out command line arguments */
		printf("Command line format: %s [-maxp -dDIR]\n",argv[0]);
		printf("\t-m       make a world\n");
		printf("\t-a       add new player\n");
		printf("\t-x       execute program\n");
		printf("\t-d DIR   to use play different game\n");
		printf("\t-p       print a map\n");
		exit(SUCCESS);
	};

	/* now that we have parsed the args, we can got to the
	 * dir where the files are kept and do some work.
	 */
	if (chdir(defaultdir)) {
		printf("unable to change dir to %s\n",defaultdir);
		exit(FAIL);
	}
	if (mflag) {
		makeworld();
		exit(SUCCESS);
	}

	/* read data*/
	readdata();
	verifydata( __FILE__, __LINE__ );

	if (aflag) { /* a new player */
		printf("\n********************************************");
		printf("\n*      PREPARING TO ADD NEW PLAYERS        *");
		printf("\n********************************************\n");
#ifndef OGOD
		if(strncmp(crypt(getpass("\nwhat is conquer super user password:"),SALT),ntn[0].passwd,PASSLTH)!=0) {
			printf("sorry, must be super user to add player\n");
			exit(FAIL);
		}
#endif OGOD
		newlogin();
		exit(SUCCESS);
	} else if (pflag) {	/* print a map of the game */
#ifndef OGOD
		if(strncmp(crypt(getpass("\nwhat is conquer super user password:"),SALT),ntn[0].passwd,PASSLTH)!=0) {
			printf("sorry, must be super user to get map\n");
			exit(FAIL);
		}
#endif OGOD
		printf("what type of map\noptions are\n");
		printf("\t1) altitudes\n\t2) vegetations\n");
		printf("\t3) nations\n\n");
		printf("\tINPUT:");
		scanf("%hd",&dismode);
		if(dismode==1) printele();
		else if(dismode==2) printveg();
		else pr_ntns();
		exit(SUCCESS);
	} else if (xflag) {	/* update the game */
#ifndef OGOD
		uid=getuid();	/* use the real user id */
		if ( uid != (getpwnam(LOGIN))->pw_uid ){
			printf("sorry -- your uid is invalid for updating\n");
			printf("you need to be logged in as %s\n",LOGIN);
			exit(FAIL);
		}
#endif OGOD
		update();
		writedata();
		exit(SUCCESS);
	}
	printf("error: must specify an option\n");

	/*  print out command line arguments */
	printf("Command line format: %s [-maxp -dDIR]\n",argv[0]);
	printf("\t-a       add new player\n");
	printf("\t-d DIR   to use play different game\n");
	printf("\t-m       make a world\n");
	printf("\t-p       print a map\n");
	printf("\t-x       execute program\n");
	exit(SUCCESS);
}
