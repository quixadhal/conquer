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

mymove()
{
	int mveleft;
	int i,j,x,y;
	char carried;
	int total,Tarmynum,Tnation;
	int startx=0,starty=0;
	int ctransport=0;	 /*number of civilians to transport*/
	int valid=0;     /*1 if move was a valid move (you must pay cost)*/
	short armynum;
	int oldxcurs,oldycurs;
	short nvynum;
	int done=0;	/*done is 1 if done with this move*/
	int capacity;

	armornvy=AORN;
	/*get selected army or navy*/
	if((armynum=getselunit())<0) {
		standout();
		beep();
		mvprintw(LINES-3,0,"ERROR: FAKE UNIT (%d) SELECTED: ",armynum);
		clrtoeol();
		mvaddstr(LINES-1,0,"hit return to continue");
		clrtoeol();
		standend();
		refresh();
		getch();
		return;
	}

	/*navy*/
	if((armynum>=MAXARM)&&(armynum<MAXARM+MAXNAVY)) {
		nvynum=armynum-MAXARM;
		armynum=(-1);
		standout();
		mvprintw(LINES-3,0,"NAVY %d SELECTED: ",nvynum);
		standend();
		clrtoeol();
		capacity=NMER*SHIPHOLD;
		if((NMOVE<=0)) {
			mvprintw(LINES-2,0,"NO MOVE POINTS LEFT on fleet %d",nvynum);
			beep();
			redraw=FALSE;
			clrtoeol();
			mvaddstr(LINES-1,0,"hit return to continue");
			clrtoeol();
			refresh();
			getch();
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

				mvaddstr( LINES-2, 0, prompt );
				clrtoeol();
				refresh();
				carried = getch();
			}
		}
		else carried='n';

		/*transport army*/
		if(carried=='a'||carried=='b'){
			mvprintw(LINES-2,0,"CAPACITY=%d ;WHAT ARMY TO XPORT?:",capacity);
			clrtoeol();
			refresh();
			armynum = get_number();
			if((armynum<MAXARM)&&(armynum>=0)
			&&(AXLOC==XREAL)&&(AYLOC==YREAL)){
				if(ASOLD>capacity){
					mvaddstr(LINES-1,0,"ERROR: TOO MANY TROOPS");
					clrtoeol();
					refresh();
					getch();
					armornvy=AORN;
					return;
				}
				else if(AMOVE==0){
					mvaddstr(LINES-1,0,"SORRY: ARMY HAS NO MOVEMENT POINTS");
					clrtoeol();
					refresh();
					getch();
					armornvy=AORN;
					return;
				}
				else capacity-=ASOLD;
			}
			else {
				mvaddstr(LINES-1,0,"ERROR: INVALID ARMY!");
				refresh();
				getch();
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
				mvaddstr(LINES-2,0,"NUMBER OF MEN ERROR");
				refresh();
				getch();
				armornvy=AORN;
				return;
			}
		}
		refresh();
		standend();
	}
	/*army*/
	else {
		mvprintw(LINES-3,0,"ARMY %d SELECTED: ",armynum);
		clrtoeol();
		if(AMOVE<=0){
			mvaddstr(LINES-2,0,"NO MOVEMENT POINTS LEFT");
			clrtoeol();
			mvaddstr(LINES-1,0,"hit return to continue");
			clrtoeol();
			beep();
			standend();
			refresh();
			redraw=FALSE;
			getch();
			armornvy=AORN;
			return;
		}
	}

	standout();
	mvaddstr(LINES-2,0,"MOVEMENT SCREEN");
	clrtoeol();
	mvaddstr(LINES-1,0,"HIT SPACE IF DONE         ");
	clrtoeol();
	standend();
	move(ycurs,xcurs*2);
	refresh();

	while(done==0){
		valid=1;
		if(armornvy==NAVY) mveleft=NMOVE;
		else mveleft=AMOVE;
		oldxcurs=xcurs;
		oldycurs=ycurs;
		switch(getch()) {
		case 'b':
			xcurs--;
			ycurs++;
			break;
		case 'h':
			xcurs--;
			break;
		case 'j':		/*move down*/
			ycurs++;
			break;
		case 'k':		/*move up*/
			ycurs--;
			break;
		case 'l':		/*move east*/
			xcurs++;
			break;
		case 'n':		/*move south-east*/
			ycurs++;
			xcurs++;
			break;
		case 'u':		/*move north-east*/
			ycurs--;
			xcurs++;
			break;
		case 'y':		/*move north-west*/
			ycurs--;
			xcurs--;
			break;
		case ' ':
			valid=0;
			done=1;
			break;
		default:
			beep();
			valid=0;
		}

		/*if valid move check if have enough movement points*/
		if(valid==1){
			/*if valid move decrement move rate*/
			if(armornvy==ARMY) {
				if((movecost[XREAL][YREAL]<0)
				||(movecost[XREAL][YREAL]>mveleft)) {
					beep();
					valid=0;
					xcurs=oldxcurs;
					ycurs=oldycurs;
				}
				else {
					AMOVE-=movecost[XREAL][YREAL];
					if(AMOVE<=0) done=1;
				}
			}
			/*then navy*/
			else if(armornvy==NAVY) {
				if(movecost[XREAL][YREAL]>mveleft) {
					beep();
					valid=0;
					xcurs=oldxcurs;
					ycurs=oldycurs;
				}
				else if(movecost[XREAL][YREAL]>0){
					standout();
					mvprintw(LINES-3,0,"DO YOU WISH TO LAND SHIPS?");
					standend();
					clrtoeol();
					refresh();
					if(getch()=='y') {
						move(3,0);
						clrtoeol();
						done=1;
			if(((sct[XREAL][YREAL].designation==DCITY)
			||(sct[XREAL][YREAL].designation==DCAPITOL))
			&&(sct[XREAL][YREAL].owner==country)) NMOVE--;
						else NMOVE=0;
					}
					else {
						move(3,0);
						clrtoeol();
						valid=0;
						xcurs=oldxcurs;
						ycurs=oldycurs;
						move(ycurs,xcurs*2);
						refresh();
					}
				}
				else NMOVE+=movecost[XREAL][YREAL];
				if(NMOVE<=0) done=1;
			}
			else if(armornvy==AORN){
				mvprintw(LINES-3,0,"AORN ERROR -- aborting hit return");
				refresh();
				getch();
				return;
			}
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
			&&(ntn[Tnation].arm[Tarmynum].stat!=SCOUT))
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
		if((valid==1)&&(done==0)){
			/*check if offmap and correct*/
			coffmap();

			mvaddstr(LINES-3,0,"                  ");

			if((armornvy==ARMY)&&(ASTAT!=SCOUT)){
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
					done=1;
				}
				else if(total>0){
					/*stop if you have < 2* total*/
					if(ASOLD < 2 * total){
						AMOVE=0;
						AADJMOV;
						mvprintw(LINES-3,0,"Zone Of Control ");
						beep();
						done=1;
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
					done=1;
				}
				else mvaddstr(LINES-3,0,"SAFE -- hit return");
			}

			standout();
			if(armornvy==ARMY){
				mvprintw(LINES-2,0,"MOVESCREEN: move left: %d  ",AMOVE);
			}
			else mvprintw(LINES-2,0,"MOVESCREEN: move left: %d  ",NMOVE);

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
		if(ASOLD>=75){
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
				&&(ntn[sct[XREAL][YREAL].owner].race!=ntn[country].race)) flee(XREAL,YREAL,0);
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
				&&(ntn[sct[XREAL][YREAL].owner].race!=ntn[country].race)) flee(XREAL,YREAL,0);
				mvprintw(LINES-2,0,"TAKING SECTOR");
				SOWN=country;
				SADJOWN;
				AMOVE=0;
				AADJMOV;
			}
			clrtoeol();
			refresh();
		}
	}
	else if(armornvy==AORN){
		mvprintw(LINES-3,0,"ERROR");
		return;
	}
	else {
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
			AXLOC=XREAL;
			AYLOC=YREAL;
			AADJLOC;
			AMOVE=0;
			AADJMOV;
			/*take unowned land*/
			if(SOWN==0) {
				SOWN=country;
				SADJOWN;
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


#define ALPHA_SIZE	128

/*movecost contains movement cost unless water  -1 or unenterable land (-2)*/
/*NEW: if water and not ajacent to land will cost -4*/
updmove(race)
{
	register struct s_sector	*sptr;
	register int	i,j;
	int x,y;
	int onecost;
	int twocost;
	char *HVegcost=  "83210000232E";
	char *OVegcost=  "52100012517E";
	char *EVegcost=  "86221000017E";
	char *DVegcost=  "57100013577E";
	char *XVegcost=  "83210001232E";	/*others*/
	char *HElecost=  "E321E";
	char *OElecost=  "E211E";
	char *EElecost=  "E631E";
	char *DElecost=  "E211E";
	char *XElecost=  "E321E";  		/*others*/

	short	veg_cost[ ALPHA_SIZE ];
	short	ele_cost[ ALPHA_SIZE ];

	for( j = 0; j < ALPHA_SIZE; j++ )
		veg_cost[j] = ele_cost[j] = -1;

	for( j = 0; j <= 10; j++ ) {
		switch( race ) {
		case ELF:
			veg_cost[ veg[j] ] = todigit( EVegcost[j] );
			break;
		case DWARF:
			veg_cost[ veg[j] ] = todigit( DVegcost[j] );
			break;
		case ORC:
			veg_cost[ veg[j] ] = todigit( OVegcost[j] );
			break;
		case HUMAN:
			veg_cost[ veg[j] ] = todigit( HVegcost[j] );
			break;
		default:
			veg_cost[ veg[j] ] = todigit( XVegcost[j] );
			break;
		} /* switch */
	} /* for */

	for( j = 0; j <= 4; j++ ) {
		switch( race ) {
		case ELF:
			ele_cost[ ele[j] ] = todigit( EElecost[j] );
			break;
		case DWARF:
			ele_cost[ ele[j] ] = todigit( DElecost[j] );
			break;
		case ORC:
			ele_cost[ ele[j] ] = todigit( OElecost[j] );
			break;
		case HUMAN:
			ele_cost[ ele[j] ] = todigit( HElecost[j] );
			break;
		default:
			ele_cost[ ele[j] ] = todigit( XElecost[j] );
			break;
		} /* switch */
	} /* for */

	for(x=0;x<MAPX;x++) for(y=0;y<MAPX;y++){
		sptr = &sct[x][y];

		if(race==GOD) movecost[x][y] = 0;
		else if(sptr->altitude==WATER) {
			movecost[x][y] = -4;
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
			if( i>=0 && j>=0 && i<MAPX && j<MAPY)
				if( sct[i][j].altitude != WATER)
					movecost[x][y] = -1;
		}
		else if(((magic(country,DERVISH)==1)
		||(magic(country,DESTROYER)==1))
		&&((sptr->vegetation==ICE)
		||(sptr->vegetation==DESERT))) 
			movecost[x][y] = 1;
		else {
			onecost = veg_cost[ sptr->vegetation ];
			twocost = ele_cost[ sptr->altitude ];

			if( onecost == -1 || twocost == -1 )
				movecost[x][y] = -2;
			else
				movecost[x][y] = onecost + twocost;
		} /* if */
	} /* for */
} /* updmove() */

/*get selected army or navy: if navy, number is MAXARM+nvynum*/
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

flee(x,y,z)
{
	/*count is number of acceptable sectors*/
	int count=0;
	int i,j;
	/*flee*/
	/*check if next to anybody of the sectors owners race*/
	for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++)
		if(i>=0&&i<MAPX&&j>=0&&j<MAPY
		&&(ntn[sct[i][j].owner].race==ntn[sct[x][y].owner].race)) 
			count++;

	if(count>0) {
	if(z==0) mvaddstr(LINES-2,20,"CIVILIANS ABANDON SECTOR");
	for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++)
		if(i>=0&&i<MAPX&&j>=0&&j<MAPY
		&&(ntn[sct[i][j].owner].race==ntn[sct[x][y].owner].race)) {
			sct[i][j].people += sct[x][y].people / count;
			if(z==0) SADJCIV2;
		}
	}
	else {
	sct[x][y].people /= 2;
	for(i=x-4;i<=x+4;i++) for(j=y-4;j<=y+4;j++)
		if(i>=0&&i<MAPX&&j>=0&&j<MAPY
		&&(ntn[sct[i][j].owner].race==ntn[sct[x][y].owner].race)) 
			count++;
	if(count>0) {
	if(z==0) mvaddstr(LINES-2,20,"PEOPLE FLEE SECTOR AND HALF DIE");
	for(i=x-4;i<=x+4;i++) for(j=y-4;j<=y+4;j++)
		if(i>=0&&i<MAPX&&j>=0&&j<MAPY
		&&(ntn[sct[i][j].owner].race==ntn[sct[x][y].owner].race)) {
			sct[i][j].people += sct[x][y].people / count;
			if(z==0) SADJCIV2;
		}
	}
	else if(z==0) mvaddstr(LINES-2,20,"PEOPLE IN SECTOR DIE");
	}

	sct[x][y].people = 0;
	if(z==0) SADJCIV;
	sct[x][y].fortress=0;
	/*SINFORT;*/
	if(isdigit(sct[XREAL][YREAL].vegetation)!=0) {
		sct[x][y].designation=DDEVASTATED;
		if(z==0) SADJDES2;
	}
}

