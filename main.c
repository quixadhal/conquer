/*conquest is copyrighted 1986 by Ed Barlow.
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

/*initialization data*/
/*Movement costs*/
char *ele=       "#^%-~";
char *elename[]= {  "PEAK", "MOUNTAIN", "HILL", "FLAT","WATER"};
char *veg=       "VDW46973JSI~";
char *vegname[]= { "VOLCANO", "DESERT", "WASTE", "BARREN(4)", "LT VEG(6)",
"GOOD (9)", "WOOD (7)", "FOREST(3)", "JUNGLE", "SWAMP", "ICE", "NONE"};
char *numbers=   "0123456789";
char *Class[]= { "NPC", "king", "emperor", "wizard", "priest", "pirate", 
"trader", "tyrant", "demon", "dragon", "shadow"};
char *races[]= { "GOD","ORC","ELF","DWARF","LIZARD",
"HUMAN","PIRATE","BARBARIAN","NOMAD","UNKNOWN"};
char *diploname[]= { "UNMET", "CONFEDERACY", "ALLIED", "FRIENDLY",
"NEUTRAL", "HOSTILE", "WAR", "JIHAD"};
char *soldname[]= { "","MARCH","SCOUT","ATTACK","DEFEND","GARRISON"};
char *des=       "cCmfx$!-";
char *desname[]= {"CITY", "CAPITOL", "MINE", "FARM", "DEVASTATED", "GOLDMINE", 
"CASTLE", "NODESIG", "PEAK", "WATER"};

/*Declarations*/
struct s_sector sct[MAPX][MAPY];
struct nation ntn[NTOTAL];   /* player nation stats */

/*is sector occupied by an army?*/
short occ[MAPX][MAPY];
short movecost[MAPX][MAPY];
extern int armornvy;
int startgold=0;

/*offset of upper left hand corner*/
short xoffset=0,yoffset=0;
/*current cursor postion (relative to 00 in upper corner)*/
/*	position is 2*x,y*/
short xcurs=0,ycurs=0;
/*redraw map in this turn if redraw is a 1*/
short redraw=TRUE;
/*1 if you have quit*/
int done=0;
/*display state*/
short hilmode=0;   /*highlight modes: 0=owned sectors, 1= armies, 2=none*/
short dismode=2;   /*display mode: 1=vegitation, 2=desig, 3=contour*/
/*		 4=armies/navies, 5=commodities, 6=fertility*/
short selector=0;  /*selector (y vbl) for which army/navy... is "picked"*/
/* nation id of owner*/
short country=0;

FILE *fexe, *fopen();

