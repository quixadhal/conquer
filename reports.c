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


/*	screen subroutines	*/

/*include files*/
#include <ctype.h>
#include "header.h"
#include "data.h"

extern FILE *fexe;
extern short country;
extern long startgold;

/*report on armies and allow changes*/
armyrpt()
{
	int i,j;
	int chg;
	short army;
	int men;
	int oldx,oldy;
	short oldarmy;
	int done=0;
	int position;
	int isgod=0;
	int count;    /*number of armies on current screen */
	short armynum=0;    /*current nation id */
	if(country==0) {
		standout();
		isgod=1;
		clear();
		mvaddstr(0,0,"SUPER USER; FOR WHAT NATION NUMBER:");
		clrtoeol();
		standend();
		refresh();
		country = get_number();
		if(country<0||country>NTOTAL) return;
	}
	armynum=0;
	/*new army screen*/
	while(done==0) {
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

		position=5;
		count=0;
		while((armynum<MAXARM)&&(count!=6)){
			if(ASOLD>0) {
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
			}
			armynum++;
		}
		if(armynum>=MAXARM) done=1;

		standout();
		mvaddstr(12,(COLS/2)-10,"HIT SPACE KEY IF DONE");
		mvaddstr(13,(COLS/2)-14,"HIT RETURN TO CHANGE AN ARMY");
		mvaddstr(14,(COLS/2)-14,"HIT ANY OTHER KEY TO CONTINUE");
		standend();
		refresh();
		if ((army=getch())==' ') done=1;
		if (army=='\n'){
			mvaddstr(16,0,"WHAT ARMY DO YOU WANT TO CHANGE:");
			clrtoeol();
			refresh();
			armynum = get_number();
			if((armynum<0)||(armynum>MAXARM)) return;
			mvaddstr(18,0,"1) CHANGE STATUS, 2) TRANSFER / MERGE, 3) SPLIT ARMY, 4) DISBAND ARMY:");
			clrtoeol();
			if(isgod==1) mvaddstr(20,0,"5) LOCATION, 6) SOLDIERS:");
			refresh();
			switch(getch()){
			case '1':
				if(ASTAT==SCOUT){
				mvaddstr(21,0,"CANT CHANGE STATUS ON SCOUTS");
				refresh();
				getch();
				break;
				}
				mvaddstr(21,0,"1=MARCH, 2=SCOUT, 3=ATTACK, 4=DEFEND, 5=GARRISON");
				clrtoeol();
				refresh();
				chg = get_number();
				if(chg<1) return;
				if(chg>5) return;
				if((chg==SCOUT)&&(ASOLD>25)){
					clear();
					mvaddstr(12,(COLS/2)-6,"NEED < 25 MEN TO SCOUT");
					mvaddstr(13,(COLS/2)-12,"HIT ANY KEY TO CONTINUE");
					refresh();
					getch();
					if(isgod==1) country=0;
					return;
				}
				else if((chg>0)&&(chg<7)) {
					ASTAT=chg; 
					AADJSTAT;
				}
				break;
			case '2':
				oldx=AXLOC;
				oldy=AYLOC;
				oldarmy=armynum;
				mvaddstr(22,0,"TO WHAT ARMY: ");
				clrtoeol();
				refresh();
				armynum = get_number();
				if(armynum==oldarmy) {
					mvprintw(23,0,"SORRY -- SAME (%d,%d",armynum,oldarmy);
					refresh();
					getch();
				}
				else if((armynum<0)||(armynum>MAXARM)){
					mvprintw(23,0,"SORRY -- INVALID ARMY %d",armynum);
					refresh();
					getch();
				}
				else if(ASTAT==SCOUT){
					mvaddstr(23,0,"SORRY -- TARGET ARMY IS SCOUTING");
					refresh();
					getch();
				}
				else if((oldx==AXLOC)&&(oldy==AYLOC)&&(ASOLD>0)) {
					ASOLD+= ntn[country].arm[oldarmy].sold;
					ntn[country].arm[oldarmy].sold=0;
					AADJMEN;
					if(AMOVE>ntn[country].arm[oldarmy].smove)
						AMOVE=ntn[country].arm[oldarmy].smove;
					AADJMOV;
					armynum=oldarmy;
					AADJMEN;
				}
				else {
					mvaddstr(23,0,"Armies not together (hit any key) ");
					refresh();
					getch();
				}
				break;
			case '3':
				mvaddstr(21,0,"HOW MANY MEN TO SPLIT: ");
				clrtoeol();
				refresh();
				men = get_number();
				if((armynum<0)||(armynum>MAXARM)) return;
				if((men<25)||(ASOLD-men<25)){
					mvaddstr(23,0,"TOO FEW MEN TRANSFERED OR LEFT");
					refresh();
					getch();
				}
				else if(men<ASOLD){
					ASOLD-=men;
					AADJMEN;
					oldarmy=armynum;
					oldx=AXLOC;
					oldy=AYLOC;
					armynum=(-1);
					for(army=0;army<MAXARM;army++)
						if(ntn[country].arm[army].sold==0){
							if(armynum==(-1)) armynum=army;
						}
					/*overflow*/
					if(armynum==(-1)) {
						mvaddstr(23,0,"TOO MANY ARMIES: ");
						armynum=oldarmy;
						ASOLD+=men;
						AADJMEN;
					}
					else {
						AMOVE=ntn[country].arm[oldarmy].smove;
						ASTAT=DEFEND;
						AXLOC=oldx;
						AYLOC=oldy;
						ASOLD=men;
						AADJSTAT;
						AADJMEN;
						AADJLOC;
						AADJMOV;
					}
				}
				else {
					mvaddstr(23,0,"TOO MANY MEN : ");
					refresh();
					getch();
				}
				break;
			case '4':
				if(sct[AXLOC][AYLOC].owner!=country){
					mvaddstr(21,0,"YOU DONT OWN SECTOR-- hit return");
					clrtoeol();
					refresh();
					getch();
				}
				else if(magic(country,VAMPIRE)==1){
					mvaddstr(21,0,"VAMPIRES CANT DISBAND--hit return");
					clrtoeol();
					refresh();
					getch();
				}
				else {
					i=AXLOC;
					j=AYLOC;
					sct[i][j].people+=ASOLD;
					ASOLD=0;
					AADJMEN;
					SADJCIV2;
				}
				break;
			case '5':
				if(isgod==1){
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
				if(isgod==1){
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
	if(isgod==1) country=0;
}

budget()
{
	register int xsctr,ysctr;
	short armynum,nvynum;

	int ingold=0,iniron=0,infood=0,incap=0,incity=0;
	long revgold=0,reviron=0,revfood=0,revcap=0,revcity=0;
	long expship=0,expsold=0;
	int isgod=0;
	if(country==0) {
		isgod=1;
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

	for(xsctr=0;xsctr<MAPX;xsctr++) for(ysctr=0;ysctr<MAPX;ysctr++) if(sct[xsctr][ysctr].owner==country) {
		if(sct[xsctr][ysctr].designation==DFARM){
			infood+= sct[xsctr][ysctr].people;
			revfood+=todigit(sct[xsctr][ysctr].vegetation)*sct[xsctr][ysctr].people;
		}
		else if(sct[xsctr][ysctr].designation==DMINE) {
			iniron+= sct[xsctr][ysctr].people;
			reviron+=sct[xsctr][ysctr].iron*sct[xsctr][ysctr].people;
		}
		else if(sct[xsctr][ysctr].designation==DGOLDMINE) {
			ingold+= sct[xsctr][ysctr].people;
			revgold+=sct[xsctr][ysctr].gold*sct[xsctr][ysctr].people;
		}
		else if(sct[xsctr][ysctr].designation==DCAPITOL) incap+= sct[xsctr][ysctr].people;
		else if(sct[xsctr][ysctr].designation==DCITY)    incity+= sct[xsctr][ysctr].people;
	}

	revfood *= TAXFOOD / 100;
	reviron *= TAXIRON / 100;
	revgold *= TAXGOLD / 100;
	revcap = (long) incap * TAXCAP / 100;
	revcity = (long) incity * TAXCITY / 100;
	if( magic(country,ARCHITECT) ) {
		revcap *= 2;
		revcity *= 2;
	}

	for(armynum=0;armynum<MAXARM;armynum++) if(ASOLD>0) expsold+= ASOLD;
	for(nvynum=0;nvynum<MAXNAVY;nvynum++)
		if(NWAR+NMER>0) expship+=(NWAR+NMER);

	standout();
	mvprintw(5,0,  "nation name is ...%s   ",ntn[country].name);
	mvprintw(6,0,  "gold in treasury..$%8ld",ntn[country].tgold);
	standend();
	if(ntn[country].tfood<ntn[country].tciv) standout();
	mvprintw(9,0,  "granary holds ....%8ld",ntn[country].tfood);
	standend();
	mvprintw(10,0, "jewels owned is...%8ld",ntn[country].jewels);
	mvprintw(11,0, "iron ore owned is.%8ld",ntn[country].tiron);
	mvprintw(7,30, "%5d people in gold mines:%8ld",ingold,revgold);
	mvprintw(8,30, "%5d people in iron mines:%8ld",iniron,reviron);
	mvprintw(9,30, "%5d people in farms:     %8ld",infood,revfood);
	mvprintw(10,30,"%5d people in capital:   %8ld",incap,revcap);
	mvprintw(11,30,"%5d people in cities:    %8ld",incity,revcity);
	standout();
	mvprintw(12,30,"%5ld people TOTAL INCOME: %8ld",ntn[country].tciv,revfood+reviron+revgold+(incap*TAXCAP/100)+(incity*TAXCITY/100));
	standend();

	if(magic(country,VAMPIRE)!=1) 
	mvprintw(14,30,"%5d troops at %5d each:%8d",expsold,SOLDMAINT,expsold*SOLDMAINT);
	else
	mvprintw(14,30,"%5d troops at     0 each:0",expsold);
	mvprintw(15,30,"%5d ships at %5d each: %8d",expship,SHIPMAINT,expship*SHIPMAINT);
	mvprintw(16,30,"other expenses this turn:  %8ld",startgold-ntn[country].tgold);
	standout();
	if(magic(country,VAMPIRE)!=1) {
	mvprintw(17,30,"TOTAL EXPENSES:            %8ld",(expsold*SOLDMAINT)+(expship*SHIPMAINT)+startgold-ntn[country].tgold);
	mvprintw(18,30,"NET INCOME:                %8ld",revfood+reviron+revgold+(incap*TAXCAP/100)+(incity*TAXCITY/100)-(expsold*SOLDMAINT)-(expship*SHIPMAINT)-startgold+ntn[country].tgold);
	} else {
	mvprintw(17,30,"TOTAL EXPENSES:            %8ld",(expship*SHIPMAINT)+startgold-ntn[country].tgold);
	mvprintw(18,30,"NET INCOME:                %8ld",revfood+reviron+revgold+(incap*TAXCAP/100)+(incity*TAXCITY/100)-(expship*SHIPMAINT)-startgold+ntn[country].tgold);
	}

	mvaddstr(20,(COLS/2)-13,"HIT 'P' TO SEE PRODUCTION STATS");
	mvaddstr(21,(COLS/2)-12,"HIT ANY OTHER KEY TO CONTINUE");
	standend();
	refresh();
	if(getch()=='P'){
		produce();
	}
	if(isgod==1) country=0;
}

produce()
{
	register int xsctr,ysctr;
	short armynum;
	int civilians=0, military=0;
	int ingold=0 ,indesert=0,iniron=0,infood=0;
	long revgold=0,reviron=0,revdesert=0,revfood=0;
	int isgod=0;

	if(country==0) {
		isgod=1;
		clear();
		mvaddstr(0,0,"SUPER USER; FOR WHAT NATION NUMBER:");
		clrtoeol();
		refresh();
		country = get_number();
	}
	clear();
	standout();
	mvaddstr(0,(COLS/2)-20,"NEXT YEARS PRODUCTION ESTIMATES");
	standend();

	for(xsctr=0;xsctr<MAPX;xsctr++) for(ysctr=0;ysctr<MAPX;ysctr++) if(sct[xsctr][ysctr].owner==country) {
		civilians += sct[xsctr][ysctr].people;

		if(sct[xsctr][ysctr].designation==DFARM){
			infood+= sct[xsctr][ysctr].people;
			revfood+=todigit(sct[xsctr][ysctr].vegetation)*sct[xsctr][ysctr].people;
		}
		else if(sct[xsctr][ysctr].designation==DMINE) {
			iniron+= sct[xsctr][ysctr].people;
			reviron+=sct[xsctr][ysctr].iron*sct[xsctr][ysctr].people;
		}
		else if(sct[xsctr][ysctr].designation==DGOLDMINE) {
			ingold+= sct[xsctr][ysctr].people;
			revgold+=sct[xsctr][ysctr].gold*sct[xsctr][ysctr].people;
		}
		else if(((magic(country,DERVISH)==1)
		||(magic(country,DESTROYER)==1))
		&&((sct[xsctr][ysctr].vegetation==ICE)
		||(sct[xsctr][ysctr].vegetation==DESERT))
		&&(sct[xsctr][ysctr].people>0)) {
			indesert+= sct[xsctr][ysctr].people;
			revdesert+=6*sct[xsctr][ysctr].people;
		}
	}
	if(magic(country,MINER)==1) {
		mvaddstr(20,0,"MINER");
		reviron*=2;
		revgold*=2;
	}

	standout();
	mvprintw(5,0,  "nation name is ...%s   ",ntn[country].name);
	mvprintw(6,0,  "gold in treasury..$%8ld",ntn[country].tgold);
	standend();
	mvaddstr(8,0,  "FOOD PRODUCTION");
	mvprintw(9,0,  "granary now holds......%8ld tons",ntn[country].tfood);
	mvprintw(10,0, "%5d people in farms..%8ld tons",infood,revfood);
	if((magic(country,DERVISH)==1)||(magic(country,DESTROYER)==1)) {
	mvprintw(11,0, "dervish: %5d people..%8d tons",indesert,revdesert);
	}
	for(armynum=0;armynum<MAXARM;armynum++) if(ASOLD>0) military+=ASOLD;
	mvprintw(12,0, "%5d civilians eat....%8d tons",civilians,civilians);
	mvprintw(13,0, "%5d soldiers  eat....%8d tons",military,military*2);
	mvprintw(15,0, "TOTAL NET FOOD.........%8d tons",revfood+revdesert-civilians-military*2);
	if(ntn[country].tfood+revfood+revdesert-civilians-military*2<FOODTHRESH*civilians) {
	mvprintw(16,0, "TOTAL NEXT YEARS FOOD..%8ld tons",ntn[country].tfood+revfood+revdesert-civilians-military*2);
	}
	else{
	mvprintw(16,0, "TOTAL NEXT YEARS FOOD..%8ld tons",FOODTHRESH*civilians);
	mvprintw(17,0, "REVENUE FROM EXCESS....%8ld gold",ntn[country].tfood+revfood+revdesert-civilians-military*2-FOODTHRESH*civilians);
	}

	mvaddstr(8,40,  "OTHER PRODUCTION");
	mvprintw(10,40, "jewels owned is............%8ld",ntn[country].jewels);
	mvprintw(11,40, "%5d people in goldmines :%8ld",ingold,revgold);
	mvprintw(12,40, "ESTIMATE NEXT YEARS JEWELS %8ld",ntn[country].jewels+revgold);
	mvprintw(14,40, "iron ore owned is..........%8ld",ntn[country].tiron);
	mvprintw(15,40, "%5d people in iron mines:%8ld",iniron,reviron);
	mvprintw(16,40, "ESTIMATE NEXT YEARS IRON   %8ld",ntn[country].tiron+reviron);

	standout();
	mvaddstr(20,(COLS/2)-12,"HIT 'B' TO SEE BUDGET SCREEN");
	mvaddstr(21,(COLS/2)-12,"HIT ANY OTHER KEY TO CONTINUE");
	standend();
	refresh();
	if(getch()=='B'){
		budget();
	}

	if(isgod==1) country=0;
}

/*report on ships and allow changes */
fleetrpt()
{
	short navy;
	short oldx,oldy,oldnavy;
	short done=0;
	int position;
	int count;       /*screen number */
	short nvynum=0;    /*current ship id */
	short wships,mships;
	int isgod=0;
	if(country==0) {
		standout();
		isgod=1;
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
		done=1;
		refresh();
		if(isgod==1) country=0;
		return;
	}
	nvynum=0;
	while(done==0) {
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
		mvaddstr(7,0,"move left :");

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
				mvprintw(7,position,"%d",NMOVE);
			}
			nvynum++;
		}
		if(nvynum>=MAXNAVY) done=1;

		standout();
		mvaddstr(12,(COLS/2)-10,"HIT SPACE KEY IF DONE");
		mvaddstr(13,(COLS/2)-14,"HIT RETURN TO CHANGE A NAVY");
		mvaddstr(14,(COLS/2)-14,"HIT ANY OTHER KEY TO CONTINUE");
		standend();
		refresh();
		if ((navy=getch())==' ') done=1;
		if (navy=='\n'){
			mvaddstr(16,0,"WHAT NAVY DO YOU WANT TO CHANGE:");
			clrtoeol();
			refresh();
			nvynum = get_number();
			if((nvynum<0)||(nvynum>MAXNAVY)) return;
			mvaddstr(18,0,"1) TRANSFER / MERGE, 2) SPLIT NAVY, 3) DISBAND NAVY:");
			clrtoeol();
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
				if(nvynum==oldnavy) {
					mvprintw(23,0,"SORRY -- SAME (%d,%d)",nvynum,oldnavy);
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
	if(isgod==1) country=0;
}
