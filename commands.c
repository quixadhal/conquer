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

/*	commands.c		*/

/*include files*/
#include "header.h"
#include "data.h"
#include <ctype.h>

extern short xcurs,ycurs,xoffset,yoffset;
extern FILE *fexe;		/*execute file pointer*/
extern short country;
extern short selector;
extern short pager;
extern short xcurs,ycurs,xoffset,yoffset;

/*change current hex designation*/
redesignate()
{
	char newdes;
	short x,y;

	if(country==0){
		mvaddstr(LINES-1,0,"SUPER USER: CHANGE (v)egitation, (e)levation, (d)esig, (o)wner");
		refresh();
		switch(getch()){
		case 'e':
			/*simple contour map definitions*/
			mvprintw(LINES-2,7,"ELEVATIONS: change to %c, %c, %c, %c or %c?",WATER,PEAK,MOUNTAIN,HILL,CLEAR);
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
			mvprintw(LINES-2,7,"VEGITATIONS: change to %c, %c, %c, %c, %c, %c, %c, %c, %c, %c, %c or %c?",
			VOLCANO,DESERT,WASTE,BARREN,LT_VEG,
			GOOD,WOOD,FORREST,JUNGLE,SWAMP,ICE,NONE);
			refresh();
			newdes=getch();
			if(newdes!=VOLCANO
			&&newdes!=DESERT&&newdes!=WASTE
			&&newdes!=BARREN&&newdes!=LT_VEG
			&&newdes!=NONE&&newdes!=GOOD
			&&newdes!=WOOD&&newdes!=FORREST&&newdes!=JUNGLE
			&&newdes!=SWAMP&&newdes!=ICE) return;
			sct[XREAL][YREAL].vegetation=newdes;
			if(isdigit(sct[XREAL][YREAL].vegetation)==0) 
				sct[XREAL][YREAL].designation=newdes;
			return;
		case 'o':
			mvprintw(LINES-2,7,"what nation owner:");
			refresh();
			x = get_number();
			sct[XREAL][YREAL].owner=x;
			return;
		}
	}
	if((country!=0)&&(sct[XREAL][YREAL].owner!=country)) {
		mvprintw(LINES-1,0,"You do not own: hit any key");
		refresh();
		getch();
		return;
	}
	mvprintw(LINES-1,0,"hit space to not redesignate anything");
	clrtoeol();
	mvprintw(LINES-2,7,"$%d: redesignate to %c, %c, %c, %c or %c?",DESCOST,DMINE,DGOLDMINE,DFARM,DCITY,DCAPITOL);
	clrtoeol();
	refresh();
	/*read answer*/
	newdes=getch();

	if(newdes!=DMINE&&newdes!=DGOLDMINE&&newdes!=DFARM&&newdes!=DCITY&&newdes!=DCAPITOL) return;

	if((SOWN==country)||(country==0)) {
		if((isdigit(sct[XREAL][YREAL].vegetation)!=0)||(country==0)) {
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

				ntn[country].tgold-=5*DESCOST;
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
				mvprintw(LINES-1,0,"Sector must be a city: hit any key  ");
				refresh();
				getch();
			}
			else if((ntn[country].tiron>DESCOST)
			&&(newdes==DCITY)){
				if(sct[XREAL][YREAL].people>=500){
					ntn[country].tgold-=5*DESCOST;
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
				mvprintw(LINES-1,0,"Need 500 people to build to city");
				refresh();
				getch();
				}
			}
			else {
			mvprintw(LINES-1,0,"Not enough iron: hit any key  ");
			refresh();
			getch();
			}
		}
		else {
		mvprintw(LINES-1,0,"vegetation unlivable: hit any key  ");
		refresh();
		getch();
		}
	}
	else {
	mvprintw(LINES-1,0,"Sorry, you don't own sector: hit any key ");
	refresh();
	getch();
	}
}

