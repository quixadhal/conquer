/* Conquer: Copyright (c) 1988 by Edward M Barlow
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


/*	screen subroutines	*/

/*include files*/
#include <ctype.h>
#include "header.h"
#include "data.h"

extern FILE *fexe;
extern short country;
extern long startgold;
extern short xcurs,ycurs;
extern short xoffset,yoffset;

/*report on armies and allow changes*/
void
armyrpt(repnum)
	int repnum;
	/* 0 is for full report 'a' and 1 is for group report 'g' */
{
	int i,j;
	short army;
	int men;
	short oldarmy;
	int done=FALSE;
	int position;
	int isgod=FALSE;
	int count;    /*number of armies on current screen */
	short armynum=0;    /*current nation id */
	if(country==0) {
		standout();
		isgod=TRUE;
		clear();
		mvaddstr(0,0,"SUPER USER; FOR WHAT NATION NUMBER:");
		clrtoeol();
		standend();
		refresh();
		country = get_number();
		if(country<0||country>NTOTAL) { country=0; return; }
	}
	armynum=0;
	/*new army screen*/
	while(done==FALSE) {
		clear();
		/*Operate on any armies that you wish*/
		standout();
		mvprintw(0,(COLS/2)-20,"ARMY STATS SUMMARY FOR %s",ntn[country].name);
		standend();
		/* give a army report */

		mvaddstr(3,0,"soldiers  :");
		mvaddstr(4,0,"movement  :");
		mvaddstr(5,0,"x location:");
		mvaddstr(6,0,"y location:");
		mvaddstr(7,0,"status    :");
		mvaddstr(8,0,"unit type :");
		mvaddstr(9,0,"cost/turn :");
		mvaddstr(10,0,"enlist    :");

		position=5;
		count=0;
		while((armynum<MAXARM)&&(count!=6)){
			if(ASOLD>0 && (!repnum || (AXLOC==XREAL && AYLOC==YREAL))) {
				count++;
				position+=10;
				standout();
				mvprintw(2,position,"%d:",armynum);
				standend();
				mvprintw(3,position,"%d",ASOLD);
				mvprintw(4,position,"%d",AMOVE);
				mvprintw(5,position,"%d",AXLOC);
				mvprintw(6,position,"%d",AYLOC);
				mvprintw(7,position,"%s",*(soldname+ASTAT));
				mvprintw(8,position,"%s",*(unittype+(ATYPE%100)));
				mvprintw(9,position,"$%d",ASOLD * *(unitmaint+(ATYPE%100)));
				mvprintw(10,position,"$%d",ASOLD * *(u_encost+(ATYPE%100)));
			}
			armynum++;
		}
		if(armynum>=MAXARM) done=TRUE;

		standout();
		mvaddstr(12,(COLS/2)-10,"HIT SPACE KEY IF DONE");
		mvaddstr(13,(COLS/2)-14,"HIT RETURN TO CHANGE AN ARMY");
		mvaddstr(14,(COLS/2)-14,"HIT ANY OTHER KEY TO CONTINUE");
		standend();
		refresh();
		if ((army=getch())==' ') done=TRUE;
		if (army=='\n'){
			done=0;
			mvaddstr(16,0,"WHAT ARMY DO YOU WANT TO CHANGE:");
			clrtoeol();
			refresh();
			armynum = get_number();
			if((armynum<0)||(armynum>MAXARM)) {
				if (isgod==TRUE) country=0;
				return;
			}
#ifdef TRADE
			if(ASTAT==TRADED) {
				errormsg("May not change traded army");
				if (isgod==TRUE) country=0;
				return;
#endif TRADe
			}
			if(ATYPE<99)
			mvaddstr(18,0,"1) CHANGE STATUS, 2) TRANSFER / MERGE, 3) SPLIT ARMY, 4) DISBAND ARMY");
			else mvaddstr(18,0,"1) CHANGE STATUS, 4) DISBAND ARMY:");
			clrtoeol();
#ifdef OGOD
			if(isgod==TRUE) mvaddstr(20,0,"5) LOCATION, 6) SOLDIERS:");
			refresh();
			switch(getch()){
			case '1':
				adjarm(armynum);
				break;
			case '2':
				clear_bottom(0);
				if(ATYPE>=MINMONSTER){
					errormsg("SORRY -- army is monster");
					break;
				}
				oldarmy=armynum;
				mvaddstr(LINES-4,0,"TO WHAT ARMY: ");
				refresh();
				armynum = get_number();

				combinearmies(armynum,oldarmy);
				break;
			case '3':
				splitarmy(armynum);
				break;
			case '4':
				clear_bottom(0);
				if(sct[AXLOC][AYLOC].owner!=country){
					errormsg("YOU DONT OWN SECTOR");
					break;
				}
				else if(ATYPE==A_MERCENARY){
					/*mercs must be payed off*/
					/*mercs do not add to local populace*/
					mvprintw(LINES-4,0,"Your mercenaries demand %ld gold to disband",
					*(u_encost+(ATYPE%100)) * ASOLD);
					mvprintw(LINES-3,0,"continue? (y or n)");
					refresh();
					if(getch()=='y'){
					ntn[country].tgold -= *(u_encost+(ATYPE%100)) * ASOLD;
					ASOLD=0;
					AADJMEN;
					}
				} else if(magic(country,VAMPIRE)==1){
					errormsg("VAMPIRES CANT DISBAND");
					break;
				} else {
					sct[AXLOC][AYLOC].people+=ASOLD;
					ASOLD=0;
					AADJMEN;
					i=AXLOC;
					j=AYLOC;
					SADJCIV2;
				}
				break;
			case '5':
				if(isgod==TRUE){
					/*X LOCATION*/
					mvaddstr(21,0,"WHAT IS THE NEW X LOC: ");
					refresh();
					men = get_number();
					AXLOC=men;
					/*Y LOCATION*/
					mvaddstr(21,0,"WHAT IS THE NEW Y LOC: ");
					refresh();
					men = get_number();
					AYLOC=men;
					AADJLOC;
				}
				break;
			case '6':
				if(isgod==TRUE){
					/*SOLDIERS*/
					mvaddstr(21,0,"WHAT IS THE NEW TOTAL SOLDIERS: ");
					refresh();
					men = get_number();
					ASOLD=men;
					AADJMEN;
				}
				break;
			default:
				mvaddstr(21,0,"ERROR : HIT ANY CHAR TO CONTINUE");
				clrtoeol();
				refresh();
				getch();
			}
			noecho();
			armynum=0;
		}
	}
	if(isgod==TRUE) country=0;
}

