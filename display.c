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
short movmode;
/* nation id of owner*/
extern short country;

static char *hasseen;

/* allocate space for the hasseen array based on the actual screen size */
void
init_hasseen()
{
	hasseen = (char *)malloc(((COLS-10)/2) * (LINES-5));
#ifdef BSD
	bzero(hasseen,((COLS-10)/2) * (LINES-5));
#else
	memset( hasseen, 0, ((COLS-10)/2) * (LINES-5));
#endif
	if (hasseen == (char *)NULL) {
		errormsg("Cannot allocate memory.");
		bye(FALSE);
	}
}

/*make a map*/
void
makemap()
{
	register int x,y;

	for(x=0;x<SCREEN_X_SIZE;x++) for(y=0;y<SCREEN_Y_SIZE;y++)
	if( HAS_SEEN(x,y) ) {
		highlight(x,y);
		see(x,y);
	}
	move(ycurs,2*xcurs);
}

void
newdisplay()
{
	mvaddstr(LINES-4,0,"viewing options:  (d)esignation, (r)ace, (M)ove cost, (p)eople, (D)efense");
	clrtoeol();
	mvaddstr(LINES-3,0,"   (f)ood, (c)ontour, (v)egetation, (m)etal, (n)ation mark, (j)ewels, (i)tems");
	clrtoeol();
	mvaddstr(LINES-2,0,"highlight option: (o)wners, (a)rmy, (y)our Army, move (l)eft, (s)pecial,(x)=none");
	clrtoeol();
	mvaddstr(LINES-1,0,"toggle move mode: hit '/'");
	clrtoeol();
	standout();
	mvaddstr(LINES-1,COLS-25,"what display?:");
	standend();
	refresh();
	redraw=TRUE;
	switch(getch()) {
	case '/':
		if( movmode==TRUE ) movmode=FALSE;
		else movmode=TRUE;
		break;
	case 'f':
		dismode=DI_FOOD;
		break;
	case 'v':	/* vegetation map*/
		dismode=DI_VEGE;
		break;
	case 'd':	/* designations map*/
		movmode=FALSE;
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
	case 'M': /* move cost map */
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
	case 'm':
		dismode=DI_METAL;
		break;
	case 'i':
		dismode=DI_ITEMS;
		break;
	case 'a':	/* armies hilighted map*/
		prep(country,FALSE);
		hilmode=HI_ARMY;
		break;
	case 'o':	/* owners hilighted map*/
		hilmode=HI_OWN;
		break;
	case 's':	/* hilight tradegoods */
		hilmode=HI_GOOD;
		break;
	case 'x':	/*no highlighting*/
		hilmode=HI_NONE;
		break;
	case 'y':	/* your armies hilighted map*/
		prep(country,FALSE);
		hilmode=HI_YARM;
		break;
	case 'l':	/* your armies with moves left hilighted map*/
		prep(country,FALSE);
		hilmode=HI_MOVE;
		break;
	default:
		beep();
		redraw=FALSE;
	}
	makebottom();
}

