/*io.c*/
/*Print and io subroutines for game*/

/*conquer is copyrighted 1986 by Ed Barlow.
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

/*Declarations*/
extern struct s_sector sct[MAPX][MAPY];
extern struct nation ntn[NTOTAL];   /* player nation stats */

/*offset of upper left hand corner*/
extern short xoffset;
extern short yoffset;
/*current cursor postion (relative to 00 in upper corner)*/
/*	position is 2*x,y*/
extern short xcurs;
extern short ycurs;
/*redraw map in this turn if redraw is a 1*/
extern short redraw;
/*display state*/
extern short hilmode;   /*highlight modes: 0=owned sectors, 1= armies, 2=none*/
extern short dismode;   /*display mode: 1=vegetation, 2=desig, 3=contour*/
/*		 4=armies/navies, 5=commodities, 6=fertility, 7=movement*/
/* nation id of owner*/
extern short country;

/*print a sector.altitude map subroutine*/
printele()
{
	register int X, Y;
	printf("doing print of altitude\n");
	for(Y=0;Y<MAPY;Y++) {
		for(X=0;X<MAPX;X++) putc(sct[X][Y].altitude,stdout);
		putc('\n',stdout);
	}
}

pr_ntns()
{
	register int X, Y;
	printf("doing print of nations\n");
	for(Y=0;Y<MAPY;Y++) {
		for(X=0;X<MAPX;X++) {
			if(sct[X][Y].owner==0)
				putc(sct[X][Y].altitude,stdout);
			else putc(ntn[sct[X][Y].owner].mark,stdout);
		}
		putc('\n',stdout);
	}
}

/*print all data--trashes/creates datafile in the process*/
writedata()
{
	int fd;
	printf("\ndoing write of data\n");
	if((fd = creat(datafile,0666))==-1) {
		printf("cant open data.  check permissions\n");
		return;
	}
	write(fd,sct,sizeof(sct));
	write(fd,ntn,sizeof(ntn));
	close(fd);
}

readdata()
{
	int fd;
	int n_read;

	/*read in existing nation army and navy data*/
	/*check if file openable*/
	printf("reading data file\n");
	if( (fd = open(datafile,0)) < 0 ) {
		fprintf( stderr, "can not open %s \n", datafile );
		exit( 1 );
	}
	if((n_read=read(fd,sct,sizeof(sct)))==0) printf("EOF\n");
	else if(n_read==-1) printf("error reading sector data (sct)\n");
	if(n_read!=sizeof(sct)) {
		printf( "wrong data format (%d vs. %d)\n",n_read, sizeof(sct) );
		exit(1);
	}
	if((n_read=read(fd,ntn,sizeof(ntn)))==0) printf("EOF\n");
	else if(n_read==-1) printf("error reading nation data (ntn)\n");
	if(n_read!=sizeof(ntn)) {
		printf( "wrong data format (%d vs. %d)\n",n_read, sizeof(ntn) );
		exit(1);
	}
	close(fd);
	verifydata( __FILE__, __LINE__ );

} /* readdata() */

/*print a map subroutine*/
printveg()
{
	register int X, Y;
	printf("doing print of vegetation\n");
	for(Y=0;Y<MAPY;Y++) {
		for(X=0;X<MAPX;X++) putc(sct[X][Y].vegetation,stdout);
		putc('\n',stdout);
	}
}