main(argc,argv)
int argc;
char **argv;
{
	register int i;
	char name[20];
	void srand();
	int getopt();
	char passwd[20];
	long time();

	srand((unsigned) time((long *) 0));
	/* process the command line arguments */
	while((i=getopt(argc,argv,"maxph"))!=EOF) switch(i){
	case 'm':  /*make a new world*/
		makeworld();
		exit(1);
	case 'a': /*anyone with password can add player*/
		readdata();
		if(strncmp(crypt(getpass("\nwhat is super user password:"),SALT),ntn[0].passwd,PASSLTH)!=0) {
			printf("sorry, must be super user to add player\n");
			exit(1);
		}
		newlogin();
		exit(1);
	case 'x': /* execute program*/
		readdata();
		update();
		writedata();
		exit(1);
	case 'p': /*print the map*/
		readdata();
		if(strncmp(crypt(getpass("\nwhat is super user password:"),SALT),ntn[0].passwd,PASSLTH)!=0) {
			printf("sorry, must be super user to get map\n");
			exit(1);
		}
		printf("what type of map\noptions are\n");
		printf("\t1) altitudes\n\t2) vegitations\n");
		printf("\t3) nations\n\n");
		printf("\tINPUT:");
		scanf("%hd",&dismode);
		if(dismode==1) printele();
		else if(dismode==2) printveg();
		else pr_ntns();
		exit(1);
	case 'h': /* execute help program*/
		initscr();
		savetty();
		noecho();
		raw();
		help();
		endwin();
		putchar('\n');
		exit(1);
	case '?': /* print out command line arguments */
		printf("Cmd line format: conquest [-maxdhp]\n\t-m   make a world\n\t-a   add new player\n\t-x   execute program\n\t-h   print help text\n\t-p   print a map\n ");
		exit(1);
	};

	/* read data*/
	readdata();
	armornvy=AORN;

	/* identify the player and the country he represents */

	/* get nation name either from command line or by asking
 *     if you fail will give you the name of administrator of game
 */

	/* verify existence of nation*/
	printf("conquest: copyrighted by Ed Barlow (1986)\n");
	printf("what nation would you like to be:");
	scanf("%s",name);
	if(strcmp(name,"god")==0) strcpy(name,"unowned");
	country=(-1);
	for(i=0;i<NTOTAL;i++)
		if(strcmp(name,ntn[i].name)==0) country=i;

	if(country==(-1)) {
		printf("name not found\n");
		printf("\nfor rules type <conquest -h>");
		printf("\nfor more information please contact %s\n",OWNER);
		return;
	}

	/*get encrypted password*/
	strcpy(passwd,crypt(getpass("\nwhat is your password:"),SALT));
	if((strncmp(passwd,ntn[country].passwd,PASSLTH)!=0)&&(strncmp(passwd,ntn[0].passwd,PASSLTH)!=0)) {
		strcpy(passwd,crypt(getpass("\nerror: reenter your password:"),SALT));
		if((strncmp(passwd,ntn[country].passwd,PASSLTH)!=0)&&(strncmp(passwd,ntn[0].passwd,PASSLTH)!=0)) {
			printf("\nsorry:");
			printf("\nfor rules type <conquest -h>");
			printf("\nfor more information on the system please contact %s\n",OWNER);
			exit(1);
		}
	}

	/* check if user is super-user nation[0] */
	/* 	else setup cursor to capitol*/
	if(country==0) {
		printf("welcome super user\n");
		xcurs=1;
		xoffset=0;
		ycurs=1;
		yoffset=0;
	}
	else {
		printf("\nverifing that nation %s exists\n",ntn[country].name);
		startgold = ntn[country].tgold;
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

	updmove(ntn[country].race);

	/* open output for future printing*/
	if ((fexe=fopen(EXEFILE,"a"))==NULL) {
		beep();
		printf("error opening %s\n",EXEFILE);
		exit(1);
	}

	/* SET UP THE SCREEN */
	printf("about to set up the screen");
	initscr();
	savetty();
	raw();

	prep();
	noecho();

	/*main while routine*/
	done=0;
	while (done==0)
	{
		/* check if cursor is out of bounds*/
		if((xcurs<1)||(ycurs<1)||(xcurs>=(COLS-21)/2)||((ycurs>=LINES-5))||((XREAL)>=MAPX)||((YREAL)>=MAPY)) offmap();

		/*update map*/
		if(redraw==TRUE) {
			clear();
			makemap();    /* update map*/
			makebottom();
			redraw=FALSE;
		}
		move(ycurs,2*xcurs);
		makeside(); /*update side*/
		move(ycurs,2*xcurs);
		refresh();
		/*get commands, make moves and input command*/
		parse();
	}

	if(country==0) writedata();
	else {
	fprintf(fexe,"NGOLD\t%d \t%d \t%d \t0 \t0 \t%s\n",XNAGOLD ,country,ntn[country].tgold,"null");
	fprintf(fexe,"NIRON\t%d \t%d \t%d \t0 \t0 \t%s\n",XNAIRON ,country,ntn[country].tiron,"null");
	fprintf(fexe,"NRGOLD\t%d \t%d \t%d \t0 \t0 \t%s\n",XNARGOLD ,country,ntn[country].jewels,"null");
	}
	/*done so quit*/
	clear();
	printw("quitting\n");
	refresh();
	noraw();
	resetty();
	endwin();
	fclose(fexe);
	exit(1);
}

/*make the bottom of the screen*/
makebottom()
{
	move(LINES-4,0);
	clrtoeol();
	mvprintw(LINES-3,0,"Conquest version %d",VERSION);
	clrtoeol();
	mvaddstr(LINES-1,0,"  type ? for help");
	clrtoeol();
	mvaddstr(LINES-2,0,"  type Q to quit");
	clrtoeol();
	if(country==0) mvaddstr(LINES-3,COLS-20,"nation..GOD  ");
	else {
		mvprintw(LINES-3,COLS-20,"nation...%s",ntn[country].name);
		mvprintw(LINES-2,COLS-20,"treasury.%d",ntn[country].tgold);
		mvprintw(LINES-1,COLS-20,"score....%d",ntn[country].score);
	}
}

/* parse */
parse()
{
	register int i;
	char name[20];
	char passwd[12];
	int ocountry;

	switch(getch()) {
	case '':	/*redraw the screen*/
		redraw=TRUE;
		break;
	case 'a':	/*army report*/
		redraw=TRUE;
		armyrpt();
		break;
	case 'A':	/*adjust army*/
		redraw=TRUE;
		adjarm();
		break;
	case 'b':	/*move south west*/
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
		break;
	case 'd':	/*change display*/
		newdisplay();
		break;
	case 'D':	/*draft*/
		draft();
		break;
	case 'f': /*report on ships and load/unload*/
		redraw=TRUE;
		fleetrpt();
		break;
	case 'H':	/*scroll west*/
		xcurs-=((COLS-22)/4);
		break;
	case 'h':	/*move west*/
		xcurs--;
		break;
	case 'J':	/*scroll down*/
		ycurs+=((LINES-5)/2);
		break;
	case 'j':	/*move down*/
		ycurs++;
		break;
	case 'k':	/*move up*/
		ycurs--;
		break;
	case 'K':	/*scroll up*/
		ycurs-=((LINES-5)/2);
		break;
	case 'l':	/*move east*/
		xcurs++;
		break;
	case 'L':	/*scroll east*/
		xcurs+=((COLS-22)/4);
		break;
	case 'm': 	/*move selected item to new x,y */
		redraw=TRUE;
		mymove();
		prep();
		break;
	case 'M': 	/*magic*/
		redraw=TRUE;
		domagic();
		break;
	case 'n':	/*move south-east*/
		ycurs++;
		xcurs++;
		break;
	case 'N':	/*read newspaper */
		redraw=TRUE;
		newspaper();
		break;
	case 'p':	/*pick*/
		selector+=2;
		if((mvinch(selector,COLS-21))!='>') selector=0;
		break;
	case 'P':	/*production*/
		redraw=TRUE;
		produce();
		break;
	case 'Q':	/*quit*/
	case 'q':	/*quit*/
		done=1;
		break;
	case 'r': 	/*redesignate*/
		redraw=TRUE;
		redesignate();
		break;
		/*list*/
	case 'R': 	/*Read Messages*/
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
	case 'u':	/*move north-east*/
		ycurs--;
		xcurs++;
		break;
	case 'W': 	/*message*/
		redraw=TRUE;
		wmessage();
		break;
	case 'y':	/*move north-west*/
		ycurs--;
		xcurs--;
		break;
	case 'Z':	/*move civilians up to 2 spaces*/
		redraw=TRUE;
		moveciv();
		break;
	case 'z':	/*login as new user */
		fprintf(fexe,"NGOLD\t%d \t%d \t%d \t0 \t0 \t%s\n",XNAGOLD ,country,ntn[country].tgold,"null");
		redraw=TRUE;
		fprintf(fexe,"NIRON\t%d \t%d \t%d \t0 \t0 \t%s\n",XNAIRON ,country,ntn[country].tiron,"null");
		fprintf(fexe,"NRGOLD\t%d \t%d \t%d \t0 \t0 \t%s\n",XNARGOLD ,country,ntn[country].jewels,"null");
		standout();
		clear();
		mvaddstr(0,0,"change login to : ");
		standend();
		refresh();
		echo();
		scanw("%s",name);
		noecho();

		ocountry=country;
		country=(-1);
		if(strcmp(name,"god")==0) country=0;
		else for(i=1;i<NTOTAL;i++)
			if((strcmp(name,ntn[i].name)==0)&&(ntn[i].active>=1))
				country=i;

		if(country==(-1)) {
			mvaddstr(2,0,"name not found");
			country=ocountry;
			break;
		}

		/*get password*/
		mvaddstr(2,0,"what is your password:");
		refresh();
		getstr(passwd);
		strcpy(name,crypt(passwd,SALT));

		if((strncmp(name,ntn[country].passwd,PASSLTH)!=0)&&(strncmp(name,ntn[0].passwd,PASSLTH)!=0)){
			mvaddstr(3,0,"sorry:");
			refresh();
			country=ocountry;
			break;
		}

		readdata();
		startgold = ntn[country].tgold;
		execute();
		updmove(ntn[country].race);
		/*go to that nations capital*/
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

makeside()
{
	int i;
	int found=0,nvyfnd=0;
	int enemy;
	int y;
	short armynum;
	short nvynum;
	int count;
	int nfound=0;

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
	for(armynum=0;armynum<MAXARM;armynum++){
		/*show any of your armies in sector*/
		if((ASOLD>0)&&(AXLOC==XREAL)&&(AYLOC==YREAL)) {
			if(nfound<5) {
				mvaddch(nfound*2,COLS-21,'>');
				if(selector==nfound*2) standout();
				mvprintw(nfound*2,COLS-20,"army %d: %d men  ",armynum,ASOLD);
				mvprintw(nfound*2+1,COLS-20," mv:%d st:%s",AMOVE,*(soldname+ASTAT));
				clrtoeol();
				standend();
				nfound++;
			}
			else mvaddstr(10,COLS-20,"MORE...");
		}
		if((occ[XREAL][YREAL]!=0)&&(occ[XREAL][YREAL]!=country)&&((SOWN==country)||((ASOLD>0)&&(AXLOC<=XREAL+1)&&(AXLOC>=XREAL-1)&&(AYLOC<=YREAL+1)&&(AYLOC>=YREAL-1)))) found=1;
	}

	for(nvynum=0;nvynum<MAXNAVY;nvynum++){
		if(((NWAR+NMER)!=0)&&(NXLOC==XREAL)&&(NYLOC==YREAL)) {
			if(nfound<6) {
				mvaddstr(nfound*2,COLS-21,">");
				if(selector==nfound*2) standout();
				mvprintw(nfound*2,COLS-20,"navy %d: move %d",nvynum,NMOVE);
				clrtoeol();
				mvprintw(nfound*2+1,COLS-20," war:%d mer:%d",NWAR,NMER);
				clrtoeol();
				standend();
				nfound++;
			}
			else mvaddstr(10,COLS-20,"MORE...");
		}
		if((occ[XREAL][YREAL]!=0)&&(occ[XREAL][YREAL]!=country)&&(sct[XREAL][YREAL].altitude==WATER)&&(NWAR+NMER>0)&&(NXLOC<=XREAL+1)&&(NXLOC>=XREAL-1)&&(NYLOC<=YREAL+1)&&(NYLOC>=YREAL-1)) nvyfnd=1;
	}

	count=0;
	if(found==1) for(i=0;i<NTOTAL;i++) {
		if(magic(i,HIDDEN)!=1){
			enemy=0;
			for(armynum=0;armynum<MAXARM;armynum++){
				if((i!=country)&&(ntn[i].arm[armynum].xloc==XREAL)&&(ntn[i].arm[armynum].yloc==YREAL)&&(ntn[i].arm[armynum].sold>0)) enemy+=ntn[i].arm[armynum].sold;
			}
			if(enemy>0) {
				if(magic(country,SPY)==1)
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
	if(nvyfnd==1) for(i=0;i<MAXNTN;i++) {
		if(magic(i,HIDDEN)!=1){
			for(nvynum=0;nvynum<MAXNAVY;nvynum++){
				if((ntn[i].arm[nvynum].xloc==XREAL)&&(ntn[i].arm[nvynum].yloc==YREAL)&&(ntn[i].arm[nvynum].sold>0)){
					if(magic(country,SPY)==1){
						mvprintw(nfound*2+count,COLS-20,"%s: %d ships  ",ntn[i].name,ntn[i].nvy[nvynum].warships+ntn[i].nvy[nvynum].merchant);
					}
					else if(magic(i,THE_VOID)==1){
					mvprintw(nfound*2+count,COLS-20,"%s: ?? ships",ntn[i].name);
					clrtoeol();
					}
					else mvprintw(nfound*2+count,COLS-20,"%s: %d ships  ",ntn[i].name,(ntn[i].nvy[nvynum].warships+ntn[i].nvy[nvynum].merchant)*(rand()%6+7)/10);
					count++;
				}
			}
		}
	}

	standend();
	mvprintw(11,COLS-20,"x is %d  ",XREAL);
	mvprintw(11,COLS-11,"y is %d  ",YREAL);

	if((country!=0)&&(sct[XREAL][YREAL].altitude=='~')){
		for(y=12;y<=20;y++) mvaddstr(y,COLS-20,"                    ");
		mvaddstr(14,COLS-9,"WATER");
	}
	else {
	if((country!=0)&&(country!=sct[XREAL][YREAL].owner)&&(magic(sct[XREAL][YREAL].owner,THE_VOID)==1)){
		for(y=13;y<=20;y++) mvaddstr(y,COLS-20,"                    ");
	}
	else {

		for(y=13;y<=14;y++) mvaddstr(y,COLS-20,"                    ");

		for(i=0;i<=7;i++)
			if(sct[XREAL][YREAL].designation==*(des+i)){
			mvprintw(13,COLS-20,"%s",*(desname+i));
			clrtoeol();
			}

		if((sct[XREAL][YREAL].owner==country)||(country==0))
		mvprintw(15,COLS-20,"people: %6d",sct[XREAL][YREAL].people);
		else 	
		mvprintw(15,COLS-20,"people: %6d",sct[XREAL][YREAL].people*(rand()%60+70)/100);

		if((sct[XREAL][YREAL].owner==country)||(sct[XREAL][YREAL].owner==0)||(sct[XREAL][YREAL].owner>=MAXNTN)){
			mvprintw(17,COLS-20,"gold is:   %3d",sct[XREAL][YREAL].gold);
			mvprintw(18,COLS-20,"iron is:   %3d",sct[XREAL][YREAL].iron);
			if(sct[XREAL][YREAL].fortress>0){
			mvprintw(19,COLS-20,"fortress:   %2d",sct[XREAL][YREAL].fortress);
			}
			else mvaddstr(19,COLS-20,"               ");
		}
		else {
		for(y=17;y<=19;y++) mvaddstr(y,COLS-20,"                    ");
		}
	}

	standout();
	if(sct[XREAL][YREAL].owner==0) mvaddstr(12,COLS-20,"unowned");
	else mvprintw(12,COLS-20,"owner: %s",ntn[sct[XREAL][YREAL].owner].name);
	standend();
	clrtoeol();

	for(i=0;i<=10;i++)
		if(sct[XREAL][YREAL].vegitation==*(veg+i))
		mvprintw(13,COLS-9,"%s",*(vegname+i));

	if(sct[XREAL][YREAL].owner!=0) for(i=1;i<=8;i++)
		if(ntn[sct[XREAL][YREAL].owner].race==*(races+i)[0]){
		mvprintw(14,COLS-20,"%s",*(races+i));
		clrtoeol();
		}

	for(i=0;i<=4;i++)
		if(sct[XREAL][YREAL].altitude==*(ele+i))
		mvprintw(14,COLS-9,"%s",*(elename+i));
	}

	if(movecost[XREAL][YREAL]<0) 
	mvaddstr(16,COLS-20,"YOU CAN'T ENTER HERE");
	else
	mvprintw(16,COLS-20,"move cost:  %2d      ",movecost[XREAL][YREAL]);

}

offmap()
{
	redraw=FALSE;
	/*set offset offsets can not be < 0*/
	if(xcurs<1){
		if(XREAL<=0) {
			xoffset=0;
			xcurs=0;
		}
		else {
			redraw=TRUE;
			xoffset-=15;
			xcurs+=15;
		}
	}
	else if(xcurs >= (COLS-22)/2){
		if(XREAL<MAPX) {
			redraw=TRUE;
			xoffset+=15;
			xcurs-=15;
		}
	}
	if(XREAL>=MAPX) xcurs=MAPX-1-xoffset;
	if(xoffset<0) xoffset=0;
	if(xcurs<0) xcurs=0;
	else if(xcurs >= (COLS-22)/2) {
		redraw=TRUE;
		xoffset+=15;
		xcurs-=15;
	}

	if(ycurs<1){
		if(YREAL<=0) {
			yoffset=0;
			ycurs=0;
		}
		else {
			redraw=TRUE;
			ycurs+=15;
			yoffset-=15;
		}
	}
	else if(ycurs >= LINES-5){
		if(YREAL<MAPY) {
			redraw=TRUE;
			yoffset+=15;
			ycurs-=15;
		}
	}
	if(YREAL>=MAPY) ycurs=MAPY-1-yoffset;
	if(yoffset<0) yoffset=0;
	if(ycurs<0) ycurs=0;
	else if(ycurs >= LINES - 5) {
		redraw=TRUE;
		yoffset+=15;
		ycurs-=15;
	}
}