/*see what is in xy as per display mode*/
/* also add move cost next to sector */
void
see(x,y)
{
	int armbonus;
	if((x<0)||(y<0)||(x>=SCREEN_X_SIZE)||(y>=SCREEN_Y_SIZE)
	||((y+yoffset)>=MAPY)||((x+xoffset)>=MAPX)) return;
	if(((y+yoffset)<MAPY)&&((x+xoffset)<MAPX)) {

		if((magic(sct[x+xoffset][y+yoffset].owner,THE_VOID)==TRUE)
		&&((dismode==DI_DEFE)||(dismode==DI_GOLD)||(dismode==DI_METAL)
			||(dismode==DI_PEOP)||(dismode==DI_FOOD)||(dismode==DI_ITEMS))
		&&(country!=sct[x+xoffset][y+yoffset].owner)
		&&(magic(country,NINJA)!=TRUE)
		&&(country!=0)) {
			mvaddch(y,2*x,'?');
		} else {
			switch(dismode){
			case DI_FOOD:	/*food */
				if(tofood( &sct[x+xoffset][y+yoffset],country)==0)
				mvaddch(y,2*x,sct[x+xoffset][y+yoffset].vegetation);
				else if (tofood( &sct[x+xoffset][y+yoffset],country)<10)
				mvprintw(y,2*x,"%d",tofood( &sct[x+xoffset][y+yoffset],country));
				else mvaddch(y,2*x,'+');
				break;
			case DI_VEGE: /*vegetation*/
				mvaddch(y,2*x,sct[x+xoffset][y+yoffset].vegetation);
				break;
			case DI_DESI: /*designation*/
				if(sct[x+xoffset][y+yoffset].owner==0){
					if(tofood( &sct[x+xoffset][y+yoffset],sct[x+xoffset][y+yoffset].owner)!=0) mvaddch(y,2*x,sct[x+xoffset][y+yoffset].altitude);
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
				if(movecost[x+xoffset][y+yoffset]>=0) {
					if(movecost[x+xoffset][y+yoffset]>=10)
						mvaddch(y,2*x,'+');
					else mvprintw(y,2*x,"%d",movecost[x+xoffset][y+yoffset]);
				} else if(sct[x+xoffset][y+yoffset].altitude==WATER)
					mvaddch(y,2*x,WATER);
				else
					mvaddch(y,2*x,'X');
				break;
			case DI_DEFE:   /*Defence*/
				if (sct[x+xoffset][y+yoffset].altitude==WATER)
					mvaddch(y,2*x,WATER);
				else if(movecost[x+xoffset][y+yoffset]<0)
					mvaddch(y,2*x,'X');
				else {

					/*Racial combat bonus due to terrain (the faster you move the better)*/
					armbonus=0;
					armbonus+=5*(9-movecost[x+xoffset][y+yoffset]);

					if(sct[x+xoffset][y+yoffset].altitude==MOUNTAIN)
						armbonus+=40;
					else if(sct[x+xoffset][y+yoffset].altitude==HILL)
						armbonus+=20;

					if(sct[x+xoffset][y+yoffset].vegetation==JUNGLE)
						armbonus+=30;
					else if(sct[x+xoffset][y+yoffset].vegetation==FOREST)
						armbonus+=20;
					else if(sct[x+xoffset][y+yoffset].vegetation==WOOD)
						armbonus+=10;

					armbonus+=fort_val(&sct[x+xoffset][y+yoffset]);

					if(armbonus<200) mvprintw(y,2*x,"%d",armbonus/20);
					else mvaddch(y,2*x,'+');
				}
				break;
			case DI_PEOP:   /*People*/
				if (sct[x+xoffset][y+yoffset].altitude==WATER)
					mvaddch(y,2*x,WATER);
				else if (sct[x+xoffset][y+yoffset].people>=9950)
					mvaddch(y,2*x,'X');
				else if (sct[x+xoffset][y+yoffset].people>=4950)
					mvaddch(y,2*x,'V');
				else if (sct[x+xoffset][y+yoffset].people>=950)
					mvaddch(y,2*x,'I');
				else
				mvprintw(y,2*x,"%d",(50+sct[x+xoffset][y+yoffset].people)/100);
				break;
			case DI_GOLD:  /*Gold*/
				if (sct[x+xoffset][y+yoffset].altitude==WATER)
					mvaddch(y,2*x,WATER);
				else if(tofood( &sct[x+xoffset][y+yoffset],country)==0)
					mvaddch(y,2*x,'X');
				else if (tg_ok(country,&sct[x+xoffset][y+yoffset])){
					if (sct[x+xoffset][y+yoffset].jewels>=10)
						mvaddch(y,2*x,'+');
					else
						mvprintw(y,2*x,"%d",sct[x+xoffset][y+yoffset].jewels);
				} else mvprintw(y,2*x,"0");
				break;
			case DI_METAL:  /*Metal*/
				if (sct[x+xoffset][y+yoffset].altitude==WATER)
					mvaddch(y,2*x,WATER);
				else if(tofood( &sct[x+xoffset][y+yoffset],country)==0)
					mvaddch(y,2*x,'X');
				else if (tg_ok(country,&sct[x+xoffset][y+yoffset])){
					if (sct[x+xoffset][y+yoffset].metal>=10)
						mvaddch(y,2*x,'+');
					else
						mvprintw(y,2*x,"%d",sct[x+xoffset][y+yoffset].metal);
				} else mvprintw(y,2*x,"0");
				break;
			case DI_ITEMS:	/* designations needed for tradegoods */
				if (sct[x+xoffset][y+yoffset].altitude==WATER)
					mvaddch(y,2*x,WATER);
				else if(tofood( &sct[x+xoffset][y+yoffset],country)==0)
					mvaddch(y,2*x,'X');
				else if (sct[x+xoffset][y+yoffset].tradegood!=TG_none
				&& (*(tg_stype+sct[x+xoffset][y+yoffset].tradegood)!='x')
				&& tg_ok(country,&sct[x+xoffset][y+yoffset]))
					mvaddch(y,2*x,*(tg_stype+sct[x+xoffset][y+yoffset].tradegood));
				else mvaddch(y,2*x,'-');
				break;
			default:
				break;
			}
		}
	}
	else mvaddch(y,2*x,' ');

	if( movmode==TRUE )
	if(movecost[x+xoffset][y+yoffset]>=10) {
		mvaddch(y,2*x+1,"+");
	} else if(movecost[x+xoffset][y+yoffset]>=0) {
		mvprintw(y,2*x+1,"%d",movecost[x+xoffset][y+yoffset]);
	}
}

/*highlight what is in xy as per highlight mode*/
void
highlight(x,y)
{
	int	armynum;
	if((x<0)||(y<0)||(x>=SCREEN_X_SIZE)||(y>=SCREEN_Y_SIZE)
	||((y+yoffset)>=MAPY)||((x+xoffset)>=MAPX)) return;
	standend();
	switch(hilmode){
	case HI_MOVE:	/* your armies w/ move left */
		for(armynum=0;armynum<MAXARM;armynum++)
			if(( P_ASOLD != 0 )
			&&( P_AMOVE != 0 )
			&&( P_AXLOC==(x+xoffset ))
			&&( P_AYLOC==(y+yoffset ))) break;
		if(armynum<MAXARM) standout();
		break;
	case HI_YARM:	/* your armies */
		for(armynum=0;armynum<MAXARM;armynum++)
			if(( P_ASOLD != 0)
			&&( P_AXLOC==x+xoffset)
			&&( P_AYLOC==y+yoffset)) break;
		if(armynum<MAXARM) standout();
		break;
	case HI_GOOD:	/* trade goods */
		if(tg_ok( country, &sct[x+xoffset][y+yoffset])
		 &&(sct[x+xoffset][y+yoffset].tradegood != TG_none)
		 &&(magic(sct[x+xoffset][y+yoffset].owner,THE_VOID)!=TRUE
			|| country==0
		 	|| country==sct[x+xoffset][y+yoffset].owner
			|| magic(country,NINJA)==TRUE)
		 &&(sct[x+xoffset][y+yoffset].altitude!=WATER)) standout();
		break;
	case HI_OWN: /* ownership */
		if(country==0) {
			if(sct[x+xoffset][y+yoffset].owner!=0)
				standout();
		} else if(sct[x+xoffset][y+yoffset].owner==country)
			standout();
		break;
	case HI_ARMY: /* any armies */
		if(occ[x+xoffset][y+yoffset]!=0) standout();
		break;
	default:
		break;
	}
}

/* check if cursor is out of bounds*/
void
coffmap()
{
	if((xcurs<=0)||(ycurs<=0)||(xcurs>=SCREEN_X_SIZE-1)
	||((ycurs>=SCREEN_Y_SIZE-1))||((XREAL)>=MAPX)
	||((YREAL)>=MAPY)) offmap();

	if( redraw==TRUE) {
		clear();
		makemap();	 /* update map */
		makebottom();
		redraw=FALSE;
	}
	move(ycurs,2*xcurs);
	makeside(FALSE);  /*update side*/
	move(ycurs,2*xcurs);
	refresh();
}

int
canbeseen(x,y)
int	x,y;
{
	if(!ONMAP(x,y)) return(FALSE);
	return( (int) HAS_SEEN(x-xoffset,y-yoffset) );
}

/** CANSEE() fills seen[SCREEN_X_SIZE][SCREEN_Y_SIZE] */
void
whatcansee()
{
	register int x,y;
	int	i,j;
	short	armynum,nvynum;

	if((magic(country,KNOWALL)==1)||(country==0)) {
		for(x=0;x<SCREEN_X_SIZE;x++) for(y=0;y<SCREEN_Y_SIZE;y++)
			HAS_SEEN(x,y)=TRUE;
		return;
	}
	for(x=0;x<SCREEN_X_SIZE;x++) for(y=0;y<SCREEN_Y_SIZE;y++)
		HAS_SEEN(x,y)=FALSE;

	for(x=(-LANDSEE);(x<SCREEN_X_SIZE+LANDSEE);x++)
	for(y=(-LANDSEE);(y<SCREEN_Y_SIZE+LANDSEE);y++)
	if((ONMAP(x+xoffset,y+yoffset))
	&&(sct[x+xoffset][y+yoffset].owner==country)){
		for(i=x-LANDSEE;i<=x+LANDSEE;i++)
		for(j=y-LANDSEE;j<=y+LANDSEE;j++)
		if(i>=0 && j>=0 && i<SCREEN_X_SIZE && j<SCREEN_Y_SIZE)
			HAS_SEEN(i,j)=TRUE;
	}

	for(nvynum=0;nvynum<MAXNAVY;nvynum++)
		if((P_NMSHP!=0)||(P_NWSHP!=0)||(P_NGSHP!=0))
		for(i=(int)P_NXLOC-xoffset-NAVYSEE;i<=(int)P_NXLOC-xoffset+NAVYSEE;i++)
		for(j=(int)P_NYLOC-yoffset-NAVYSEE;j<=(int)P_NYLOC-yoffset+NAVYSEE;j++)
		if(i>=0 && j>=0 && i<SCREEN_X_SIZE && j<SCREEN_Y_SIZE)
			HAS_SEEN(i,j)=TRUE;

	for(armynum=0;armynum<MAXARM;armynum++)
		if(P_ASOLD>0)
		for(i=(int)P_AXLOC-xoffset-ARMYSEE;i<=(int)P_AXLOC-xoffset+ARMYSEE;i++)
		for(j=(int)P_AYLOC-yoffset-ARMYSEE;j<=(int)P_AYLOC-yoffset+ARMYSEE;j++)
		if(i>=0 && j>=0 && i<SCREEN_X_SIZE && j<SCREEN_Y_SIZE)
			HAS_SEEN(i,j)=TRUE;

	return;
}