/*build fort or ship-type */
construct()
{
	int nearsea;
	long cost;
	int armbonus;
	int x,y;
	short nvynum=0;
	short mnumber,wnumber;
	short isgod=0;
	char type;

	if(country==0){
		isgod=1;
		country=sct[XREAL][YREAL].owner;
	}

	if(sct[XREAL][YREAL].owner!=country) {
		mvaddstr(LINES-1,0,"You do not own: hit any key");
		refresh();
		getch();
		if(isgod==1) country=0;
		return;
	}

	if((isgod == 0) && (ntn[country].tgold < 0 )) {
		mvaddstr(LINES-1,0,"You are broke: hit any key");
		refresh();
		getch();
		return;
	}

	if((sct[XREAL][YREAL].designation==DCITY)
	||(sct[XREAL][YREAL].designation==DCAPITOL)) {
		/*calculate cost for fort*/
		cost=FORTCOST;
		if(isgod==1) cost=0;
		else for(x=1;x<=sct[XREAL][YREAL].fortress;x++) 
			cost*=2;

		if(sct[XREAL][YREAL].designation==DCITY) armbonus=8;
		else if(sct[XREAL][YREAL].designation==DCAPITOL) armbonus=10;
		if(magic(country,ARCHITECT)==1) armbonus*=2;

		move(LINES-1,0);
		clrtoeol();
		mvprintw(LINES-2,0,"Construct <f>ortifications (+%d%% - %ld gold) or <s>hips?:",armbonus,cost);
		clrtoeol();
		refresh();
		type=getch();
	}
	else {
		mvprintw(LINES-2,0,"Must construct in city or capitol--hit any key");
		clrtoeol();
		refresh();
		getch();
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
			mvprintw(LINES-2,0,"not in a harbor: hit space");
			clrtoeol();
			refresh();
			getch();
			if(isgod==1) country=0;
			return;
		}

		mvprintw(LINES-2,0,"build how many merchant ships:");
		clrtoeol();
		refresh();
		mnumber = get_number();
		mvprintw(LINES-2,0,"build how many warships:");
		clrtoeol();
		refresh();
		wnumber = get_number();
		/*sanity checks*/
		if((wnumber>100)||(wnumber<0)) wnumber=0;
		if((mnumber>100)||(mnumber<0)) mnumber=0;
		mvprintw(LINES-2,0,"constructing %hd warships and %hd merchant",wnumber,mnumber);
		clrtoeol();

		if((nvynum=getselunit()-MAXARM)>=0){
			if(nvynum>=MAXNAVY){
				mvaddstr(LINES-1,0,"INVALID NAVY--hit any key");
				clrtoeol();
				refresh();
				getch();
				if(isgod==1) country=0;
				return;
			}
			mvaddstr(LINES-1,0,"Do you wish to raise a new fleet (y or n)");
			clrtoeol();
			refresh();
			if(getch()=='y') nvynum=(-1);
		}

		if(nvynum<0) {
			mvprintw(LINES-2,0,"raising a new fleet");
			clrtoeol();
			refresh();

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

		if(nvynum==(-1)){
			mvaddstr(LINES-1,0,"NO FREE NAVIES--hit any key");
			clrtoeol();
			refresh();
			getch();
			if(isgod==1) country=0;
			return;
		}

		mvprintw(LINES-3,0,"Raising fleet %d",nvynum);
		if((nvynum>=0)&&(nvynum<MAXNAVY)) {
			NXLOC=XREAL;
			NYLOC=YREAL;
			NWAR+=wnumber;
			NMER+=mnumber;
			if(isgod==0) {
			ntn[country].tgold -= (long) wnumber*WARSHPCOST;
			ntn[country].tgold -= (long) mnumber*MERSHPCOST;
			}
			NMOVE=0;
			NADJSHP;
			NADJLOC;
			NADJMOV;
		}
		else mvprintw(LINES-2,0,"ERROR!!!!!!!!!!!!!");

		refresh();
		makebottom();
	}
	/* construct fortification points*/
	else if(type=='f'){
		if(sct[XREAL][YREAL].people>=500) {

			mvprintw(LINES-1,25,"you build %d%% fort points for %ld gold",armbonus,cost);
			ntn[country].tgold-=cost;
			sct[XREAL][YREAL].fortress++;
			INCFORT;
		}
		else mvprintw(LINES-1,25,"need 500 people");
	}
	else mvprintw(LINES-1,0,"error");

	if(isgod==1) country=0;
	refresh();
}