/*make a map*/
makemap()
{
	register int x,y;
	register int i,j;
	short armynum,nvynum;

	/*can you see all?*/
	if((magic(country,KNOWALL)==1)||(country==0)) {
		for(x=0;x<SCREEN_X_SIZE;x++) {
			for(y=0;y<(LINES-4);y++) {
				highlight(x,y);
				see(x,y);
			}
		}
		if((hilmode==1)||(hilmode==4)||(hilmode==3)) {
			for(armynum=0;armynum<MAXARM;armynum++) 
			if((ASOLD>0)&&((AMOVE>0)||(hilmode!=4))){
				standout();
				see(AXLOC-xoffset,AYLOC-yoffset);
			}
		}
	}
	/*see as appropriate?*/
	else {
		for(x=0;x<SCREEN_X_SIZE;x++) for(y=0;y<(LINES-4);y++) {
			if(sct[x+xoffset][y+yoffset].owner==country){
				for(i=x-LANDSEE;i<=x+LANDSEE;i++){
					for(j=y-LANDSEE;j<=y+LANDSEE;j++) {
						highlight(i,j);
						see(i,j);
					}
				}
			}
		}
		for(nvynum=0;nvynum<MAXNAVY;nvynum++) if(NMER+NWAR>0){
			for(i=NXLOC-xoffset-NAVYSEE;i<=NXLOC-xoffset+NAVYSEE;i++) for(j=NYLOC-yoffset-NAVYSEE;j<=NYLOC-yoffset+NAVYSEE;j++){
				highlight(i,j);
				see(i,j);
			}
		}
		for(armynum=0;armynum<MAXARM;armynum++) if(ASOLD>0){
			for(i=AXLOC-xoffset-ARMYSEE;i<=AXLOC-xoffset+ARMYSEE;i++) for(j=AYLOC-yoffset-ARMYSEE;j<=AYLOC-yoffset+ARMYSEE;j++) {
				highlight(i,j);
				see(i,j);
			}
		}
		/*optimal method of highlighting your armies*/
		if((hilmode==3)||(hilmode==4)) {
			for(armynum=0;armynum<MAXARM;armynum++) 
			if((ASOLD>0)&&((AMOVE>0)||(hilmode==3))){
				standout();
				see(AXLOC-xoffset,AYLOC-yoffset);
			}
			for(nvynum=0;nvynum<MAXNAVY;nvynum++) 
			if((NWAR+NMER>0)&&((NMOVE>0)||(hilmode==3))){
				standout();
				see(NXLOC-xoffset,NYLOC-yoffset);
			}
		}
	}
	move(ycurs,2*xcurs);
}

newdisplay()
{
	mvaddstr(LINES-4,0,"viewing options:  (d)esignation, (r)ace, (m)ove cost, (p)eople, (D)efense");
	clrtoeol();
        mvaddstr(LINES-3,0,"                  (c)ontour, (v)egitation, (i)ron, (n)ation mark, (g)old");
	clrtoeol();
	mvaddstr(LINES-2,0,"highlight option: (o)wners, (a)rmy, (y)our Army, (M)ove left, (x)=none");
	clrtoeol();
	standout();
	mvaddstr(LINES-1,0,"what display?:");
	clrtoeol();
	move(LINES-1,16);
	standend();
	refresh();
	redraw=TRUE;
	switch(getch()) {
	case 'v':	/* vegetation map*/
		dismode=1;
		break;
	case 'd':	/* designations map*/
		dismode=2;
		break;
	case 'c':	/* contour map of world */
		dismode=3;
		break;
	case 'n':	/* nations map*/
		dismode=4;
		break;
	case 'r':	/* race map*/
		dismode=5;
		break;
	case 'm':	/* move cost map*/
		dismode=7;
		break;
	case 'D':
		dismode=8;
		break;
	case 'p':
		dismode=9;
		break;
	case 'g':
		dismode=10;
		break;
	case 'i':
		dismode=11;
		break;
	case 'a':	/* armies hilighted map*/
		prep();
		hilmode=1;
		break;
	case 'o':	/* owners hilighted map*/
		hilmode=0;
		break;
	case 'x':	/*no highlighting*/
		hilmode=2;
		break;
	case 'y':	/* your armies hilighted map*/
		prep();
		hilmode=3;
		break;
	case 'M':	/* your armies with moves left hilighted map*/
		prep();
		hilmode=4;
		break;
	default:
		beep();
		redraw=FALSE;
	}
	makebottom();
}

