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

extern	int armornvy;

/*Declarations*/
char	fison[20];
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
/*redraw map in this turn if redraw is a 1*/
short	redraw=TRUE;
/*1 if you have quit*/
int done=FALSE;
/*display state*/
short	hilmode=HI_OWN;
short	dismode=DI_DESI;
short	selector=0;  /*selector (y vbl) for which army/navy... is "picked"*/
short	pager=0;     /*pager for selector 0,1,2,3*/
/* nation id of owner*/
short	country=0;
int owneruid;

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
	char passwd[20];
	long time();
	char string[80];
	extern char *optarg;
	char defaultdir[256];
	struct passwd *getpwnam();
	int sflag=0;

	owneruid=getuid();
	srand((unsigned) time((long *) 0));
	strcpy(defaultdir, DEFAULTDIR);
	name = string;
	*name = 0;

	/* process the command line arguments */
	while((i=getopt(argc,argv,"hn:d:s"))!=EOF) switch(i){
	/* process the command line arguments */
	case 'h': /* execute help program*/
		if (chdir(defaultdir)) {
			printf("unable to change dir to %s\n",defaultdir);
			exit(FAIL);
		}
		initscr();
		savetty();
		noecho();
		crmode();			/* cbreak mode */
		signal(SIGINT,SIG_IGN);		/* disable keyboard signals */
		signal(SIGQUIT,SIG_IGN);
		help();
		endwin();
		putchar('\n');
		exit(SUCCESS);
	case 'd':
		strcpy(defaultdir, optarg);
		break;
	case 'n':
		strcpy(name, optarg);
		break;
        case 's': /*print the score*/
		sflag++;
		break;
	case '?': /*  print out command line arguments */
		printf("Command line format: %s [-hs -d DIR -nNAT]\n",argv[0]);
		printf("\t-n NAT   play as nation NAT\n");
		printf("\t-h       print help text\n");
		printf("\t-d DIR   to use play different game\n");
		printf("\t-s       print scores\n");
		exit(SUCCESS);
	};

	/* now that we have parsed the args, we can got to the
	 * dir where the files are kept and do some work.
	 */
	if (chdir(defaultdir)) {
		printf("unable to change dir to %s\n",defaultdir);
		exit(FAIL);
	}

	/* read data*/
	readdata();
	verifydata( __FILE__, __LINE__ );

	if(sflag){
		printscore();
		exit(SUCCESS);
	}

        /*
         *  Set the real uid to the effective.  This will avoid a
         *  number of problems involving file protection if the
         *  executable is setuid.
         */
        (void) setuid (geteuid ()) ;

	/* must be a normal interactive game */
	armornvy=AORN;

	/* identify the player and the country he represents */

	/* get nation name either from command line or by asking
         *     if you fail will give you the name of administrator of game
         */

	/* verify existence of nation*/
	printf("conquer %s: Copyright (c) 1988 by Edward M Barlow\n",VERSION);
	if (*name == 0) {
		printf("what nation would you like to be:");
		scanf("%s",name);
	}
#ifdef OGOD
        if(strcmp(name,"god")==0) {
		if ( owneruid != (getpwnam(LOGIN))->pw_uid ){
		printf("sorry -- you can not login as god\n");
		printf("you need to be logged in as %s\n",LOGIN);
		exit(FAIL);
	}
		strcpy(name,"unowned");
	}
#else
	if(strcmp(name,"god")==0) strcpy(name,"unowned");