/*DRAFT IF IN A CITY*/
draft()
{
	short armynum;
	int men=0;
	short army=(-1);
	short isgod=0;
	if(country==0) {
		isgod=1;
		country=sct[XREAL][YREAL].owner;
	}
	else if(sct[XREAL][YREAL].owner!=country) {
		mvprintw(LINES-1,0,"You do not own: hit any key");
		refresh();
		getch();
		return;
	}

	if((sct[XREAL][YREAL].designation!=DCITY)
	&&(sct[XREAL][YREAL].designation!=DCAPITOL)) {
		mvprintw(LINES-1,0,"must raise in cities: hit any key");
		refresh();
		getch();
		if(isgod==1) country=0;
		return;
	}

	if((sct[XREAL][YREAL].designation==DCITY)
	&&(sct[XREAL][YREAL].people*(2*CITYLIMIT+(ntn[country].tsctrs/2))<ntn[country].tciv)){
		mvprintw(LINES-1,0,"need %d people in city: hit any key",ntn[country].tciv/(2*CITYLIMIT+(ntn[country].tsctrs/2)));
		refresh();
		getch();
		if(isgod==1) country=0;
		return;
	}

	if(ntn[country].tgold <= 0){
		mvprintw(LINES-1,0,"you dont have any gold: hit any key");
		refresh();
		getch();
		if(isgod==1) country=0;
		return;
	}

	/*raise an untrained army */
	mvprintw(LINES-3,0,"how many men do you wish to raise:");
	clrtoeol();
	refresh();
	men = get_number();
	if(men<=0) return;
	if(men > sct[XREAL][YREAL].people/4) {
		mvprintw(LINES-2,0,"can only raise %d soldiers",sct[XREAL][YREAL].people/4);
		clrtoeol();
		refresh();
		men = sct[XREAL][YREAL].people/4;
	}
	if(men >  (int) (ntn[country].tiron/10)) {
		mvprintw(LINES-2,0,"aborting--only enough iron for %d troops",ntn[country].tiron/10);
		clrtoeol();
		refresh();
		getch();
		if(isgod==1) country=0;
		return;
	}
	else {
		move(LINES-2,0);
		clrtoeol();
		ntn[country].tiron -= 10*men;
	}

	/*count is order of that army in sector*/
	/*armynum is number of that army*/
	if((armynum=getselunit())>=0){
		if(armynum>=MAXARM) {
			army = -1;
		} else {
			mvaddstr(LINES-1,0,"Do you wish to raise a new army:");
			clrtoeol();
			refresh();
			if(getch()!='y') army=armynum;
			else army= -1;
		}
	}
	if(army==(-1)) {
		mvprintw(LINES-2,0,"raising a new army");
		clrtoeol();
		refresh();
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
			mvaddstr(LINES-1,0,"NO FREE ARMIES--hit any key");
			clrtoeol();
			refresh();
			getch();
			if(isgod==1) country=0;
			return;
		}
		armynum=army;
	}

	AYLOC=YREAL;
	AXLOC=XREAL;
	AADJLOC;
	AMOVE=0;
	AADJMOV;
	ASOLD+=men;
	AADJMEN;
	sct[XREAL][YREAL].people -= men;
	SADJCIV;
	makemap(); /* if display 'y' is set, this will show new army */
	if(isgod==0)
		/*magiced get 1/2 enlistment costs*/
		if((magic(country,WARRIOR)==1)
		||(magic(country,WARLORD)==1)
		||(magic(country,CAPTAIN)==1))
			ntn[country].tgold -= (long) men*ENLISTCOST/2;
		else ntn[country].tgold -= (long) men*ENLISTCOST;
	else country=0;
}