void
budget()
{
	short armynum,nvynum;
	long expship=0L,expsold=0L,nosold=0L;	/*nosold = # men,exp expense */
	int isgod=FALSE;

	if(country==0) {
		isgod=TRUE;
		clear();
		mvaddstr(0,0,"SUPER USER; FOR WHAT NATION NUMBER:");
		clrtoeol();
		refresh();
		country = get_number();
	}

	clear();
	standout();
	mvaddstr(0,(COLS/2)-10,"NEXT YEARS BUDGET ESTIMATES");
	standend();

	spreadsheet(country);

	for(armynum=0;armynum<MAXARM;armynum++) if(ASOLD>0) {
		nosold+= ASOLD;
		expsold += ASOLD * *(unitmaint+(ATYPE%100));
	}
	for(nvynum=0;nvynum<MAXNAVY;nvynum++)
		if(NWAR+NMER>0) expship+=(NWAR+NMER);

	standout();
	mvprintw(5,0,  "nation name is.....%s   ",ntn[country].name);
	mvprintw(6,0,  "gold in treasury..$%8ld",ntn[country].tgold);
	mvprintw(7,0,  "number of sectors..%d",spread.sectors);
	standend();
	if(ntn[country].tfood<ntn[country].tciv) standout();
	mvprintw(9,0,  "granary holds ....%8ld",ntn[country].tfood);
	standend();
	mvprintw(10,0, "jewels owned is...%8ld",ntn[country].jewels);
	mvprintw(11,0, "iron ore owned is.%8ld",ntn[country].tiron);
	mvprintw(7,30, "%8d people in gold mines:%8ld",spread.ingold,spread.revjewels);
	mvprintw(8,30, "%8d people in iron mines:%8ld",spread.iniron,spread.reviron);
	mvprintw(9,30, "%8d people in farms:     %8ld",spread.infarm,spread.revfood);
	mvprintw(10,30,"%8d people in capitol:   %8ld",spread.incap,spread.revcap);
	mvprintw(11,30,"%8d people in towns:     %8ld",spread.incity,spread.revcity);
	standout();
	mvprintw(12,30,"%8ld people TOTAL INCOME: %8ld",spread.civilians,spread.gold - ntn[country].tgold);
	standend();

	if(magic(country,VAMPIRE)==1)  expsold/=4;
	mvprintw(14,30,"%8d troops:              %8ld",nosold,expsold);

	mvprintw(15,30,"%8d ships at %5d each: %8ld",expship,SHIPMAINT,expship*SHIPMAINT);
	mvprintw(16,30,"other expenses this turn:     %8ld",startgold-ntn[country].tgold);
	standout();
	mvprintw(17,30,"TOTAL EXPENSES:               %8ld",expsold+(expship*SHIPMAINT)+startgold-ntn[country].tgold);
	mvprintw(18,30,"NET INCOME:                   %8ld",spread.gold-expsold-(expship*SHIPMAINT)-startgold);

	mvaddstr(20,(COLS/2)-13,"HIT 'P' TO SEE PRODUCTION STATS");
	mvaddstr(21,(COLS/2)-12,"HIT ANY OTHER KEY TO CONTINUE");
	standend();
	refresh();
	if(getch()=='P'){
		produce();
	}
	if(isgod==TRUE) country=0;
}