#endif OGOD
	country=(-1);
	for(i=0;i<NTOTAL;i++)
		if(strcmp(name,ntn[i].name)==0) country=i;

	if(country==(-1)) {
		printf("name not found\n");
		printf("\nfor rules type <conquer -h>");
		printf("\nfor more information please contact %s\n",OWNER);
		return;
	}

	/*get encrypted password*/
	strcpy(passwd,crypt(getpass("\nwhat is your nations password:"),SALT));
	if((strncmp(passwd,ntn[country].passwd,PASSLTH)!=0)
	&&(strncmp(passwd,ntn[0].passwd,PASSLTH)!=0)) {
		strcpy(passwd,crypt(getpass("\nerror: reenter your nations password:"),SALT));
		if((strncmp(passwd,ntn[country].passwd,PASSLTH)!=0)
		&&(strncmp(passwd,ntn[0].passwd,PASSLTH)!=0)) {
			printf("\nsorry:");
			printf("\nfor rules type <conquer -h>");
			printf("\nfor more information on the system please contact %s\n",OWNER);
			exit(FAIL);
		}
	}

	/* check if user is super-user nation[0] */
	/*	else setup cursor to capitol*/
	if(country==0) {
		printf("welcome super user\n");
		xcurs=1;
		xoffset=0;
		ycurs=1;
		yoffset=0;
	} else {
		printf("\nloading nation %s\n",ntn[country].name);
		if (ntn[country].active==0) {
			printf("\nSorry, for some reason, your country no longer exists.");
			printf("\nIf there is a problem, please contact %s.\n",OWNER);
			exit(FAIL);
		}
		if(aretheyon()==1) {
			printf("\nSorry, country is already logged in.\n");
			printf("Please try again later.\n");
			exit(FAIL);
		}
		execute();
		if(ntn[country].capx>15) {
			xcurs=15;
			xoffset= (ntn[country].capx-15);
		}
		else {
			xcurs= ntn[country].capx;
			xoffset= 0;
		}
		if(ntn[country].capy>10) {
			ycurs=10;
			yoffset= (ntn[country].capy-10);
		}
		else {
			yoffset= 0;
			ycurs= ntn[country].capy;
		}
	}

	updmove(ntn[country].race,country);

	/* open output for future printing*/
	{
	    char filename[80];
	    sprintf(filename,"%s%d",exefile,country);
	    if ((fexe=fopen(filename,"a"))==NULL) {
		    beep();
		    printf("error opening %s\n",filename);
		    unlink(fison);
		    exit(FAIL);
	    }
	}

	/* SET UP THE SCREEN */
	printf("about to set up the screen");
	initscr();
	crmode();			/* cbreak mode */
	signal(SIGINT,SIG_IGN);		/* disable keyboard signals */
	signal(SIGQUIT,SIG_IGN);
        signal(SIGHUP,hangup);		/* must catch hangups */

	prep(country);
	noecho();

	/*main while routine*/
	done=FALSE;
	while (done==FALSE)
	{
		/* check if cursor is out of bounds*/
		coffmap();
		/*get commands, make moves and input command*/
		parse();
	}

	if(country==0) writedata();
	else {
	fprintf(fexe,"L_NGOLD\t%d \t%d \t%ld \t0 \t0 \t%s\n",
		XNAGOLD ,country,ntn[country].tgold,"null");
	fprintf(fexe,"L_NIRON\t%d \t%d \t%ld \t0 \t0 \t%s\n",
		XNAIRON ,country,ntn[country].tiron,"null");
	fprintf(fexe,"L_NJWLS\t%d \t%d \t%ld \t0 \t0 \t%s\n",
		XNARGOLD ,country,ntn[country].jewels,"null");
	}
	/*done so quit*/
	unlink(fison);
	clear();
	printw("quitting\n");
	refresh();
	nocrmode();
	endwin();
	fclose(fexe);
	exit(SUCCESS);
}

/*make the bottom of the screen*/
int
makebottom()
{
	move(LINES-4,0);
	clrtoeol();
	mvprintw(LINES-3,0,"Conquer: %s",VERSION);
	clrtoeol();
	mvaddstr(LINES-1,0,"  type ? for help");
	clrtoeol();
	mvaddstr(LINES-2,0,"  type Q to quit");
	clrtoeol();
	if(country==0) mvaddstr(LINES-3,COLS-20,"nation..GOD  ");
	else {
		mvprintw(LINES-3,COLS-20,"nation...%s",ntn[country].name);
		mvprintw(LINES-2,COLS-20,"treasury.%ld",ntn[country].tgold);
		mvprintw(LINES-1,COLS-20,"score....%ld",ntn[country].score);
	}
}