/*see what is in xy as per display mode*/
see(x,y)
{
	int armbonus;
	if((x<0)||(y<0)||(x>COLS-21)||(y>=LINES-4)) return;
	if(((y+yoffset)<MAPY)&&((x+xoffset)<MAPX)) {

		if((dismode>7)&&(country!=sct[x+xoffset][y+yoffset].owner)
		&&(magic(sct[x+xoffset][y+yoffset].owner,THE_VOID)==1)
		&&(country!=0)) {
			standout();
			mvaddch(y,2*x,' ');
			standend();
		}
		else {
			switch(dismode){
			case 1: /*vegetation*/
				mvaddch(y,2*x,sct[x+xoffset][y+yoffset].vegetation);
				break;
			case 2: /*designation*/
				if(sct[x+xoffset][y+yoffset].owner==0){
					if(isdigit(sct[x+xoffset][y+yoffset].vegetation)!=0) mvaddch(y,2*x,sct[x+xoffset][y+yoffset].altitude);
					else mvaddch(y,2*x,sct[x+xoffset][y+yoffset].vegetation);
				}
				else if((country==0)
				||(sct[x+xoffset][y+yoffset].owner==country)) 
				mvaddch(y,2*x,sct[x+xoffset][y+yoffset].designation);
				else mvaddch(y,2*x,ntn[sct[x+xoffset][y+yoffset].owner].mark);
				break;
			case 3: /*contour*/
				mvaddch(y,2*x,sct[x+xoffset][y+yoffset].altitude);
				break;
			case 4: /*ownership*/
				if(sct[x+xoffset][y+yoffset].owner==0)
					mvaddch(y,2*x,sct[x+xoffset][y+yoffset].altitude);
				else mvaddch(y,2*x,ntn[sct[x+xoffset][y+yoffset].owner].mark);
				break;
			case 5: /*race*/
				if(sct[x+xoffset][y+yoffset].owner==0)
					mvaddch(y,2*x,sct[x+xoffset][y+yoffset].altitude);
				else mvaddch(y,2*x,ntn[sct[x+xoffset][y+yoffset].owner].race);
				break;
			case 7:	/*movement cost map*/
				if(movecost[x+xoffset][y+yoffset]>=0) mvprintw(y,2*x,"%d",movecost[x+xoffset][y+yoffset]);
				else if(sct[x+xoffset][y+yoffset].altitude==WATER)
					mvaddch(y,2*x,WATER);
				else
					mvaddch(y,2*x,'X');
				break;
			case 8:   /*Defence*/
				if (sct[x+xoffset][y+yoffset].altitude==WATER)
					mvaddch(y,2*x,WATER);
				else if (movecost[x+xoffset][y+yoffset]<0)
					mvaddch(y,2*x,'*');
				else {

					/*Racial combat bonus due to terrain (the faster you move the better)*/
					armbonus=0;
					armbonus+=5*(9-movecost[x+xoffset][y+yoffset]);

					if(sct[x+xoffset][y+yoffset].altitude==MOUNTAIN) armbonus+=40;
					else if(sct[x+xoffset][y+yoffset].altitude==HILL) armbonus+=20;

					if(sct[x+xoffset][y+yoffset].vegetation==JUNGLE)
						armbonus+=30;
					else if(sct[x+xoffset][y+yoffset].vegetation==FORREST)
						armbonus+=20;
					else if(sct[x+xoffset][y+yoffset].vegetation==WOOD)
						armbonus+=10;

			if((sct[x+xoffset][y+yoffset].designation==DCASTLE)
			||(sct[x+xoffset][y+yoffset].designation==DCITY)
			||(sct[x+xoffset][y+yoffset].designation==DCAPITOL))
						armbonus+=8*sct[x+xoffset][y+yoffset].fortress;

					mvprintw(y,2*x,"%d",armbonus/20);
				}
				break;
			case 9:   /*People*/
				if (sct[x+xoffset][y+yoffset].altitude==WATER)
					mvaddch(y,2*x,WATER);
				else if (sct[x+xoffset][y+yoffset].people>=1000)
					mvaddch(y,2*x,'+');
				else if (sct[x+xoffset][y+yoffset].people>=450)
					mvaddch(y,2*x,'>');
				else if (sct[x+xoffset][y+yoffset].people==0)
					mvaddch(y,2*x,'0');
				else
					mvprintw(y,2*x,"%d",1+sct[x+xoffset][y+yoffset].people/50);
				break;
			case 10:  /*Gold*/
				if (sct[x+xoffset][y+yoffset].altitude==WATER)
					mvaddch(y,2*x,WATER);
				else if(isdigit(sct[x+xoffset][y+yoffset].vegetation)==0) 
					mvaddch(y,2*x,'X');
				else if((sct[x+xoffset][y+yoffset].owner!=0)
				&&(country!=0)
				&&(sct[x+xoffset][y+yoffset].owner!=country))
					mvaddch(y,2*x,'?');
				else if(sct[x+xoffset][y+yoffset].gold>=10)
					mvaddch(y,2*x,'+');
				else
					mvprintw(y,2*x,"%d",sct[x+xoffset][y+yoffset].gold);
				break;
			case 11:  /*Iron*/
				if (sct[x+xoffset][y+yoffset].altitude==WATER)
					mvaddch(y,2*x,WATER);
				else if(isdigit(sct[x+xoffset][y+yoffset].vegetation)==0) 
					mvaddch(y,2*x,'X');
				else if((sct[x+xoffset][y+yoffset].owner!=0)
				&&(country!=0)
				&&(sct[x+xoffset][y+yoffset].owner!=country))
					mvaddch(y,2*x,'?');
				else if (sct[x+xoffset][y+yoffset].iron>=10)
					mvaddch(y,2*x,'+');
				else
					mvprintw(y,2*x,"%d",sct[x+xoffset][y+yoffset].iron);
				break;
			default:
				break;
			}
		}
	}
	else mvaddch(y,2*x,' ');
	standend();
}