void
produce()
{
	short armynum;
	int military=0;
	int isgod=FALSE;

	if(country==0) {
		isgod=TRUE;
		clear();
		mvaddstr(0,0,"SUPER USER; FOR WHAT NATION NUMBER:");
		clrtoeol();
		refresh();
		country = get_number();
	}
	clear();
	standout();
	mvaddstr(0,(COLS/2)-12,"NEXT YEARS PRODUCTION ESTIMATES");
	standend();

	spreadsheet(country);
	for(armynum=0;armynum<MAXARM;armynum++) if(ASOLD>0) military+=ASOLD;

	standout();
	mvprintw(5,0,  "nation name is ...%9s",ntn[country].name);
	mvprintw(6,0,  "gold in treasury..$%8ld",ntn[country].tgold);
	standend();
	mvaddstr(8,0,  "FOOD PRODUCTION");
	mvprintw(9,0,  "granary now holds.........%8ld tons",ntn[country].tfood);
	mvprintw(10,0, "%8d people in farms..%8ld tons",spread.infarm,spread.food-ntn[country].tfood);
	mvprintw(12,0, "%8d civilians eat....%8d tons",spread.civilians,spread.civilians);
	mvprintw(13,0, "%8d soldiers  eat....%8d tons",military,military*2);
	mvprintw(15,0, "TOTAL NET FOOD............%8d tons",spread.food-ntn[country].tfood-spread.civilians-military*2);
	if(spread.food-spread.civilians-military*2<FOODTHRESH*(100+ntn[country].repro)*spread.civilians/100){
	mvprintw(16,0, "TOTAL NEXT YEARS FOOD.....%8ld tons",spread.food-spread.civilians-military*2);
	}
	else{
	mvprintw(16,0, "TOTAL NEXT YEARS FOOD.....%8ld tons",FOODTHRESH*(100+ntn[country].repro)*spread.civilians/100);
	mvprintw(17,0, "REVENUE FROM EXCESS.......%8ld gold",spread.food-spread.civilians-military*2-FOODTHRESH*(100+ntn[country].repro)*spread.civilians/100);
	}

	mvaddstr(8,41,  "OTHER PRODUCTION");
	mvprintw(10,41, "jewels owned is...............%8ld",ntn[country].jewels);
	mvprintw(11,41, "%8d people in goldmines :%8ld",spread.ingold,spread.jewels-ntn[country].jewels);
	mvprintw(12,41, "ESTIMATE NEXT YEARS JEWELS    %8ld",spread.jewels);
	mvprintw(14,41, "iron ore owned is.............%8ld",ntn[country].tiron);
	mvprintw(15,41, "%8d people in iron mines:%8ld",spread.iniron,spread.iron-ntn[country].tiron);
	mvprintw(16,41, "ESTIMATE NEXT YEARS IRON      %8ld",spread.iron);

	standout();
	mvaddstr(20,(COLS/2)-12,"HIT 'B' TO SEE BUDGET SCREEN");
	mvaddstr(21,(COLS/2)-12,"HIT ANY OTHER KEY TO CONTINUE");
	standend();
	refresh();
	if(getch()=='B'){
		budget();
	}

	if(isgod==TRUE) country=0;
}

