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

/*	commands.c		*/

/*include files*/
#include "header.h"
#include "data.h"
#include <ctype.h>

/*execute file pointer*/
extern FILE *fexe;
extern short country;
extern short selector;
extern short pager;
extern short xcurs,ycurs,xoffset,yoffset;

/*change current hex designation*/
void
redesignate()
{
	char newdes;
	short x,y;

	if(country==0){
		clear_bottom(0);
		mvaddstr(LINES-4,0,"SUPER USER: CHANGE (v)egetation, (e)levation, (d)esig, (o)wner, (m)inerals");
		refresh();
		switch(getch()){
		case 'e':
			/*simple contour map definitions*/
			mvprintw(LINES-3,7,"ELEVATIONS: change to %c, %c, %c, %c or %c?",WATER,PEAK,MOUNTAIN,HILL,CLEAR);
			refresh();
			newdes=getch();
			if(newdes!=WATER&&newdes!=PEAK&&newdes!=MOUNTAIN
			&&newdes!=HILL&&newdes!=CLEAR) return;
			sct[XREAL][YREAL].altitude=newdes;
			if((newdes==PEAK)||(newdes==WATER)) {
				sct[XREAL][YREAL].owner=0;
				sct[XREAL][YREAL].people=0;
				sct[XREAL][YREAL].fortress=0;
			}
			/*will fall through as must change vegetation*/
		case 'v':
			/*vegetation types*/
			mvprintw(LINES-3,7,"VEGETATIONS: change to %c, %c, %c, %c, %c, %c, %c, %c, %c, %c, %c or %c?",
			VOLCANO,DESERT,TUNDRA,BARREN,LT_VEG,
			GOOD,WOOD,FOREST,JUNGLE,SWAMP,ICE,NONE);
			refresh();
			newdes=getch();
			if(newdes!=VOLCANO
			&&newdes!=DESERT&&newdes!=TUNDRA
			&&newdes!=BARREN&&newdes!=LT_VEG
			&&newdes!=NONE&&newdes!=GOOD
			&&newdes!=WOOD&&newdes!=FOREST&&newdes!=JUNGLE
			&&newdes!=SWAMP&&newdes!=ICE) return;
			sct[XREAL][YREAL].vegetation=newdes;
			if(tofood(sct[XREAL][YREAL].vegetation,0)!=0)
				sct[XREAL][YREAL].designation=newdes;
			return;
		case 'o':
			mvprintw(LINES-3,7,"what nation owner:");
			refresh();
			x = get_number();
			if (x<NTOTAL) sct[XREAL][YREAL].owner=x;
			return;
		case 'm':
			mvaddstr(LINES-2,7,"(g)old, or (i)ron?");
			refresh();
			switch(getch()) {
			case 'g':
				mvaddstr(LINES-2,7,"new sector gold value: ");
				refresh();
				x = get_number();
				if (x<100 && x>=0)
					sct[XREAL][YREAL].gold = x;
				break;
			case 'i':
				mvaddstr(LINES-2,7,"new sector iron value:");
				refresh();
				x = get_number();
				if (x<100 && x>=0)
					sct[XREAL][YREAL].iron = x;
				break;
			default:
				break;
			}
			return;
		}
	}
	if((country!=0)&&(sct[XREAL][YREAL].owner!=country)) {
		errormsg("You do not own");
		return;
	}
	clear_bottom(0);
	mvprintw(LINES-4,7,"$%d: redesignate to %c, %c, %c",DESCOST,DMINE,DGOLDMINE,DFARM);
	mvprintw(LINES-3,7,"$%d: redesignate to %c, %c?",20*DESCOST,DCITY,DCAPITOL);
	mvprintw(LINES-2,0,"space to return => what new designation:");
	refresh();
	/*read answer*/
	newdes=getch();

	if(newdes!=DMINE&&newdes!=DGOLDMINE&&newdes!=DFARM&&newdes!=DCITY&&newdes!=DCAPITOL) {
		errormsg("invalid input");
		return;
	}

	if((SOWN==country)||(country==0)) {
		if(((tofood(sct[XREAL][YREAL].vegetation,country)>=DESFOOD)
		||(country==0))){
			if(((newdes!=DCITY)&&(newdes!=DCAPITOL))||(country==0)) {
				/*decrement treasury*/
				sct[XREAL][YREAL].designation=newdes;
				if(country==0) {
					country=sct[XREAL][YREAL].owner;
					SADJDES;
					country=0;
				}
				else {
					SADJDES;
					ntn[country].tgold-=DESCOST;
				}
			}
			else if((newdes==DCAPITOL)
			&&(sct[XREAL][YREAL].designation==DCITY)){

				ntn[country].tgold-=20*DESCOST;
				ntn[country].tiron-=DESCOST;
				x=ntn[country].capx;
				y=ntn[country].capy;
				sct[x][y].designation=DCITY;
				if(country==0) {
					country=sct[x][y].owner;
					SADJDES2;
					country=0;
				}
				else SADJDES2;
				sct[XREAL][YREAL].designation=newdes;
				ntn[country].capx=XREAL;
				ntn[country].capy=YREAL;
				if(country==0) {
					country=sct[XREAL][YREAL].owner;
					SADJDES;
					country=0;
				}
				else SADJDES;
			}
			else if((newdes==DCAPITOL)
			&&(sct[XREAL][YREAL].designation!=DCITY)){
				errormsg("New Capitol must be in a town");
			}
			else if((ntn[country].tiron>DESCOST)
			&&(newdes==DCITY)){
				if(sct[XREAL][YREAL].people>=500){
					ntn[country].tgold-=20*DESCOST;
					ntn[country].tiron-=DESCOST;
					sct[XREAL][YREAL].designation=newdes;
					if(country==0) {
						country=sct[XREAL][YREAL].owner;
						SADJDES;
						country=0;
					}
					else SADJDES;
				}
				else {
				errormsg("Need 500 people to build to town");
				}
			} else	errormsg("Not enough iron for town");
		} else	errormsg("vegetation too sparse");
	} else {
	errormsg("Sorry, you don't own sector");
	}
}