/* parse */
int
parse()
{
	register int i;
	char name[20];
	char passwd[12];
	int ocountry;

	switch(getch()) {
	case EXT_CMD:	/* extended command */
	        ext_cmd();
		break;
	case '':	/*redraw the screen*/
		redraw=TRUE;
		break;
	case 'a':	/*army report*/
		redraw=TRUE;
		armyrpt(0);
		break;
	case 'A':	/*adjust army*/
		adjarm(-1);
		makebottom();
		break;
	case '1':
	case 'b':	/*move south west*/
		pager=0;
		selector=0;
		xcurs--;
		ycurs++;
		break;
	case 'B':	/*budget*/
		redraw=TRUE;
		budget();
		break;
	case 'c':	/*change nation stats*/
		redraw=TRUE;
		change();
		break;
	case 'C':	/*construct*/
		construct();
		makebottom();
		break;
	case 'd':	/*change display*/
		newdisplay();
		break;
	case 'D':	/*draft*/
		draft();
		makebottom();
		break;
	case 'f': /*report on ships and load/unload*/
		redraw=TRUE;
		fleetrpt();
		break;
	case 'F':	/*go to next army*/
		navygoto();
		break;
	case 'g':	/*group report*/
		redraw=TRUE;
		armyrpt(1);
		break;
	case 'G':	/*go to next army*/
		armygoto();
		break;
	case 'H':	/*scroll west*/
		pager=0;
		selector=0;
		xcurs-=((COLS-22)/4);
		break;
	case '4':
	case 'h':	/*move west*/
		pager=0;
		selector=0;
		xcurs--;
		break;
	case 'J':	/*scroll down*/
		pager=0;
		selector=0;
		ycurs+=((SCREEN_Y_SIZE)/2);
		break;
	case '2':
	case 'j':	/*move down*/
		pager=0;
		selector=0;
		ycurs++;
		break;
	case '8':
	case 'k':	/*move up*/
		pager=0;
		selector=0;
		ycurs--;
		break;
	case 'K':	/*scroll up*/
		pager=0;
		selector=0;
		ycurs-=((SCREEN_Y_SIZE)/2);
		break;
	case '6':
	case 'l':	/*move east*/
		pager=0;
		selector=0;
		xcurs++;
		break;
	case 'L':	/*scroll east*/
		pager=0;
		selector=0;
		xcurs+=((COLS-22)/4);
		break;
	case 'm':	/*move selected item to new x,y */
		mymove();
		makebottom();
		prep(country);
		pager=0;
		selector=0;
		break;
	case 'M':	/*magic*/
		redraw=TRUE;
		domagic();
		break;
	case '3':
	case 'n':	/*move south-east*/
		pager=0;
		selector=0;
		ycurs++;
		xcurs++;
		break;
	case 'N':	/*read newspaper */
		redraw=TRUE;
		newspaper();
		break;
	case 'p':	/*pick*/
		selector+=2;
		if(selector>=10) {
			selector=0;
			pager+=1;
		}
		/*current selected unit is selector/2+5*pager*/
		if((selector/2)+(pager*5)>=units_in_sector(XREAL,YREAL,country)) {
			pager=0;
			selector=0;
		}
		break;
	case 'P':	/*production*/
		redraw=TRUE;
		produce();
		break;
	case 'Q':	/*quit*/
	case 'q':	/*quit*/
		done=TRUE;
		break;
	case 'r':	/*redesignate*/
		redesignate();
		makemap();
		makebottom();
		break;
		/*list*/
	case 'R':	/*Read Messages*/
		redraw=TRUE;
		rmessage();
		refresh();
		break;
	case 's':	/*score*/
		redraw=TRUE;
		showscore();
		break;
	case 'S':	/*diplomacy screens*/
		diploscrn();
		redraw=TRUE;
		break;
	case '9':
	case 'u':	/*move north-east*/
		pager=0;
		selector=0;
		ycurs--;
		xcurs++;
		break;
    	case 'U':	/* scroll north-east */
		pager=0;
		selector=0;
		xcurs+=((COLS-22)/4);
		ycurs-=((SCREEN_Y_SIZE)/2);
		break;
	case 'W':	/*message*/
		redraw=TRUE;
		wmessage();
		break;
	case '7':
	case 'y':	/*move north-west*/
		pager=0;
		selector=0;
		ycurs--;
		xcurs--;
		break;
	case 'Y':	/* scroll north-west */
		pager=0;
		selector=0;
		xcurs-=((COLS-22)/4);
		ycurs-=((SCREEN_Y_SIZE)/2);
		break;
	case 'Z':	/*move civilians up to 2 spaces*/
		redraw=TRUE;
		moveciv();
		break;
	case 'z':	/*login as new user */
#ifdef OGOD
		if (owneruid != (getpwnam(LOGIN))->pw_uid) break;
#endif
		clear();
		redraw=TRUE;
		if(country != 0) {
		fprintf(fexe,"L_NGOLD\t%d \t%d \t%ld \t0 \t0 \t%s\n",
			XNAGOLD ,country,ntn[country].tgold,"null");
		fprintf(fexe,"L_NIRON\t%d \t%d \t%ld \t0 \t0 \t%s\n",
			XNAIRON ,country,ntn[country].tiron,"null");
		fprintf(fexe,"L_NJWLS\t%d \t%d \t%ld \t0 \t0 \t%s\n",
			XNARGOLD ,country,ntn[country].jewels,"null");
		} else
		mvaddstr(0,0,"SUPER-USER: YOUR CHANGES WILL NOT BE SAVED IF YOU DO THIS!!!");
		standout();
		mvaddstr(2,0,"change login to : ");
		standend();
		refresh();
		get_nname(name);

		ocountry=country;
		country=(-1);
		if(strcmp(name,"god")==0) country=0;
		else for(i=1;i<NTOTAL;i++)
			if((strcmp(name,ntn[i].name)==0)&&(ntn[i].active>=1))
				country=i;

		if(country==(-1)) {
			errormsg("name not found");
			country=ocountry;
			break;
		}
		if(country==ocountry){
			errormsg("same country");
			break;
		}

		/*get password*/
		mvaddstr(2,0,"what is your nations password:");
		refresh();
		getstr(passwd);
		strcpy(name,crypt(passwd,SALT));

		if((strncmp(name,ntn[country].passwd,PASSLTH)!=0)
		&&(strncmp(name,ntn[0].passwd,PASSLTH)!=0)){
			errormsg("sorry:  password invalid");
			country=ocountry;
			break;
		}
		unlink(fison);
		if(aretheyon()==1) {
			errormsg("sorry:  country is already logged in.");
			refresh();
			country=ocountry;
			break;
		}

		fclose(fexe);
		/* open output for future printing*/
	 	sprintf(name,"%s%d",exefile,country);
	 	if ((fexe=fopen(name,"a"))==NULL) {
			beep();
			printf("error opening %s\n",name);
			unlink(fison);
			exit(FAIL);
	 	}

		printf("\n");
		readdata();
		execute();

		updmove(ntn[country].race,country);
		/*go to that nations capitol*/
		if(country!=0) {
			if(ntn[country].capx>15) {
				xcurs=15;
				xoffset= (ntn[country].capx-15);
			}
			else {
				xcurs= ntn[country].capx;
				xoffset= 0;
			}
			if(ntn[country].capy>10) {
				ycurs=10;
				yoffset= (ntn[country].capy-10);
			}
			else {
				yoffset= 0;
				ycurs= ntn[country].capy;
			}
		}
		break;
	case '?':	/*display help screen*/
		redraw=TRUE;
		help();
		break;
	default:
		beep();
	}
}