/*highlight what is in xy as per highlight mode*/
highlight(x,y)
{
	if((x<0)||(y<0)||(x>COLS-21)||(y>=LINES-4)) return;
	if(((y+yoffset)<MAPY)&&((x+xoffset)<MAPX)) {
		switch(hilmode){
		case 0: /*ownership*/
			if(country==0) {
				if(sct[x+xoffset][y+yoffset].owner>0)
					standout();
			}
			else if(sct[x+xoffset][y+yoffset].owner==country)
				standout();
			break;
		case 1: /*army map*/
			if(occ[x+xoffset][y+yoffset]!=0) standout();
			break;
		default:
			break;
		}
	}
}

/* check if cursor is out of bounds*/
coffmap()
{
	if((xcurs<1)||(ycurs<1)||(xcurs>=SCREEN_X_SIZE)
	||((ycurs>=SCREEN_Y_SIZE))||((XREAL)>=MAPX)
	||((YREAL)>=MAPY)) offmap();
   
	/*update map*/
	if(redraw==TRUE) {
		clear();
		makemap(); /* update map*/
		makebottom();
		redraw=FALSE;
	}
	move(ycurs,2*xcurs);
	makeside();  /*update side*/
	move(ycurs,2*xcurs);
	refresh();
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
	else if(ycurs >= SCREEN_Y_SIZE){
		if(YREAL<MAPY) {
			redraw=TRUE;
			yoffset+=15;
			ycurs-=15;
		}
	}
	if(YREAL>=MAPY) ycurs=MAPY-1-yoffset;
	if(yoffset<0) yoffset=0;
	if(ycurs<0) ycurs=0;
	else if(ycurs >= SCREEN_Y_SIZE) {
		redraw=TRUE;
		yoffset+=15;
		ycurs-=15;
	}
}

printscore()
{
 	int i;
 	int nationid;    	/*current nation id */
 
 	printf("id	race	class	score	gold	militia	people	sectors	name\n");
 	for (nationid=1; nationid<MAXNTN; nationid++) {
 		if(ntn[nationid].active==0) 
 			continue;
 		printf("%d",nationid);
 		for(i=1;i<8;i++)
 			if(ntn[nationid].race==*(races+i)[0])
 				printf("	%s",*(races+i));
 		if(ntn[nationid].active>=2) printf("	NPC");
 		else printf("	%s",*(Class+ntn[nationid].class));
 		printf("	%d	%d	%d	%d	%d",
 			ntn[nationid].score ,ntn[nationid].tgold
 			,ntn[nationid].tmil ,ntn[nationid].tciv
 			,ntn[nationid].tsctrs );
 		printf("	%s\n",ntn[nationid].name);
 	}
}