/*build fort or ship-type */
void
construct()
{
	int nearsea,repairs;
	long cost;
	int armbonus;
	int x,y;
	short nvynum=0;
	short mnumber,wnumber;
	short isgod=FALSE;
	char type;

	clear_bottom(0);
	if(country==0){
		isgod=TRUE;
		country=sct[XREAL][YREAL].owner;
	}

	if(sct[XREAL][YREAL].owner!=country) {
		errormsg("You do not own");
		if(isgod==TRUE) country=0;
		return;
	}

	if((isgod==FALSE) && (ntn[country].tgold < 0 )) {
		errormsg("You are broke");
		return;
	}

	if((sct[XREAL][YREAL].designation==DCITY)
	||(sct[XREAL][YREAL].designation==DCAPITOL)) {
		/*calculate cost for fort*/
		cost=FORTCOST;
		if(isgod==TRUE) cost=0;
		else for(x=1;x<=sct[XREAL][YREAL].fortress;x++)
			cost*=2;

		if(sct[XREAL][YREAL].designation==DCITY) armbonus=8;
		else if(sct[XREAL][YREAL].designation==DCAPITOL) armbonus=10;
		if(magic(country,ARCHITECT)==TRUE) armbonus*=2;

		mvprintw(LINES-4,0,"Construct <f>ortifications (+%d%% - %ld gold) or <s>hips?:",armbonus,cost);
		refresh();
		type=getch();
	} else {
		errormsg("Must construct in town or capitol");
		if (isgod==1) country=0;
		return;
	}

	/* construct ships*/
	if (type=='s') {
		/*check if next to sea*/
		nearsea=0;
		for(x=XREAL-1;x<=XREAL+1;x++)
			for(y=YREAL-1;y<=YREAL+1;y++)
				if(sct[x][y].altitude==WATER) nearsea=1;

		if (nearsea==0) {
			errormsg("not in a harbor");
			if(isgod==TRUE) country=0;
			return;
		}

		repairs=FALSE;
		if((nvynum=getselunit()-MAXARM)>=0){
			if(nvynum>=MAXNAVY){
				errormsg("INVALID NAVY");
				if(isgod==TRUE) country=0;
				return;
			}
			mvaddstr(LINES-3,0,"Do you wish to repair existing fleet (y or n)");
			refresh();
			if(getch()=='y') repairs=TRUE;
			else{
				mvaddstr(LINES-3,0,"Do you wish to raise a new fleet (y or n)");
				clrtoeol();
				refresh();
				if(getch()=='y') nvynum=(-1);
			}
		}

		if(repairs==TRUE){
			clear_bottom(0);
			mvprintw(LINES-4,0,"Repairing Fleet %d",nvynum);
			mvaddstr(LINES-3,0,"How many crew per ship do you wish to add:");
			clrtoeol();
			refresh();
			mnumber = get_number();
			cost = (long) mnumber*WARSHPCOST*NWAR + (long) mnumber*MERSHPCOST*NMER;
			if( ntn[country].tgold < cost ) {
				errormsg("NOT ENOUGH GOLD");
				if(isgod==TRUE) country=0;
				return;
			}
			if( sct[XREAL][YREAL].people < (NWAR+NMER) * mnumber ){
				errormsg("NOT ENOUGH CIVILIANS IN SECTOR");
				if(isgod==TRUE) country=0;
				return;
			}
			if( SHIPCREW * (NWAR+NMER) < NCREW + (NWAR+NMER) * mnumber ){
				errormsg("THAT WOULD EXCEED MAXIMUM SHIP CREW");
				if(isgod==TRUE) country=0;
				return;
			}
	
			if(magic(country,SAILOR)==TRUE){
				ntn[country].tgold -= cost/2;
			} else {
				ntn[country].tgold -= cost;
			}

			NCREW += (NWAR+NMER) * mnumber;
			NADJCRW;
			if(isgod==TRUE) country=0;
			return;
		}

		if(nvynum<0) {
			nvynum=0;
			x=(-1);
			while((x==(-1))&&(nvynum<MAXNAVY)) {
				if(NWAR+NMER<=0) {
					x=nvynum;
					NWAR=0;
					NMER=0;
					NADJSHP;
				}
				nvynum++;
			}
			nvynum=x;
		}
		if(nvynum<0){
			errormsg("NO FREE NAVIES");
			if(isgod==TRUE) country=0;
			return;
		}
		else	mvprintw(LINES-2,30,"raising new fleet %d",nvynum);


		mvprintw(LINES-3,0,"how many merchants:");
		refresh();
		mnumber = get_number();
		mvprintw(LINES-3,40,"how many warships:");
		refresh();
		wnumber = get_number();
		/*sanity checks*/
		if((wnumber>100)||(wnumber<0)) wnumber=0;
		if((mnumber>100)||(mnumber<0)) mnumber=0;
		if (wnumber==0 && mnumber==0)
		{
			if(isgod==1) country=0;
			return;
		}
		cost =  wnumber*WARSHPCOST + mnumber*MERSHPCOST;
		if(isgod==TRUE)				cost=0;
		else if(magic(country,SAILOR)==TRUE)	cost/=2;

		if(cost > ntn[country].tgold ) {
			errormsg("sorry - not enough gold");
			if(isgod==TRUE) country=0;
			return;
		}

		if( sct[XREAL][YREAL].people < (wnumber+mnumber) * SHIPCREW ){
			errormsg("NOT ENOUGH CIVILIANS IN SECTOR");
			if(isgod==TRUE) country=0;
			return;
		}

		if((nvynum>=0)&&(nvynum<MAXNAVY)) {
			clear_bottom(0);
			NCREW += (wnumber+mnumber) * SHIPCREW;
			mvprintw(LINES-4,0,"constructing %hd warships and %hd merchants (crew now %d)",wnumber,mnumber,NCREW);

			sct[XREAL][YREAL].people -= (wnumber+mnumber)*SHIPCREW;

			ntn[country].tgold -= cost;

			NXLOC =XREAL;
			NYLOC =YREAL;
			NWAR+=wnumber;
			NMER+=mnumber;
			NMOVE=0;
			SADJCIV;
			NADJCRW;
			NADJSHP;
			NADJLOC;
			NADJMOV;
		} else errormsg("ERROR!!!!!!!!!!!!!");

		mvprintw(LINES-2,30,"adding ships to fleet %d",nvynum);
		mvprintw(LINES-2, 60, "PRESS ANY KEY");
		beep();
		refresh();
		getch();
	}
	/* construct fortification points*/
	else if(type=='f'){
		if(sct[XREAL][YREAL].people>=500)
		/* can only go into debt as much as the nation has jewels */
		if ((ntn[country].tgold - cost) > ((-1)*10*ntn[country].jewels)) {
			mvprintw(LINES-3,25,"you build +%d%% fort points for %ld gold",armbonus,cost);
			ntn[country].tgold-=cost;
			sct[XREAL][YREAL].fortress++;
			INCFORT;
			errormsg("");
		}
		else errormsg("need 500 people or you are broke");
	}
	else errormsg("invalid input error");

	if(isgod==TRUE) country=0;
	refresh();
}

