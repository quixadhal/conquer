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
#include "header.h"
#include "data.h"
#include <ctype.h>

extern FILE *fexe;
/*redraw map in this turn if redraw is a 1*/
extern short redraw;
extern short selector;
extern short pager;
extern short xcurs,ycurs,xoffset,yoffset;
extern short hilmode;   /*highlight modes: 0=owned sectors, 1= armies, 2=none*/

/* nation id of owner*/
extern short country;
int armornvy=AORN;	/*is one if navy*/

void
mymove()
{
	int mveleft;
	int i,j,x,y;
	char carried;
	int total,Tarmynum,Tnation;
	int startx=0,starty=0;
	int ctransport=0;	 /*number of civilians to transport*/
	int valid=FALSE;     /*1 if move was a valid move (you must pay cost)*/
	short armynum;
	int oldxcurs,oldycurs;
	short nvynum;
	int done=FALSE;	/*done is TRUE if done with this move*/
	int capacity;

	armornvy=AORN;
	clear_bottom(0);
	/*get selected army or navy*/
	if((armynum=getselunit())<0) {
		errormsg("ERROR: FAKE UNIT SELECTED: ");
		armornvy=AORN;
		return;
	}

	/*navy*/
	if((armynum>=MAXARM)&&(armynum<MAXARM+MAXNAVY)) {
		nvynum=armynum-MAXARM;
		armynum=(-1);
		mvprintw(LINES-4,0,"NAVY %d: ",nvynum);
		standend();
		clrtoeol();
		capacity=NMER*SHIPHOLD;
		if((NMOVE<=0)) {
			errormsg("NO MOVE POINTS LEFT");
			armornvy=AORN;
			return;
		}
		if( capacity > 0 ) {
			int	has_army;
			int	has_civilians;
			char	prompt[ 200 ];

			has_army = solds_in_sector( XREAL, YREAL, country ) > 0;
			has_civilians = (sct[XREAL][YREAL].owner == country) &&
				(sct[XREAL][YREAL].people > 0);

			if( !has_civilians && !has_army ) {
				carried = 'n';
			} else {
				strcpy( prompt, "Transport " );
				if( has_army )
					strcat( prompt, "(a)rmy, " );
				if( has_civilians )
					strcat( prompt, "(c)iv, " );
				if( has_army && has_civilians )
					strcat( prompt, "(b)oth, " );
				strcat( prompt, "(n)othing: " );

				mvaddstr( LINES-4, 13, prompt );
				clrtoeol();
				refresh();
				carried = getch();
			}
		}
		else carried='n';

		/*transport army*/
		if(carried=='a'||carried=='b'){
			mvprintw(LINES-3,0,"CAPACITY=%d ;WHAT ARMY TO XPORT?:",capacity);
			clrtoeol();
			refresh();
			armynum = get_number();
			if((armynum<MAXARM)&&(armynum>=0)
			&&(AXLOC==XREAL)&&(AYLOC==YREAL)){
				if(ASOLD>capacity){
					errormsg("ERROR: TOO MANY TROOPS");
					armornvy=AORN;
					return;
				}
				else if((AMOVE==0)&&(ATYPE!=A_MARINES)){
					errormsg("SORRY: ARMY HAS NO MOVEMENT POINTS");
					armornvy=AORN;
					return;
				}
				else capacity-=ASOLD;
			}
			else {
				errormsg("ERROR: INVALID ARMY!");
				armornvy=AORN;
				return;
			}
		}
		/*transport civilians*/
		if(carried=='c'||carried=='b'){
			mvprintw(LINES-2,0,"CAPACITY=%d ;HOW MANY CIVILIANS XPORT?:",capacity);
			clrtoeol();
			refresh();
			ctransport = get_number();
			if(ctransport>0&&ctransport<=sct[XREAL][YREAL].people&&(ctransport<=capacity)){
				startx=XREAL;
				starty=YREAL;
			}
			else {
				errormsg("NUMBER OF MEN ERROR");
				armornvy=AORN;
				return;
			}
		}
	}
	/*army*/
	else {
		mvprintw(LINES-4,0,"ARMY %d: ",armynum);
		clrtoeol();
		if(AMOVE<=0){
			errormsg("NO MOVEMENT POINTS LEFT");
			redraw=FALSE;
			armornvy=AORN;
			return;
		}
	}

	clear_bottom(3);
	standout();
	mvaddstr(LINES-2,0,"MOVEMENT SCREEN - see documentation");
	clrtoeol();
	mvaddstr(LINES-1,0,"HIT SPACE IF DONE         ");
	clrtoeol();
	standend();
	move(ycurs,xcurs*2);
	refresh();

	while(done==FALSE){
		valid=TRUE;
		if(armornvy==NAVY) mveleft=NMOVE;
		else mveleft=AMOVE;
		oldxcurs=xcurs;
		oldycurs=ycurs;
		switch(getch()) {
		case '1':
		case 'b':
			xcurs--;
			ycurs++;
			break;
		case '4':
		case 'h':
			xcurs--;
			break;
		case '2':
		case 'j':		/*move down*/
			ycurs++;
			break;
		case '8':
		case 'k':		/*move up*/
			ycurs--;
			break;
		case '6':
		case 'l':		/*move east*/
			xcurs++;
			break;
		case '3':
		case 'n':		/*move south-east*/
			ycurs++;
			xcurs++;
			break;
		case '9':
		case 'u':		/*move north-east*/
			ycurs--;
			xcurs++;
			break;
		case '7':
		case 'y':		/*move north-west*/
			ycurs--;
			xcurs--;
			break;
		case '':		/* redraw map */
			valid=FALSE;
			redraw=TRUE;
			coffmap();
				/*see within one sector of unit*/
				if(hilmode==3) {
					for(i=XREAL-xoffset-1;i<=XREAL-xoffset+1;i++){
						for(j=YREAL-yoffset-1;j<=YREAL-yoffset+1;j++){
							highlight(i,j);
							see(i,j);
						}
					}
					for(i=0;i<MAXARM;i++) if(ntn[country].arm[i].sold>0){
						standout();
						see(ntn[country].arm[i].xloc-xoffset,ntn[country].arm[i].yloc-yoffset);
					}
					for(i=0;i<MAXNAVY;i++) if(ntn[country].nvy[i].merchant+ntn[country].nvy[i].warships>0){
						standout();
						see(ntn[country].nvy[i].xloc-xoffset,ntn[country].nvy[i].yloc-yoffset);

					}
				}
				else for(i=XREAL-xoffset-1;i<=XREAL-xoffset+1;i++){
					for(j=YREAL-yoffset-1;j<=YREAL-yoffset+1;j++){
						highlight(i,j);
						see(i,j);
					}
				}
			move(ycurs,xcurs*2);
			refresh();
			break;
		case ' ':
			valid=FALSE;
			done=TRUE;
			break;
		default:
			beep();
			valid=FALSE;
		}

		/*if valid move check if have enough movement points*/
		if(valid==TRUE)
		if(armornvy==ARMY) {
			if((movecost[XREAL][YREAL]<0)
			||(movecost[XREAL][YREAL]>mveleft)) {
				beep();
				valid=FALSE;
				xcurs=oldxcurs;
				ycurs=oldycurs;
			} else {
				AMOVE-=movecost[XREAL][YREAL];
				if(AMOVE<=0) done=TRUE;
			}

		} else if(armornvy==NAVY) {
			if(movecost[XREAL][YREAL]>mveleft){
				beep();
				valid=FALSE;
				xcurs=oldxcurs;
				ycurs=oldycurs;
			} else if(movecost[XREAL][YREAL] > 0){
				/* LAND OF SOME TYPE */
				if(((sct[XREAL][YREAL].designation==DCITY)
				||(sct[XREAL][YREAL].designation==DCAPITOL))){
					/* harbor */
					if(NMOVE>=4) NMOVE-=4;
					else {
						beep();
						valid=FALSE;
						xcurs=oldxcurs;
						ycurs=oldycurs;
					}
				} else {	/* coastland */
				standout();
				mvprintw(LINES-3,0,"DO YOU WISH TO LAND SHIPS?");
				standend();
				clrtoeol();
				refresh();
				if(getch()=='y') {
					move(3,0);
					clrtoeol();
					if(NMOVE>=10) NMOVE=0;
					else {
						beep();
						valid=FALSE;
						xcurs=oldxcurs;
						ycurs=oldycurs;
						move(ycurs,xcurs*2);
						refresh();
					}
				} else {
					move(3,0);
					clrtoeol();
					valid=FALSE;
					xcurs=oldxcurs;
					ycurs=oldycurs;
					move(ycurs,xcurs*2);
					refresh();
				}
				}
			} else NMOVE+=movecost[XREAL][YREAL];

			if(NMOVE<=0) done=TRUE;

		} else if(armornvy==AORN){
			errormsg("ERROR - NOT ARMY OR NAVY");
			return;
		}

		/*calc enemy soldiers */
		total=0;
		if(armornvy==ARMY) {
			for(Tnation=0;Tnation<NTOTAL;Tnation++)
			if(Tnation!=country)
			for(Tarmynum=0;Tarmynum<MAXARM;Tarmynum++)
			if((ntn[Tnation].arm[Tarmynum].sold>0)
			&&(ntn[Tnation].arm[Tarmynum].xloc==XREAL)
			&&(ntn[Tnation].arm[Tarmynum].yloc==YREAL)
			&&((ntn[country].dstatus[Tnation]>=HOSTILE)
			||(ntn[Tnation].dstatus[country]>=HOSTILE))
			&&(ntn[Tnation].arm[Tarmynum].stat!=SCOUT)
			&&(ntn[Tnation].arm[Tarmynum].unittyp!=A_NINJA))
				total+=ntn[Tnation].arm[Tarmynum].sold;
		}
		/*naval total is number of at war WARSHIPS within one sector*/
		else {
			for(Tnation=0;Tnation<NTOTAL;Tnation++)
			if(Tnation!=country)
			for(Tarmynum=0;Tarmynum<MAXNAVY;Tarmynum++)
			if((ntn[Tnation].nvy[Tarmynum].warships>0)
			&&(ntn[Tnation].nvy[Tarmynum].xloc<=XREAL+1)
			&&(ntn[Tnation].nvy[Tarmynum].xloc>=XREAL-1)
			&&(ntn[Tnation].nvy[Tarmynum].yloc<=YREAL+1)
			&&(ntn[Tnation].nvy[Tarmynum].yloc>=YREAL-1)
			&&(ntn[Tnation].dstatus[country]>=HOSTILE))
				total+=ntn[Tnation].nvy[Tarmynum].warships;
		}

		/*if moved and not done*/
		if((valid==TRUE)&&(done==FALSE)){
			/*check if offmap and correct*/
			coffmap();

			mvaddstr(LINES-3,0,"                  ");

			/*scouts/ninja and rocs/griffon ignore zones of
			control*/
			if((armornvy==ARMY)
			&&(ASTAT!=SCOUT)
			&&(ATYPE!=A_NINJA)
			&&(ATYPE!=A_ROC)
			&&(ATYPE!=A_GRIFFON)){
				if((sct[XREAL][YREAL].owner!=0)
				&&(sct[XREAL][YREAL].owner!=country)
				&&(sct[XREAL][YREAL].people>100)
		&&(ntn[sct[XREAL][YREAL].owner].dstatus[country]>FRIENDLY)
		&&(ntn[country].dstatus[sct[XREAL][YREAL].owner]<WAR)){
		/*if other owner (with civilians) and not hostile then stop*/
					beep();
					mvaddstr(LINES-3,0,"HALT! YOU MAY NOT PASS HERE!");
					clrtoeol();
					refresh();
					AMOVE=0;
					AADJMOV;
					done=TRUE;
				}
				else if(total>0){
					/*stop if you have < 2* total*/
					if(ASOLD < 2 * total){
						AMOVE=0;
						AADJMOV;
						mvprintw(LINES-3,0,"Zone Of Control ");
						beep();
						done=TRUE;
					}
					else mvaddstr(LINES-3,0,"SAFE -- hit return");
				}
			}
			else if((armornvy==NAVY)&&(total>0)){
				/*25% stop if they have > total*/
				if((ntn[country].nvy[nvynum].warships < total)
				&&(rand()%4==0)){
					NMOVE=0;
					NADJMOV;
					mvprintw(LINES-3,0,"%d Enemy WarShips Sighted ",total);
					beep();
					done=TRUE;
				}
				else mvaddstr(LINES-3,0,"SAFE -- hit return");
			}

			standout();
			if(armornvy==ARMY){
				mvprintw(LINES-2,0,"MOVESCREEN: move left: %d  ",AMOVE);
			}
			else mvprintw(LINES-2,0,"MOVESCREEN: move left: %d  ",NMOVE);
			clrtoeol();

			mvprintw(LINES-1,20,"move cost is %d  ",abs(movecost[XREAL][YREAL]));
			standend();
			move(ycurs,xcurs*2);

			/*if you just moved off map then see it*/
			if(inch()==' ') {
				if(armornvy==ARMY) {
					AXLOC=XREAL;
					AYLOC=YREAL;
				}
				else if(armornvy==NAVY) {
					NXLOC=XREAL;
					NYLOC=YREAL;
				}
				makemap();
			}
			else {
				makeside();
				/*see within one sector of unit*/
				if(hilmode==3) {
					for(i=XREAL-xoffset-1;i<=XREAL-xoffset+1;i++){
						for(j=YREAL-yoffset-1;j<=YREAL-yoffset+1;j++){
							highlight(i,j);
							see(i,j);
						}
					}
					for(i=0;i<MAXARM;i++) if(ntn[country].arm[i].sold>0){
						standout();
						see(ntn[country].arm[i].xloc-xoffset,ntn[country].arm[i].yloc-yoffset);
					}
					for(i=0;i<MAXNAVY;i++) if(ntn[country].nvy[i].merchant+ntn[country].nvy[i].warships>0){
						standout();
						see(ntn[country].nvy[i].xloc-xoffset,ntn[country].nvy[i].yloc-yoffset);

					}
				}
				else for(i=XREAL-xoffset-1;i<=XREAL-xoffset+1;i++){
					for(j=YREAL-yoffset-1;j<=YREAL-yoffset+1;j++){
						highlight(i,j);
						see(i,j);
					}
				}
			}
			move(ycurs,xcurs*2);
			refresh();
		}
	}

	/*at this point you are done with move*/
	/*move unit now to XREAL,YREAL*/
	selector=0;
	pager=0;
	mvaddstr(LINES-1,0,"DONE MOVEMENT");
	clrtoeol();
	if(armornvy==ARMY){
		AXLOC=XREAL;
		AYLOC=YREAL;
		AADJLOC;
		AADJMOV;
		/*if sector unowned take it*/
		/*first check if occupied*/
		/* if (other owner and unoccupied) or (no owner) you take*/
		if(ASOLD>=TAKESECTOR){
			if(SOWN==0){
				mvprintw(LINES-2,0,"TAKING UNOWNED SECTOR");
				SOWN=country;
				SADJOWN;
				AMOVE=0;
				AADJMOV;
			}
			else if((sct[AXLOC][AYLOC].owner!=country)
				&&((sct[AXLOC][AYLOC].designation==DCITY)
				||(sct[AXLOC][AYLOC].designation==DCAPITOL))){
				mvprintw(LINES-2,0,"ENTERING CITY SECTOR");
			}
			else if((ASOLD > 7*total)&&(total!=0)){
				/*over run if > 7x as many*/
				mvaddstr(LINES-3,0,"OVERRAN HOSTILE ARMY ");
				AMOVE=0;
				AADJMOV;
				x=country;
				y=armynum;
				for(Tnation=0;Tnation<MAXNTN;Tnation++)
				if(Tnation!=x)
				for(Tarmynum=0;Tarmynum<MAXARM;Tarmynum++)
				if((ntn[Tnation].arm[Tarmynum].xloc==XREAL)
				&&(ntn[Tnation].arm[Tarmynum].yloc==YREAL)
				&&((ntn[x].dstatus[Tnation]>=HOSTILE)
				||(ntn[Tnation].dstatus[x]>=HOSTILE))) {
					country=Tnation;
					armynum=Tarmynum;
					ASOLD=0;
					AADJMEN;
				}
				country=x;
				armynum=y;
				if(sct[XREAL][YREAL].owner!=country){
				if((sct[XREAL][YREAL].people>0)
				&&(ntn[sct[XREAL][YREAL].owner].race!=ntn[country].race))
				if(magic(country,SLAVER)==TRUE) {
					flee(XREAL,YREAL,0,TRUE);
				} else{
					flee(XREAL,YREAL,0,FALSE);
				}
				SOWN=country;
				SADJOWN;
				if (is_habitable(AXLOC,AYLOC)) {
					sct[AXLOC][AYLOC].designation=DDEVASTATED;
					SADJDES;
				}
				}
				beep();
			}
			else if((SOWN!=country)
			&&((occ[XREAL][YREAL]==0)
				||(occ[XREAL][YREAL]==country))
			&&(ntn[country].dstatus[SOWN]>HOSTILE)
			&&((ASTAT==ATTACK)||(ASTAT==DEFEND))) {
				/*people flee if not of same race*/
				if((sct[XREAL][YREAL].people>0)
				&&(ntn[sct[XREAL][YREAL].owner].race!=ntn[country].race))
				if(magic(country,SLAVER)==TRUE) {
					flee(XREAL,YREAL,0,TRUE);
				} else{
					flee(XREAL,YREAL,0,FALSE);
				}
				mvprintw(LINES-2,0,"TAKING SECTOR");
				SOWN=country;
				SADJOWN;
				AMOVE=0;
				AADJMOV;
			}
			clrtoeol();
			refresh();
		}
	} else if(armornvy==AORN){
		mvprintw(LINES-3,0,"ERROR");
		return;
	} else {
		/*else navy*/
		mvprintw(LINES-1,0,"NAVY DONE: ");
		clrtoeol();
		NXLOC=XREAL;
		NYLOC=YREAL;
		NADJLOC;
		NADJMOV;
		if((carried=='a'||carried=='b')
		&&(sct[XREAL][YREAL].altitude!=WATER)&&(armynum>=0)
		&&(armynum<MAXARM)) {
			if((SOWN!=0)&&(SOWN!=country)&&(ATYPE!=A_MARINES)){
				mvprintw(LINES-2,0,"SECTOR OWNED - CANT LAND");
				refresh();
				sleep(1);
			} else {
				AXLOC=XREAL;
				AYLOC=YREAL;
				AADJLOC;
				AMOVE=0;
				AADJMOV;
				/*take unowned land*/
				if((SOWN==0)&&(ASOLD>=TAKESECTOR)) {
					SOWN=country;
					SADJOWN;
				}
			}
		}
		/*land civilians*/
		if((carried=='b'||carried=='c')
		&&(sct[XREAL][YREAL].altitude!=WATER)
		&&(ctransport>0)) {
			if(SOWN==country) {
				sct[XREAL][YREAL].people+=ctransport;
				SADJCIV;
				sct[startx][starty].people-=ctransport;
				i=startx;
				j=starty;
				SADJCIV2;
			}
			else mvprintw(LINES-3,0,"cant move people to %d,%d as unowned",XREAL,YREAL);
		}
		if((carried=='a'||carried=='b'||carried=='c')
		&&(sct[XREAL][YREAL].altitude==WATER)) {
			mvaddstr(LINES-3,0,"You stopped in WATER, returning any transported people");
		}
	}
	redraw=FALSE;
	makemap();
	armornvy=AORN;
}

/*get selected army or navy: if navy, number is MAXARM+nvynum*/
int
getselunit()
{
	int number=(-1);
	short armynum=0;
	short nvynum=0;
	int count=0;
	/*current selected unit is selector/2+5*pager*/
	/*count is order of that army in sector*/
	/*armynum is number of that army*/
	for(armynum=0;armynum<MAXARM;armynum++){
		if((ASOLD>0)&&(AXLOC==XREAL)&&(AYLOC==YREAL)) {
			if((5*pager)+(selector/2)==count) number=armynum;
			count++;
		}
	}

	if(number==(-1)){
	for(nvynum=0;nvynum<MAXNAVY;nvynum++)
		if((NMER+NWAR>0)&&(NXLOC==XREAL)&&(NYLOC==YREAL)) {
			if((5*pager)+(selector/2)==count) number=MAXARM+nvynum;
			count++;
		}
	}
	if(number>=0){
		if(number>=MAXARM) armornvy=NAVY;
		else armornvy=ARMY;
	}
	return(number);
}