/*report on ships and allow changes */
void
fleetrpt()
{
	short navy;
	short oldx,oldy,oldnavy;
	short done=FALSE;
	int position;
	int count;       /*screen number */
	short nvynum=0;    /*current ship id */
	short wships,mships;
	int isgod=FALSE;
	if(country==0) {
		standout();
		isgod=TRUE;
		clear();
		mvaddstr(0,0,"SUPER USER; FOR WHAT NATION NUMBER:");
		clrtoeol();
		standend();
		refresh();
		country = get_number();
		if(country<0||country>NTOTAL) return;
	}

	count=0;
	for(nvynum=0;nvynum<MAXNAVY;nvynum++) count+=NWAR+NMER;
	if(count==0){
		clear();
		standout();
		mvaddstr(5,(COLS/2)-6, "NO NAVIES ");
		mvaddstr(15,(COLS/2)-14,"HIT ANY KEY TO CONTINUE");
		standend();
		done=TRUE;
		refresh();
 		getch();
		if(isgod==TRUE) country=0;
		return;
	}
	nvynum=0;
	while(done==FALSE) {
		clear();
		/*Operate on any navies that you wish*/
		standout();
		mvprintw(0,(COLS/2)-20,"NAVY STATS SUMMARY FOR %s",ntn[country].name);
		standend();
		/* give a navy report */

		mvaddstr(3,0, "warships  :");
		mvaddstr(4,0, "merchant :");
		mvaddstr(5,0, "x location:");
		mvaddstr(6,0, "y location:");
 		mvaddstr(7,0, "crew:");
		mvaddstr(8,0,"move left :");

		position=5;
		count=0;
		while((nvynum<MAXNAVY)&&(count!=6)){
			if((NWAR+NMER)>0) {
				count++;
				position+=10;
				standout();
				mvprintw(2,position,"%d:",nvynum);
				standend();
				mvprintw(3,position,"%d",NWAR);
				mvprintw(4,position,"%d",NMER);
				mvprintw(5,position,"%d",NXLOC);
				mvprintw(6,position,"%d",NYLOC);
				mvprintw(7,position,"%d",NCREW/(NWAR+NMER));
				mvprintw(8,position,"%d",NMOVE);
			}
			nvynum++;
		}
		if(nvynum>=MAXNAVY) done=TRUE;

		standout();
		mvaddstr(12,(COLS/2)-10,"HIT SPACE KEY IF DONE");
		mvaddstr(13,(COLS/2)-14,"HIT RETURN TO CHANGE A NAVY");
		mvaddstr(14,(COLS/2)-14,"HIT ANY OTHER KEY TO CONTINUE");
		standend();
		refresh();
		if ((navy=getch())==' ') done=TRUE;
		if (navy=='\n'){
			mvaddstr(16,0,"WHAT NAVY DO YOU WANT TO CHANGE:");
			clrtoeol();
#endif OGOD
			refresh();
			nvynum = get_number();
#ifdef TRADE
			if (ntn[country].nvy[nvynum].armynum==TRADED) {
				mvaddstr(23,0,"SORRY - THAT NAVY IS UP FOR TRADE");
				refresh();
				getch();
				if (isgod==TRUE) country=0;
				return;
			}
#endif TRADE
			if((nvynum<0)||(nvynum>MAXNAVY)) return;
			mvaddstr(18,0,"1) TRANSFER / MERGE, 2) SPLIT NAVY, 3) DISBAND NAVY:");
  			clrtoeol();
#ifdef OGOD
			if(isgod==TRUE) mvaddstr(19,0,"4) ADJUST SHIPS, 5) LOCATION, 6) CREW");
			clrtoeol();
#endif OGOD
			refresh();
			switch(getch()){
			case '1':
				oldx=NXLOC;
				oldy=NYLOC;
				oldnavy=nvynum;
				mvaddstr(22,0,"TO WHAT NAVY: ");
				clrtoeol();
				refresh();
				nvynum = get_number();
#ifdef TRADE
				if (ntn[country].nvy[nvynum].armynum==TRADED) {
					mvaddstr(23,0,"SORRY - THAT NAVY IS UP FOR TRADE");
					refresh();
					getch();
				} else
#endif TRADE
				if(nvynum==oldnavy) {
					mvprintw(23,0,"SORRY -- SAME NAVY (%d,%d)",nvynum,oldnavy);
					refresh();
					getch();
				}
				else if((nvynum<0)||(nvynum>MAXNAVY)){
					mvprintw(23,0,"SORRY -- INVALID NAVY %d",nvynum);
					refresh();
					getch();
				}
				else if((oldx==NXLOC)&&(oldy==NYLOC)) {
					NWAR+=ntn[country].nvy[oldnavy].warships;
					NMER+=ntn[country].nvy[oldnavy].merchant;
					NADJSHP;
					if(NMOVE>ntn[country].nvy[oldnavy].smove)
						NMOVE=ntn[country].nvy[oldnavy].smove;
					NADJMOV;
					nvynum=oldnavy;
					NWAR=0;
					NMER=0;
					NADJSHP;
				}
				else {
					mvaddstr(23,0,"Navies not together (hit any key) ");
					refresh();
					getch();
				}
				break;
			case '2':
				mvaddstr(21,0,"HOW MANY WARSHIPS TO SPLIT: ");
				clrtoeol();
				refresh();
				wships = get_number();
				mvaddstr(21,0,"HOW MANY MERCHANTS TO SPLIT: ");
				clrtoeol();
				refresh();
				mships = get_number();
				if(wships<0) wships=0;
				if(mships<0) mships=0;
				if((wships<=NWAR)&&(mships<=NMER)){
					NWAR-=wships;
					NMER-=mships;
					NADJSHP;
					oldnavy=nvynum;
					oldx=NXLOC;
					oldy=NYLOC;
					navy=(-1);
					for(nvynum=0;nvynum<MAXNAVY;nvynum++)
						if((NWAR+NMER==0)&&(navy==(-1)))
							navy=nvynum;
					nvynum=navy;
					/*overflow*/
					if(nvynum==(-1)) {
						mvaddstr(23,0,"TOO MANY NAVIES: ");
						nvynum=oldnavy;
						NWAR+=wships;
						NMER+=mships;
						NADJSHP;
					}
					else {
						NMOVE=ntn[country].nvy[oldnavy].smove;
						NXLOC=oldx;
						NYLOC=oldy;
						NWAR=wships;
						NMER=mships;
						NADJSHP;
						NADJLOC;
						NADJMOV;
					}
				}
				else {
					mvaddstr(23,0,"TOO MANY SHIPS: ");
					refresh();
					getch();
				}
				break;
			case '3':
				NWAR=0;
				NMER=0;
				NADJSHP;
				break;
			case '4':
				if (isgod==TRUE) {
					/* ADJUST SHIPS */
					mvaddstr(21,0,"HOW MANY WAR SHIPS: ");
					refresh();
					wships = get_number();
					NWAR = wships;
					mvaddstr(22,0,"HOW MANY MERCHANT SHIPS: ");
					refresh();
					mships = get_number();
					NMER = mships;
					NADJSHP;
				}
				break;
#ifdef OGOD
			case '5':
				if (isgod==TRUE) {
					/*X LOCATION*/
					mvaddstr(21,0,"WHAT IS THE NEW X LOC: ");
					refresh();
					wships = get_number();
					if (wships>=0 && wships<MAPX)
						NXLOC=wships;
					/*Y LOCATION*/
					mvaddstr(22,0,"WHAT IS THE NEW Y LOC: ");
					refresh();
					wships = get_number();
					if (wships>=0 && wships<MAPY)
						NYLOC=wships;
					NADJLOC;
				}
				break;
			case '6':
				if (isgod==TRUE) {
					/* ADJUST CREWSIZE */
					mvaddstr(21,0,"WHAT VALUE FOR CREW PER SHIP: ");
					refresh();
					wships = get_number();
					if (wships>=0 && wships<=SHIPCREW)
						NCREW = wships*(NMER+NWAR);
					NADJCRW;
				}
				break;
#endif OGOD
			default:
				mvaddstr(21,0,"ERROR : HIT ANY CHAR TO CONTINUE");
				clrtoeol();
				refresh();
				getch();
			}
			noecho();
			nvynum=0;
		}
	}
	if(isgod==TRUE) country=0;
}