/*DRAFT IF IN A CITY*/
void
draft()
{
	short armynum,x,y,i;
	int men=0,mercs;
	short army=(-1), isgod=FALSE, newtype=0;
	long i_cost, e_cost;

	clear_bottom(0);
	if(country==0) {
		isgod=TRUE;
		country=sct[XREAL][YREAL].owner;
	}
	else if(sct[XREAL][YREAL].owner!=country) {
		errormsg("You do not own");
		return;
	}

	if((sct[XREAL][YREAL].designation!=DCITY)
	&&(sct[XREAL][YREAL].designation!=DCAPITOL)) {
		errormsg("must raise in towns");
		if(isgod==TRUE) country=0;
		return;
	}

	if((sct[XREAL][YREAL].designation==DCITY)
	&&(sct[XREAL][YREAL].people*(2*CITYLIMIT+(ntn[country].tsctrs/2))<ntn[country].tciv)){
		mvprintw(LINES-1,0,"need %d people in town: hit any key",ntn[country].tciv/(2*CITYLIMIT+(ntn[country].tsctrs/2)));
		refresh();
		getch();
		if(isgod==TRUE) country=0;
		return;
	}

	if(ntn[country].tgold <= 0){
		errormsg("you dont have any gold");
		if(isgod==TRUE) country=0;
		return;
	}

	/*raise an untrained army */
	mvprintw(LINES-3,0,"how many men do you wish to raise:");
	clrtoeol();
	refresh();
	men = get_number();
	if(men<=0) {
		if (isgod==TRUE) country=0;
		return;
	}
	/* i_people is initial people -> can  draft up to following */
	/*	draftable = max_draft - already drafted */
	/*		  = imen/4 - ( imen - people)   */
	/*		  = -3/4 * imen + people)   */
	if((men > sct[XREAL][YREAL].people - (3*sct[XREAL][YREAL].i_people/4))
	||(sct[XREAL][YREAL].i_people <= 0)) {
		if(sct[XREAL][YREAL].i_people <= 0) {
		errormsg("aborting: can not raise in this sector");
		}
		else errormsg("aborting: raising too many soldiers");
		if(isgod==TRUE) country=0;
		return;
	}
	/*ask what type of unit*/
	y=LINES-2;
	x=0;
	mvaddstr(y,x,"options:");
	clrtoeol();
	x+=9;
	for(i=0;i<=NOUNITTYPES;i++){
		if(unitvalid(i)==TRUE) {
			mvprintw(y,x+2,"%s",*(shunittype+i));
			mvprintw(y,x,"(%c)",*(shunittype+i)[0]);
			x+=7;
			if(x>COLS-10){
				x=0;
				y++;
			}
		}
	}
	move(y,x);
	clrtoeol();
	if((magic(country,WARRIOR)==TRUE)
	||(magic(country,WARLORD)==TRUE)
	||(magic(country,CAPTAIN)==TRUE))
	mvaddstr(LINES-3,0,"(Warrior = 1/2 enlist cost) what type of unit do you want:");
	else
	mvaddstr(LINES-3,0,"what type of unit do you want to raise:");

	clrtoeol();
	refresh();
	switch(getch()){
	case 'm':
		newtype=A_MILITIA;
		break;
	case 'I':
		newtype=A_INFANTRY;
		break;
	case 'A':
		newtype=A_ARCHER;
		break;
	case 'M':
		newtype=A_MERCENARY;
		break;
	case 'P':
		newtype=A_PHALANX;
		break;
	case 'L':
		newtype=A_LEGION;
		break;
	case 'E':
		newtype=A_ELITE;
		break;
	case 'D':
		newtype=A_DRAGOON;
		break;
	case 'C':
		newtype=A_CAVALRY;
		break;
	case 'K':
		newtype=A_KNIGHT;
		break;
	case 'c':
		newtype=A_CATAPULT;
		break;
	case 's':
		newtype=A_SEIGE;
		break;
	case 'R':
		newtype=A_ROC;
		break;
	case 'O':
		newtype=A_ORC;
		break;
	case 'U':
		newtype=A_URUK;
		break;
	case 'T':
		newtype=A_TROLL;
		break;
	case 'g':
		newtype=A_GRIFFON;
		break;
	case 'l':
		newtype=A_LT_CAV;
		break;
	case 'o':
		newtype=A_OLOG;
		break;
	case 'N':
		newtype=A_NINJA;
		break;
	case 'X':
		newtype=A_MARINES;
		break;
	case 'S':
		newtype=A_SAILOR;
		break;
	case 'G':
		newtype=A_GOBLIN;
		break;
	case 'e':
		newtype=A_ELEPHANT;
		break;
	default:
		errormsg("Invalid type");
		if (isgod==TRUE) country=0;
		return;
	}
	if(unitvalid(newtype)==FALSE) {
		errormsg("Invalid type");
		if (isgod==TRUE) country=0;
		return;
	}

	/* check that you dont have too many mercenaries */
	mercs=0;
	if(newtype == A_MERCENARY){
		for(armynum=0;armynum<MAXARM;armynum++){
			if(ATYPE==A_MERCENARY) mercs+=ASOLD;
		}
		if(men+mercs > (ntn[country].tmil-mercs)/2) {
			errormsg("aborting--you would then have more than 50%% mercenaries");
			if(isgod==TRUE) country=0;
			return;
		}
	}

	e_cost= (long) *(u_encost+newtype) * men;
	i_cost= (long) *(u_eniron+newtype) * men;

	/*magiced get 1/2 enlistment costs*/
	if((magic(country,WARRIOR)==TRUE)
	||(magic(country,WARLORD)==TRUE)
	||(magic(country,CAPTAIN)==TRUE))
		e_cost/=2;

	if((magic(country,SAPPER)==TRUE)
	&&((newtype==A_SEIGE)||(newtype==A_CATAPULT))){
		e_cost/=2;
		i_cost/=2;
	}

	/*check to see if enough gold*/
	if(e_cost >  ntn[country].tgold) {
		errormsg("aborting--you dont have enough gold in treasury");
		if(isgod==TRUE) country=0;
		return;
	}
	/*check for iron deposits*/
	else if(i_cost > ntn[country].tiron) {
		mvprintw(LINES-1,0,"aborting--you dont have %ld iron",i_cost);
		clrtoeol();
		refresh();
		getch();
		if(isgod==TRUE) country=0;
		return;
	}
	else {
		move(LINES-2,0);
		clrtoeol();
	}

	/*count is order of that army in sector*/
	/*armynum is number of that army*/
	if((armynum=getselunit())>=0){
		if(armynum>=MAXARM) {
			army = -1;
		} else {
			/*if different types, must raise new army*/
			if(newtype == ATYPE){
			mvaddstr(LINES-1,0,"Do you wish to raise a new army:");
			clrtoeol();
			refresh();
			if(getch()!='y') army=armynum;
			else army= -1;
			}
			else army=(-1);
		}
	}
	if(army==(-1)) {
		mvprintw(LINES-2,0,"(%s, gold=%ld, iron=%ld) raising a new army",*(unittype+newtype),e_cost,i_cost);
		clrtoeol();
		refresh();
		sleep(1);

		armynum=0;
		while((army==(-1))&&(armynum<MAXARM)) {
			if(ASOLD<=0) {
				army=armynum;
				ASOLD=0;
				ASTAT=DEFEND; /* set new armies to DEFEND */
				AADJSTAT;
				AADJMEN;
			}
			armynum++;
		}
		if(army==(-1)){
			errormsg("NO FREE ARMIES");
			if(isgod==TRUE) country=0;
			return;
		}
		armynum=army;
	} else {
		mvprintw(LINES-2,0,"(%s, gold=%ld, iron=%ld) adding to existing army",*(unittype+newtype),e_cost,i_cost);
		clrtoeol();
		refresh();
		sleep(1);
	}

	ATYPE=newtype;
	if(ATYPE != A_MERCENARY){
		sct[XREAL][YREAL].people -= men;
		SADJCIV;
	}
	AYLOC=YREAL;
	AXLOC=XREAL;
	AADJLOC;
	AMOVE=0;
	AADJMOV;
	ASOLD+=men;
	AADJMEN;
	ntn[country].tiron -= i_cost;
	ntn[country].tgold -= e_cost;
	makemap(); /* if display 'y' is set, this will show new army */
	if(isgod==TRUE) country=0;
}

