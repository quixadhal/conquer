/*Print and io subroutines for interactive game*/

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
/*display state SEE data.h FOR CURRENT VALUES OF THESE */
extern short hilmode;
extern short dismode;
/* nation id of owner*/
extern short country;

/*make a map*/
void
makemap()
{
	register int x,y;
	register int i,j;
	short armynum,nvynum;

	/*can you see all?*/
	if((magic(country,KNOWALL)==1)||(country==0)) {
		for(x=0;x<SCREEN_X_SIZE;x++) {
			for(y=0;y<SCREEN_Y_SIZE;y++) {
				highlight(x,y);
				see(x,y);
			}
		}
		if((hilmode==HI_ARMY)||(hilmode==HI_MOVE)||(hilmode==HI_YARM)) {
			for(armynum=0;armynum<MAXARM;armynum++)
			if((ASOLD>0)&&((AMOVE>0)||(hilmode!=4))){
				standout();
				see(AXLOC-xoffset,AYLOC-yoffset);
			}
		}
	}
	/*see as appropriate?*/
	else {
		for(x=(-LANDSEE);(x<SCREEN_X_SIZE+LANDSEE)&&(x+xoffset<MAPX);x++)
		for(y=(-LANDSEE);(y<SCREEN_Y_SIZE+LANDSEE)&&(y+yoffset<MAPY);y++) {
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
		if((hilmode==HI_YARM)||(hilmode==HI_MOVE)) {
			for(armynum=0;armynum<MAXARM;armynum++)
			if((ASOLD>0)&&((AMOVE>0)||(hilmode==HI_YARM))){
				standout();
				see(AXLOC-xoffset,AYLOC-yoffset);
			}
			for(nvynum=0;nvynum<MAXNAVY;nvynum++)
			if((NWAR+NMER>0)&&((NMOVE>0)||(hilmode==HI_YARM))){
				standout();
				see(NXLOC-xoffset,NYLOC-yoffset);
			}
		}
	}
	move(ycurs,2*xcurs);
}

void
newdisplay()
{
	mvaddstr(LINES-4,0,"viewing options:  (d)esignation, (r)ace, (m)ove cost, (p)eople, (D)efense");
	clrtoeol();
        mvaddstr(LINES-3,0,"          (f)ood, (c)ontour, (v)egetation, (i)ron, (n)ation mark, (j)ewels");
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
	case 'f':
		dismode=DI_FOOD;
		break;
	case 'v':	/* vegetation map*/
		dismode=DI_VEGE;
		break;
	case 'd':	/* designations map*/
		dismode=DI_DESI;
		break;
	case 'c':	/* contour map of world */
		dismode=DI_CONT;
		break;
	case 'n':	/* nations map*/
		dismode=DI_NATI;
		break;
	case 'r':	/* race map*/
		dismode=DI_RACE;
		break;
	case 'm':	/* move cost map*/
		dismode=DI_MOVE;
		break;
	case 'D':
		dismode=DI_DEFE;
		break;
	case 'p':
		dismode=DI_PEOP;
		break;
	case 'j':
		dismode=DI_GOLD;
		break;
	case 'i':
		dismode=DI_IRON;
		break;
	case 'a':	/* armies hilighted map*/
		prep(country);
		hilmode=HI_ARMY;
		break;
	case 'o':	/* owners hilighted map*/
		hilmode=HI_OWN;
		break;
	case 'x':	/*no highlighting*/
		hilmode=HI_NONE;
		break;
	case 'y':	/* your armies hilighted map*/
		prep(country);
		hilmode=HI_YARM;
		break;
	case 'M':	/* your armies with moves left hilighted map*/
		prep(country);
		hilmode=HI_MOVE;
		break;
	default:
		beep();
		redraw=FALSE;
	}
	makebottom();
}

