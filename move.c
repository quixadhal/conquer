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
#include "header.h"
#include <ctype.h>

extern FILE *fexe;
/*redraw map in this turn if redraw is a 1*/
extern short redraw;
extern short selector;
extern short xcurs,ycurs,xoffset,yoffset;
extern short occ[MAPX][MAPY];
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
	redraw=TRUE;
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
		if(capacity>0){
			mvaddstr(LINES-2,0,"Transport (a)rmy,(c)iv,(b)oth a & c,(n)othing:");
			clrtoeol();
			refresh();
			carried=getch();
		}
		else carried='n';

		/*transport army*/
		if(carried=='a'||carried=='b'){
			mvprintw(LINES-2,0,"CAPACITY=%d ;WHAT ARMY TO XPORT?:",capacity);
			clrtoeol();
			refresh();
			scanw("%hd",&armynum);
			if((armynum<MAXARM)&&(armynum>=0)&&(AXLOC==XREAL)&&(AYLOC==YREAL)){
				if(ASOLD>capacity){
					mvaddstr(LINES-1,0,"ERROR: TOO MANY TROOPS");
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
			scanw("%d",&ctransport);
			if(ctransport>0&&ctransport<=sct[XREAL][YREAL].people&&(ctransport<capacity)){
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
				if((movecost[XREAL][YREAL]<0)||(movecost[XREAL][YREAL]>mveleft)) {
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
						done=0;
						NMOVE=0;
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
			if((ntn[Tnation].arm[Tarmynum].sold>0)&&(ntn[Tnation].arm[Tarmynum].xloc==XREAL)&&(ntn[Tnation].arm[Tarmynum].yloc==YREAL)&&((ntn[country].dstatus[Tnation]>=HOSTILE)||(ntn[Tnation].dstatus[country]>=HOSTILE))&&(ntn[Tnation].arm[Tarmynum].stat!=SCOUT))
				total+=ntn[Tnation].arm[Tarmynum].sold;
		}
		/*naval total is number of at war WARSHIPS within one sector*/
		else {
			for(Tnation=0;Tnation<NTOTAL;Tnation++) 
			if(Tnation!=country) 
			for(Tarmynum=0;Tarmynum<MAXNAVY;Tarmynum++) 
			if((ntn[Tnation].nvy[Tarmynum].warships>0)&&(ntn[Tnation].nvy[Tarmynum].xloc<=XREAL+1)&&(ntn[Tnation].nvy[Tarmynum].xloc>=XREAL-1)&&(ntn[Tnation].nvy[Tarmynum].yloc<=YREAL+1)&&(ntn[Tnation].nvy[Tarmynum].yloc>=YREAL-1)&&(ntn[Tnation].dstatus[country]>=
HOSTILE))
				total+=ntn[Tnation].nvy[Tarmynum].warships;
		}

		/*if moved and not done*/
		if((valid==1)&&(done==0)){
			/*check if offmap and correct*/
			if((xcurs<1)||(ycurs<1)||(xcurs>=(COLS-21)/2)||((ycurs>=LINES-5))||((XREAL)>=MAPX)||((YREAL)>=MAPY)) {
				clear();
				offmap();
				makemap();
			}

			mvaddstr(LINES-3,0,"                  ");

			if((armornvy==ARMY)&&(ASTAT!=SCOUT)){
				if((sct[XREAL][YREAL].owner!=0)&&(sct[XREAL][YREAL].owner!=country)&&(sct[XREAL][YREAL].people>100)&&(ntn[sct[XREAL][YREAL].owner].dstatus[country]>FRIENDLY)&&(ntn[country].dstatus[sct[XREAL][YREAL].owner]<WAR)){
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
						mvprintw(LINES-3,0,"Zone Of Control -- hit return");
						beep();
						done=1;
					}
					else mvaddstr(LINES-3,0,"SAFE -- hit return");
				}
			}
			else if((armornvy==NAVY)&&(total>0)){
				/*25% stop if they have > total*/
				if((ntn[country].nvy[nvynum].warships < total)&&(rand()%4==0)){
					NMOVE=0;
					NADJMOV;
					mvprintw(LINES-3,0,"%d Enemy WarShips Sighted -- hit return",total);
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

			mvprintw(LINES-1,20,"move cost is %d  ",movecost[XREAL][YREAL]);
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
			else if((sct[AXLOC][AYLOC].owner!=country)&&((sct[AXLOC][AYLOC].designation==DCITY)||(sct[AXLOC][AYLOC].designation==DCAPITOL))){
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
				if((ntn[Tnation].arm[Tarmynum].xloc==XREAL)&&(ntn[Tnation].arm[Tarmynum].yloc==YREAL)&&((ntn[x].dstatus[Tnation]>=HOSTILE)||(ntn[Tnation].dstatus[x]>=HOSTILE))) {
					country=Tnation;
					armynum=Tarmynum;
					ASOLD=0;
					AADJMEN;
				}
				country=x;
				armynum=y;
				if(sct[XREAL][YREAL].owner!=country){
				if((sct[XREAL][YREAL].people>0)&&(ntn[sct[XREAL][YREAL].owner].race!=ntn[country].race)) flee(XREAL,YREAL,0);
				SOWN=country;
				SADJOWN;
				sct[AXLOC][AYLOC].designation=DDEVASTATED;
				SADJDES;
				}
				beep();
			}
			else if((SOWN!=country)&&((occ[XREAL][YREAL]==0)||(occ[XREAL][YREAL]==country))&&(ntn[country].dstatus[SOWN]>HOSTILE)&&((ASTAT==ATTACK)||(ASTAT==DEFEND))) {
				/*people flee if not of same race*/
				if((sct[XREAL][YREAL].people>0)&&(ntn[sct[XREAL][YREAL].owner].race!=ntn[country].race)) flee(XREAL,YREAL,0);
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
		if((carried=='a'||carried=='b')&&(sct[XREAL][YREAL].altitude!=WATER)&&(armynum>=0)&&(armynum<MAXARM)) {
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
		if((carried=='b'||carried=='c')&&(sct[XREAL][YREAL].altitude!=WATER)&&(ctransport>0)) {
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
	}
	redraw=FALSE;
	makemap();
	armornvy=AORN;
}

/*movecost contains movement cost unless water (-1) or unenterable land (-2)*/
updmove(race)
{
	int i,j;
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

	for(x=0;x<MAPX;x++) for(y=0;y<MAPX;y++){

		onecost=(-1);
		twocost=(-1);
		if(race==GOD) movecost[x][y]=0;
		else if(sct[x][y].altitude==WATER) movecost[x][y]=(-1);
		else if(((magic(country,DERVISH)==1)||(magic(country,DESTROYER)==1))&&((sct[x][y].vegitation==ICE)||(sct[x][y].vegitation==DESERT))) movecost[x][y]=1;
		else {
			for(j=0;j<=10;j++) if(sct[x][y].vegitation==*(veg+j))
				for(i=0;i<=9;i++) switch(race) {
				case ELF:
					if(*(EVegcost+j)==*(numbers+i)) onecost=i;
					break;
				case DWARF:
					if(*(DVegcost+j)==*(numbers+i)) onecost=i;
					break;
				case ORC:
					if(*(OVegcost+j)==*(numbers+i)) onecost=i;
					break;
				case HUMAN:
					if(*(HVegcost+j)==*(numbers+i)) onecost=i;
					break;
				default:
					if(*(XVegcost+j)==*(numbers+i)) onecost=i;
					break;
				}

			for(j=0;j<=4;j++) if(sct[x][y].altitude==*(ele+j)) for(i=0;i<=9;i++) switch(race) {
			case ELF:
				if(*(EElecost+j)==*(numbers+i)) twocost=i;
				break;
			case DWARF:
				if(*(DElecost+j)==*(numbers+i)) twocost=i;
				break;
			case ORC:
				if(*(OElecost+j)==*(numbers+i)) twocost=i;
				break;
			case HUMAN:
				if(*(HElecost+j)==*(numbers+i)) twocost=i;
				break;
			default:
				if(*(XElecost+j)==*(numbers+i)) twocost=i;
				break;
			}

			if(onecost==-1) movecost[x][y]=(-2);
			else if(twocost==-1) movecost[x][y]=(-2);
			else movecost[x][y]=onecost+twocost;
		}
	}
}

/*get selected army or navy: navy number is MAXARM+nvynum*/
getselunit()
{
	int number=(-1);
	short armynum=0;
	short nvynum=0;
	int count=0;
	/*count is order of that army in sector*/
	/*armynum is number of that army*/
	for(armynum=0;armynum<MAXARM;armynum++){
		if((ASOLD>0)&&(AXLOC==XREAL)&&(AYLOC==YREAL)) {
			if(selector==count*2) number=armynum;
			count++;
		}
	}

	if(number==(-1)){
		for(nvynum=0;nvynum<MAXNAVY;nvynum++)
			if((NMER+NWAR>0)&&(NXLOC==XREAL)&&(NYLOC==YREAL)) {
				if(selector==2*(count)) number=MAXARM+nvynum;
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
		if(i>=0&&i<MAPX&&j>=0&&j<MAPY&&(ntn[sct[i][j].owner].race==ntn[sct[x][y].owner].race)) count++;

	if(count>0) {
	mvaddstr(LINES-2,20,"CIVILIANS ABANDON SECTOR");
	for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++)
		if(i>=0&&i<MAPX&&j>=0&&j<MAPY&&(ntn[sct[i][j].owner].race==ntn[sct[x][y].owner].race)) {
			sct[i][j].people += sct[x][y].people / count;
			if(z==0) SADJCIV2;
		}
	}
	else {
	sct[x][y].people /= 2;
	for(i=x-4;i<=x+4;i++) for(j=y-4;j<=y+4;j++)
		if(i>=0&&i<MAPX&&j>=0&&j<MAPY&&(ntn[sct[i][j].owner].race==ntn[sct[x][y].owner].race)) count++;
	if(count>0) {
	mvaddstr(LINES-2,20,"PEOPLE FLEE SECTOR AND HALF DIE");
	for(i=x-4;i<=x+4;i++) for(j=y-4;j<=y+4;j++)
		if(i>=0&&i<MAPX&&j>=0&&j<MAPY&&(ntn[sct[i][j].owner].race==ntn[sct[x][y].owner].race)) {
			sct[i][j].people += sct[x][y].people / count;
			if(z==0) SADJCIV2;
		}
	}
	else mvaddstr(LINES-2,20,"PEOPLE IN SECTOR DIE");
	}

	sct[x][y].people = 0;
	if(z==0) SADJCIV;
	sct[x][y].fortress=0;
	/*SINFORT;*/
	if(isdigit(sct[XREAL][YREAL].vegitation)!=0) {
		sct[x][y].designation=DDEVASTATED;
		if(z==0) SADJDES2;
	}
}