/*adjust picked army*/
void
adjarm(armynum)
int armynum;
{
	short status;

	clear_bottom(0);
	if(armynum==(-1)) armynum=getselunit();
	mvprintw(LINES-4,0,"ADJUST ARMY %d",armynum);
	mvaddstr(LINES-3,0,"1) March  2) Scout  3) Attack  4) Defend  5) Garrison");
	mvaddstr(LINES-2,0,"Enter your choice (return to continue):");
	refresh();
	status = get_number();
	if((status<1)||(status>5)) return;
	change_status(armynum,status);
}

/*go through msgfile not rewriting to temp messages you discard*/
/* then move temp to msgfile*/
void
rmessage()
{
	int unlink();
	FILE *mesgfp;
	FILE *fptemp;
	int i;
	int count;
	int contd;
	int done=FALSE;
	char tempfile[ 30 ];
	char mesgfile[ 30 ];
	char line[80];
	char save[20][80];

	clear();
	/*open file*/
	sprintf(tempfile,"%s:temp",msgfile);
	if( (fptemp = fopen(tempfile,"w")) == NULL ) {
		mvprintw(0,0,"error on creating %s--hit return", tempfile);
		refresh();
		getch();
		return;
	}

	sprintf(mesgfile,"%s%d",msgfile,country);
	if ((mesgfp=fopen(mesgfile,"r"))==NULL) {
	        (void) unlink (tempfile) ;
		mvprintw(0,0,"no messages in %s, hit return",mesgfile);
		refresh();
		getch();
		return;
	}

	/*read in file a line at at time*/
	if(fgets(line,80,mesgfp)==NULL) done=TRUE;
	while(done==FALSE) {
		contd=0;
		count=3;
		if(strncmp(line,ntn[country].name,strlen(ntn[country].name))==0) {
			clear();
			standout();
			/*print to end of message*/
			while(contd==0) {
				if(count<22) strcpy(save[count-3],line);
				mvprintw(count,0,"%s",line);
				standend();
				for(i=0;i<strlen(ntn[country].name);i++)
					mvaddch(count,i,' ');
				count++;
				if(fgets(line,80,mesgfp)==NULL) contd=1;
				if(strncmp(line,"END",3)==0) contd=1;
			}
			standout();
			mvaddstr(LINES-3,(COLS/2)-22,"HIT RETURN TO SAVE MESSAGE");
			mvaddstr(LINES-2,(COLS/2)-22,"HIT ANY OTHER KEY TO CONTINUE");
			standend();
			refresh();
			if(getch()=='\n') {
				for(i=0;i<count-3;i++) fputs(save[i],fptemp);
				strcpy(line,"END\n");
				fputs(line,fptemp);
			}
		}
		else fputs(line,fptemp);
		if(fgets(line,80,mesgfp)==NULL) done=TRUE;
	}
	fclose(mesgfp);
	fclose(fptemp);

	/*IMPLEMENT A MOVE BETWEEN TMP FILE AND REAL FILE HERE*/
	move_file( tempfile, mesgfile );
}