void
makeside()
{
	int i;
	int armbonus=0;
	int found=0,nvyfnd=0;
	int enemy;
	int y;
	short armynum;
	short nvynum;
	int count;
	int nfound=0;
	register struct s_sector	*sptr = &sct[XREAL][YREAL];

	/*clear side if you cant see it as you are out of bounds*/
	if(inch()==' ') {
		for(i=0;i<LINES-3;i++){
			move(i,COLS-21);
			clrtoeol();
		}
		return;
	}

	/*clear top right hand side each new sector*/
	for(count=0;count<11;count++){
		move(count,COLS-21);
		clrtoeol();
	}

	/*check for your armies*/
	count=units_in_sector(XREAL,YREAL,country);
	if(pager*5>count) pager=0;

	/*first army found is #0*/
	/*show armies / navies in range pager*5 to pager*5 + 4*/
	/*so if pager=0 show 0 to 4, pager=2 show 10 to 14*/
	/*current selected unit is selector/2+5*pager*/

	if(count>(5+(pager*5))) mvaddstr(10,COLS-20,"MORE...");

	nfound=0;
	for(armynum=0;armynum<MAXARM;armynum++){
		if((ASOLD>0)&&(AXLOC==XREAL)&&(AYLOC==YREAL)) {
			if((nfound>=pager*5)&&(nfound<=4+(pager*5))) {
				/*print that army to nfound%5*/
				mvaddch((nfound%5)*2,COLS-21,'>');
				if(selector==(nfound%5)*2) standout();
				/*the mv,for gets the highlighting pretty*/
				move((nfound%5)*2,COLS-10);
				for(i=0;i<9;i++) addch(' ');

				mvprintw((nfound%5)*2,COLS-20,"army %d: %d (%s)",armynum,ASOLD,*(shunittype+(ATYPE%100)));
				/*the mv,for gets the highlighting pretty*/
				move((nfound%5)*2+1,COLS-10);
				for(i=0;i<9;i++) addch(' ');

				mvprintw((nfound%5)*2+1,COLS-20," mv:%d st:%s",AMOVE,*(soldname+ASTAT));
				standend();
			}
			nfound++;
		}
		if((occ[XREAL][YREAL]!=0)
		&&(occ[XREAL][YREAL]!=country)
		&&((sptr->owner==country)||((ASOLD>0)&&(AXLOC<=XREAL+1)
		&&(AXLOC>=XREAL-1)&&(AYLOC<=YREAL+1)&&(AYLOC>=YREAL-1))))
			found=1;
	}

	if(nfound<4+(pager*5)) for(nvynum=0;nvynum<MAXNAVY;nvynum++){
		if(((NWAR+NMER)!=0)&&(NXLOC==XREAL)&&(NYLOC==YREAL)) {
			if((nfound>=pager*5)&&(nfound<=4+(pager*5))) {
				/*print a navy*/
				mvaddch((nfound%5)*2,COLS-21,'>');
				if(selector==(nfound%5)*2) standout();
				mvprintw((nfound%5)*2,COLS-20,"navy %d: move %d",nvynum,NMOVE);
				mvprintw((nfound%5)*2+1,COLS-20," war:%d mer:%d",NWAR,NMER);
				standend();
			}
			nfound++;
		}
		if((occ[XREAL][YREAL]!=0)&&(occ[XREAL][YREAL]!=country)
		&&(sptr->altitude==WATER)
		&&(NWAR+NMER>0)&&(NXLOC<=XREAL+1)&&(NXLOC>=XREAL-1)
		&&(NYLOC<=YREAL+1)&&(NYLOC>=YREAL-1))
			nvyfnd=1;
	}

	count=0;
	if(found==1) for(i=0;i<NTOTAL;i++) {
		if( !magic(i,HIDDEN) || country == 0 ){
			enemy=0;
			for(armynum=0;armynum<MAXARM;armynum++){
				if((i!=country)
				&&(ntn[i].arm[armynum].xloc==XREAL)
				&&(ntn[i].arm[armynum].yloc==YREAL)
				&&(ntn[i].arm[armynum].sold>0))
				enemy+=ntn[i].arm[armynum].sold;
			}
			if(enemy>0) {
				if((magic(country,NINJA)==1) || country == 0 )
					mvprintw(nfound*2+count,COLS-20,"%s: %d men  ",ntn[i].name,enemy);
				else if(magic(i,THE_VOID)==1){
				mvprintw(nfound*2+count,COLS-20,"%s: ?? men  ",ntn[i].name);
				clrtoeol();
				}
				else mvprintw(nfound*2+count,COLS-20,"%s: %d men  ",ntn[i].name,(enemy*(rand()%60+70)/100));
				count++;
			}
		}
	}
	if(nvyfnd==1) for(i=0;i<NTOTAL;i++) {
		if( magic(i,HIDDEN)!=1 || country == 0 )
		for(nvynum=0;nvynum<MAXNAVY;nvynum++)
		if((ntn[i].arm[nvynum].xloc==XREAL)
		&&(ntn[i].arm[nvynum].yloc==YREAL)
		&&(ntn[i].arm[nvynum].sold>0)){
			if((magic(country,NINJA)==1) || country == 0 ){
				mvprintw(nfound*2+count,COLS-20,"%s: %d ships  ",
				ntn[i].name,ntn[i].nvy[nvynum].warships+ntn[i].nvy[nvynum].merchant);
			}
			else if(magic(i,THE_VOID)==1){
			mvprintw(nfound*2+count,COLS-20,"%s: ?? ships",ntn[i].name);
			clrtoeol();
			}
			else mvprintw(nfound*2+count,COLS-20,"%s: %d ships  ",
			ntn[i].name,(ntn[i].nvy[nvynum].warships+ntn[i].nvy[nvynum].merchant)*(rand()%6+7)/10);
			count++;
		}
	}

	standend();
	mvprintw(11,COLS-20,"x is %d  ",XREAL);
	mvprintw(11,COLS-11,"y is %d  ",YREAL);

	if((country!=0)&&(sptr->altitude==WATER)){
		for(y=12;y<=20;y++) mvaddstr(y,COLS-20,"                    ");
		mvaddstr(14,COLS-9,"WATER");
	}
	else {
	if((country!=0)&&(country!=sptr->owner)
	&&(magic(sptr->owner,THE_VOID)==1)){
		for(y=13;y<=20;y++) mvaddstr(y,COLS-20,"                    ");
	}
	else {

		for(y=13;y<=14;y++) mvaddstr(y,COLS-20,"                    ");

		for(i=0;i<=7;i++)
			if(sptr->designation==*(des+i)){
			mvprintw(13,COLS-20,"%s",*(desname+i));
			clrtoeol();
			}

		if((sptr->owner==country)||(country==0))
		mvprintw(15,COLS-20,"people: %6d",sptr->people);
		else
		mvprintw(15,COLS-20,"people: %6d",sptr->people*(rand()%60+70)/100);

		if((sptr->owner==country)
		||(sptr->owner==0)
		||(sptr->owner>=MAXNTN)){
			mvprintw(17,COLS-20,"gold is:   %3d",sptr->gold);
			mvprintw(18,COLS-20,"iron is:   %3d",sptr->iron);
			if(sptr->fortress == 0) {
			mvaddstr(19,COLS-20,"               ");
			} else {
			if(sptr->designation==DCASTLE)
				armbonus=5 * sptr->fortress;
			else
			if(sptr->designation==DCITY){
				if(magic(country,ARCHITECT)==1){
				armbonus+=10+16 * sptr->fortress;
				}
				else armbonus+=10+8 * sptr->fortress;
			}
			else if(sptr->designation==DCAPITOL){
				if(magic(country,ARCHITECT)==1){
				armbonus+=20+20 * sptr->fortress;
				}
				else armbonus+=20+10 * sptr->fortress;
			}
			if(armbonus>0)
			mvprintw(19,COLS-20,"fortress: +%2d%%",armbonus);
			}
		}
		else {
		for(y=17;y<=19;y++) mvaddstr(y,COLS-20,"                    ");
		}
	}

	standout();
	if(sptr->owner==0) mvaddstr(12,COLS-20,"unowned");
	else mvprintw(12,COLS-20,"owner: %s",ntn[sptr->owner].name);
	standend();
	clrtoeol();

	for(i=0;i<=10;i++)
		if(sptr->vegetation==*(veg+i))
		mvprintw(13,COLS-9,"%s",*(vegname+i));

	if(tofood(sptr->vegetation,country) != 0)
		mvprintw(13,COLS-1,"%d",tofood(sptr->vegetation,country));

	if(sptr->owner!=0) for(i=1;i<=8;i++)
		if(ntn[sptr->owner].race==*(races+i)[0]){
		mvprintw(14,COLS-20,"%s",*(races+i));
		clrtoeol();
		}

	for(i=0;(*(ele+i) != '0');i++)
		if( sptr->altitude == *(ele+i) ){
			mvprintw(14,COLS-9,"%s",*(elename+i));
			break;
		}
	}

	if(movecost[XREAL][YREAL]<0)
	mvaddstr(16,COLS-20,"YOU CAN'T ENTER HERE");
	else
	mvprintw(16,COLS-20,"move cost:  %2d      ",movecost[XREAL][YREAL]);
}

/* returns 1 if they are on, else 0 */
int
aretheyon()
{
	FILE *fon;
	/* set up a detection feature for when a country */
	/* is logged on... to prevent multiple logins    */
	sprintf(fison,"%s%d",isonfile,country);
	if((fon=fopen(fison,"r"))==NULL) {
		fon=fopen(fison,"w");
		fprintf(fon,"Arg!");
		fclose(fon);
		return(0);
	} else {
		return(1);
	}
}