/*adjust picked army*/
adjarm()
{
	short status;
	short armynum=0;
	armynum=getselunit();
	if((armynum<0)||(armynum>=MAXARM)) {
		beep();
		mvprintw(LINES-1,0,"Sorry you have an Invalid army number (%d)",armynum);
		refresh();
		getch();
		return;
	}
	if(ASTAT==SCOUT) {
		beep();
		mvprintw(LINES-1,0,"Sorry can't change scouts");
		refresh();
		getch();
		return;
	}
	clear();
	mvprintw(0,10,"ADJUST ARMY %d OF NATION %s",armynum,ntn[country].name);
	mvaddstr(2,10,"1.  March ");
	mvaddstr(3,10,"2.  Scouting--will not engage enemy if possible");
	mvaddstr(4,10,"3.  Attack anybody (Hostile+) within 2 sectors");
	mvaddstr(5,10,"4.  Defend ");
	mvaddstr(6,10,"5.  Garrison--for a city or Capital");
	mvaddstr(12,10,"Enter your choice (return to continue):");
	refresh();
	status = get_number();
	if((status<1)||(status>5)) return;
	if((status==SCOUT)&&(ASOLD>25)){
		clear();
		mvaddstr(12,(COLS/2)-6,"MUST HAVE < 25 MEN TO SCOUT");
		mvaddstr(13,(COLS/2)-12,"HIT ANY KEY TO CONTINUE");
		refresh();
		getch();
		return;
	}
	ASTAT=status;
	AADJSTAT;
}

/*go through msgfile not rewriting to temp messages you discard*/
/* then move temp to msgfile*/
rmessage()
{
	FILE *mesgfp;
	FILE *fptemp;
	int i;
	int count;
	int contd;
	int done=0;
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
		mvprintw(0,0,"no messages in %s, hit return",mesgfile);
		refresh();
		getch();
		return;
	}

	/*read in file a line at at time*/
	if(fgets(line,80,mesgfp)==NULL) done=1;
	while(done==0) {
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
		if(fgets(line,80,mesgfp)==NULL) done=1;
	}
	fclose(mesgfp);
	fclose(fptemp);

	/*IMPLEMENT A MOVE BETWEEN TMP FILE AND REAL FILE HERE*/
	move_file( tempfile, mesgfile );
}

wmessage()
{
	FILE *fp, *fopen();
	int x,y;
	int done=0;
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
	echo();
	getstr(name);
	noecho();

	/*is name valid*/
	for(nationid=0;nationid<MAXNTN;nationid++) if(ntn[nationid].active!=0)
		if(strcmp(name,ntn[nationid].name)==0) temp=nationid;

	if(strcmp(name,"god")==0) {
		temp=0;
		strcpy(realname,"unowned");
	}
	else strcpy(realname,name);

	if (temp==(-1)) {
		mvprintw(2,0,"error--invalid name");
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
	mvprintw(LINES-2,(COLS-25)/2,"end with a return on a new line");
	mvprintw(LINES-1,(COLS-35)/2,"btw... This is my first editor, any comments?");
	standend();

	fprintf(fp,"%s Message to %s from %s\n",realname,name,ntn[country].name);
	fprintf(fp,"%s \n",realname);
	y=6;
	x=0;
	refresh();
	while(done==0) {
		linedone=0;
		ch=' ';
		/*read line*/
		while(linedone==0){
			if(ch=='\b'){
				if(x>0) x--;
				mvaddch(y,x,' ');
				move(y,x);
				line[x]=' ';
				refresh();
				ch=getch();
			}
			else if(ch=='\n') linedone=1;
			else{
				/*concatonate to end*/
				line[x]=ch;
				mvaddch(y,x,ch);
				if(x<99) x++;
				else linedone=1;
				refresh();
				ch=getch();
			}
		}
		line[x]='\0';
		if(x<=1) done=1;
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