void
wmessage()
{
	FILE *fp, *fopen();
	int x,y;
	int done=FALSE;
	char ch;
	char name[12];
	char realname[12];
	int temp=(-1);
	int linedone;
	char line[100];
	int nationid;

	/*what nation to send to*/
	clear();
	mvprintw(0,0,"to send a message to the administrator, send to 'god':");
	mvprintw(2,0,"What country name do you wish to send to:");
	refresh();
	get_nname(name);

	/*is name valid*/
	for(nationid=0;nationid<MAXNTN;nationid++) if(ntn[nationid].active!=0)
		if(strcmp(name,ntn[nationid].name)==0) temp=nationid;

	if(strcmp(name,"god")==0) {
		temp=0;
		strcpy(realname,"unowned");
	}
	else strcpy(realname,name);

	if (temp==(-1)) {
		mvprintw(3,0,"error--invalid name");
		refresh();
		getch();
		return;
	}

	sprintf(line,"%s%d",msgfile,temp);
	if ((fp=fopen(line,"a+"))==NULL) {
		mvprintw(4,0,"error opening %s",line);
		refresh();
		getch();
		return;
	}

	move(0,0);
	clrtoeol();
	move(2,0);
	clrtoeol();
	standout();
	mvprintw(3,(COLS-25)/2,"message to nation %s",name);
	mvprintw(LINES-2,(COLS-35)/2,"end with a return on a new line");
	standend();

	if (country==0)
		fprintf(fp,"%s Message to %s from GOD\n",realname,name);
	else fprintf(fp,"%s Message to %s from %s\n",realname,name,ntn[country].name);
	fprintf(fp,"%s \n",realname);
	y=6;
	x=0;
	refresh();
	while(done==FALSE) {
		linedone=FALSE;
		ch=' ';
		/*read line*/
		while(linedone==FALSE){
			/* check for delete or backspace */
			if(ch=='\b' || ch=='\177'){
				if(x>0) x--;
				mvaddch(y,x,' ');
				move(y,x);
				line[x]=' ';
				refresh();
				ch=getch();
			} else if(ch=='\n') linedone=TRUE;
			else if(isprint(ch)&&(x<65)){
				/*concatonate to end*/
				line[x]=ch;
				mvaddch(y,x,ch);
				x++;
				refresh();
				ch=getch();
			} else	ch=getch();
		}
		if (ch!='\n') {
			mvaddch(y,x,ch);
			line[x]=ch;
			x++;
		}
		line[x]='\0';
		if(x<=1) done=TRUE;
		/*write to file*/
		fprintf(fp,"%s %s\n",realname,line);
		x=0;
		y++;
	}
	fputs("END\n",fp);
	mvprintw(20,0,"Done with messaging");
	fclose(fp);
}