/*see what is in xy as per display mode*/
void
see(x,y)
{
	int armbonus;
	if((x<0)||(y<0)||(x>=SCREEN_X_SIZE)||(y>=SCREEN_Y_SIZE)) return;
	if(((y+yoffset)<MAPY)&&((x+xoffset)<MAPX)) {

		if((magic(sct[x+xoffset][y+yoffset].owner,THE_VOID)==TRUE)
		&&((dismode==DI_DEFE)||(dismode==DI_GOLD)||(dismode==DI_IRON)
			||(dismode==DI_PEOP)||(dismode==DI_FOOD))
		&&(country!=sct[x+xoffset][y+yoffset].owner)
		&&(country!=0)) {
			mvaddch(y,2*x,'?');
		} else {
			switch(dismode){
			case DI_FOOD:	/*food */
				if(tofood(sct[x+xoffset][y+yoffset].vegetation,country)==0)
				mvaddch(y,2*x,sct[x+xoffset][y+yoffset].vegetation);
				else
				mvprintw(y,2*x,"%d",tofood(sct[x+xoffset][y+yoffset].vegetation,country));
				break;
			case DI_VEGE: /*vegetation*/
				mvaddch(y,2*x,sct[x+xoffset][y+yoffset].vegetation);
				break;
			case DI_DESI: /*designation*/
				if(sct[x+xoffset][y+yoffset].owner==0){
					if(tofood(sct[x+xoffset][y+yoffset].vegetation,sct[x+xoffset][y+yoffset].owner)!=0) mvaddch(y,2*x,sct[x+xoffset][y+yoffset].altitude);
					else mvaddch(y,2*x,sct[x+xoffset][y+yoffset].vegetation);
				}
				else if((country==0)
				||(sct[x+xoffset][y+yoffset].owner==country))
				mvaddch(y,2*x,sct[x+xoffset][y+yoffset].designation);
				else mvaddch(y,2*x,ntn[sct[x+xoffset][y+yoffset].owner].mark);
				break;
			case DI_CONT: /*contour*/
				mvaddch(y,2*x,sct[x+xoffset][y+yoffset].altitude);
				break;
			case DI_NATI: /*ownership*/
				if(sct[x+xoffset][y+yoffset].owner==0)
					mvaddch(y,2*x,sct[x+xoffset][y+yoffset].altitude);
				else mvaddch(y,2*x,ntn[sct[x+xoffset][y+yoffset].owner].mark);
				break;
			case DI_RACE: /*race*/
				if(sct[x+xoffset][y+yoffset].owner==0)
					mvaddch(y,2*x,sct[x+xoffset][y+yoffset].altitude);
				else mvaddch(y,2*x,ntn[sct[x+xoffset][y+yoffset].owner].race);
				break;
			case DI_MOVE:	/*movement cost map*/
				if(movecost[x+xoffset][y+yoffset]>=0) mvprintw(y,2*x,"%d",movecost[x+xoffset][y+yoffset]);
				else if(sct[x+xoffset][y+yoffset].altitude==WATER)
					mvaddch(y,2*x,WATER);
				else
					mvaddch(y,2*x,'X');
				break;
			case DI_DEFE:   /*Defence*/
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
					else if(sct[x+xoffset][y+yoffset].vegetation==FOREST)
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
			case DI_PEOP:   /*People*/
				if (sct[x+xoffset][y+yoffset].altitude==WATER)
					mvaddch(y,2*x,WATER);
				else if (sct[x+xoffset][y+yoffset].people>=4950)
					mvaddch(y,2*x,'+');
				else if (sct[x+xoffset][y+yoffset].people>=950)
					mvaddch(y,2*x,'>');
				else
				mvprintw(y,2*x,"%d",(50+sct[x+xoffset][y+yoffset].people)/100);
				break;
			case DI_GOLD:  /*Gold*/
				if (sct[x+xoffset][y+yoffset].altitude==WATER)
					mvaddch(y,2*x,WATER);
				else if(tofood(sct[x+xoffset][y+yoffset].vegetation,country)==0)
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
			case DI_IRON:  /*Iron*/
				if (sct[x+xoffset][y+yoffset].altitude==WATER)
					mvaddch(y,2*x,WATER);
				else if(tofood(sct[x+xoffset][y+yoffset].vegetation,country)==0)
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
void
highlight(x,y)
{
	if((x<0)||(y<0)||(x>COLS-21)||(y>=LINES-4)) return;
	if(((y+yoffset)<MAPY)&&((x+xoffset)<MAPX)) {
		switch(hilmode){
		case HI_OWN: /*ownership*/
			if(country==0) {
				if(sct[x+xoffset][y+yoffset].owner>0)
					standout();
			}
			else if(sct[x+xoffset][y+yoffset].owner==country)
				standout();
			break;
		case HI_ARMY: /*army map*/
			if(occ[x+xoffset][y+yoffset]!=0) standout();
			break;
		default:
			break;
		}
	}
}

/* check if cursor is out of bounds*/
void
coffmap()
{
	if((xcurs<0)||(ycurs<0)||(xcurs>=SCREEN_X_SIZE)
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