/*strategic move of civilians...once only*/
void
moveciv()
{
	short people;
	short i,j;

	mvaddstr(LINES-1,0,"Moving civilians costs 50 per civilian");
	if(sct[XREAL][YREAL].owner!=country){
		mvprintw(LINES-2,0,"you do not own:  hit return");
		clrtoeol();
		refresh();
		getch();
		return;
	}
	else if(sct[XREAL][YREAL].people==0){
		mvaddstr(LINES-2,0,"nobody lives here!!!:  hit return");
		clrtoeol();
		refresh();
		getch();
		makebottom();
		return;
	}

	clear();
	mvaddstr(0,0,"Moving civilians costs 50 per civilian");
	mvprintw(1,0,"sector contains %d people",sct[XREAL][YREAL].people);
	mvaddstr(2,0,"how many people to move?");
	clrtoeol();
	refresh();
	people = get_number();
	if((people<0)
	||(people>sct[XREAL][YREAL].people)
	||(people*50>ntn[country].tgold)){
		mvaddstr(5,0,"Sorry...Input error or you do not have the gold");
		clrtoeol();
		refresh();
		getch();
		makebottom();
		return;
	}

	mvprintw(5,0,"sector location is x=%d, y=%d",XREAL,YREAL);
	mvaddstr(7,0,"what x location to move to?");
	refresh();
	i = get_number();

	if((i-(XREAL))>2||(i-(XREAL))<-2) {
		mvprintw(10,0,"can only move 2 sectors (you tried %hd)...--hit any key",i-(XREAL));
		refresh();
		getch();
		return;
	}

	mvaddstr(10,0,"what y location to move to?");
	clrtoeol();
	refresh();
	j = get_number();
	if((j-(YREAL)>2)||((YREAL)-j>2)) {
		mvprintw(10,0,"sorry, can only move 2 sectors (you tried %hd)...--hit any key",j-(XREAL));
		refresh();
		getch();
	}
	else if(sct[i][j].owner!=country){
		mvaddstr(12,0,"sorry, you dont own it...");
		clrtoeol();
		refresh();
		getch();
	}
	/*need to check move cost > 0 for sector*/
	else if(movecost[i][j]<0){
		mvaddstr(12,0,"you can't enter there...");
		clrtoeol();
		refresh();
		getch();
	}
	else {
		ntn[country].tgold-=50*people;
		sct[XREAL][YREAL].people-=people;
		SADJCIV;
		sct[i][j].people+=people;
		SADJCIV2;
	}
}

int
armygoto()
{
	short armynum=0,loop=0;
	armynum=getselunit();
	if((armynum<0)||(armynum>MAXARM)) armynum=0;
	else armynum++;
	/* move to next army with > 0 soldiers*/
	while ((armynum < MAXARM) && (ASOLD <= 0)) armynum++;
	if(armynum >= MAXARM)  {
		armynum=0;
		if(ASOLD <= 0) return(0);
	}
	/*move to correct location*/
	xcurs = AXLOC - xoffset;
	ycurs = AYLOC - yoffset;
	coffmap();

	/*select correct unit*/
	selector=0;
	pager=0;
	while((getselunit() != armynum) && (loop++ < 500)) {
		selector+=2;
		if(selector>=10) {
			selector=0;
			pager+=1;
		}
	}
	return(1);
}

int
navygoto()
{
	short nvynum=0,loop=0;
	nvynum=getselunit()-MAXARM;
	if((nvynum<0)||(nvynum>MAXNAVY)) nvynum=0;
	else nvynum++;
	/* move to next nvy with > 0 soldiers*/
	while ((nvynum < MAXNAVY) && (NMER+NWAR <= 0)) nvynum++;
	if(nvynum >= MAXNAVY) {
		nvynum=0;
		if(NMER+NWAR <= 0) return(0);
	}
	/*move to correct location*/
	xcurs = NXLOC - xoffset;
	ycurs = NYLOC - yoffset;
	coffmap();

	/*select correct unit*/
	selector=0;
	pager=0;
	while(((getselunit()-MAXARM) != nvynum) && (loop++ < 500)) {
		selector+=2;
		if(selector>=10) {
			selector=0;
			pager+=1;
		}
	}
	return(1);
}
