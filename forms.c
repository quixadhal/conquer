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

/*	screen subroutines	*/

/*include files*/
#include <ctype.h>
#include "header.h"
#include "data.h"

extern FILE *fexe;
extern short country;
extern FILE *fnews;

void
showscore()
{
	int i;
	int done=FALSE;
	int position;
	int count; 	/*number of time through the loop on this screen*/
	int nationid; 	/*current nation id */

	nationid=1;
	while((done==FALSE)&&(nationid<MAXNTN)) {
		clear();
		standout();
		mvaddstr(0,(COLS/2)-10,"NATION SCORE SCREEN");
		standend();
		mvaddstr(3,0,"nationid is:");
		mvaddstr(4,0,"name is:");
		mvaddstr(5,0,"leader:");
		mvaddstr(6,0,"race:");
		mvaddstr(7,0,"class:");
		mvaddstr(8,0,"score:");
		mvaddstr(9,0,"gold:");
		mvaddstr(10,0,"military:");
		mvaddstr(11,0,"civilians:");
		mvaddstr(12,0,"sectors:");

		count=0;
		position=2;
		while((nationid<MAXNTN)&&(count<5)){
			if((nationid<MAXNTN)&&(ntn[nationid].active!=0)) {
				position+=13;
				mvprintw(3,position,"%d",nationid);
				standout();
				mvprintw(4,position,"%s",ntn[nationid].name);
				standend();
				mvprintw(5,position,"%s",ntn[nationid].leader);
				for(i=1;i<8;i++)
					if(ntn[nationid].race==*(races+i)[0])
						mvprintw(6,position,"%s",*(races+i));
				if(ntn[nationid].active>=2) mvprintw(7,position,"NPC");
				else mvprintw(7,position,"%s",*(Class+ntn[nationid].class));
				mvprintw(8,position,"%ld",ntn[nationid].score);
				mvprintw(9,position,"%ld",ntn[nationid].tgold);
				mvprintw(10,position,"%ld",ntn[nationid].tmil);
				mvprintw(11,position,"%ld",ntn[nationid].tciv);
				mvprintw(12,position,"%d",ntn[nationid].tsctrs);
				count++;
			}
			nationid++;
		}
		standout();
		mvaddstr(18,(COLS/2)-12,"HIT ANY KEY TO CONTINUE");
		mvaddstr(19,(COLS/2)-9,"HIT SPACE IF DONE");
		standend();
		refresh();
		if (getch()==' ') done=TRUE;
	}
}

void
diploscrn()
{
	int i;
	char k;
	char tempifile[30];
	short nation, offset, count, olddip, oldnat, temp;
	FILE *fp, *fopen();
	short isgod=FALSE;
	if(country==0) {
		isgod=TRUE;
		clear();
		mvaddstr(0,0,"WHAT NATION NUMBER:");
		refresh();
		country = get_number();
	}
	while(1){
		count=1;
		offset=0;
		clear();
		standout();
		mvaddstr(0,(COLS/2)-10,"NATION DIPLOMACY SUMMARY");
		standend();
		mvaddstr(2,0,"                BY YOU,        TO YOU");
		for(i=1;i<NTOTAL;i++) if((ntn[i].active>0)&&(i!=country)) {
			if(count%14==0) {
				offset+=40;
				mvaddstr(2,40,"                BY YOU,        TO YOU");
				count++;
			}
			mvprintw(count%14+2,offset, "%d. %s",i,ntn[i].name);
			if((ntn[country].dstatus[i]==WAR)
			||(ntn[country].dstatus[i]==JIHAD)) standout();
			mvprintw(count%14+2,offset+14, "=> %s",*(diploname+ntn[country].dstatus[i]));
			standend();
			if((ntn[i].dstatus[country]==WAR)
			||(ntn[i].dstatus[country]==JIHAD)) standout();
			mvprintw(count%14+2,offset+28, "=> %s",*(diploname+ntn[i].dstatus[country]));
			standend();
			count++;
		}
		standout();
		mvaddstr(17,0,"HIT RETURN KEY TO CHANGE STATUS");
		mvprintw(18,0,"HIT 'B' KEY TO BRIBE WARRING NPC NATION (%ld GOLD/level)",BRIBE);
		mvaddstr(19,0,"ANY OTHER KEY TO CONTINUE:");
		standend();
		refresh();
		k=getch();
		if((k!='\n')&&(k!='B')) {
			if(isgod==TRUE) country=0;
			return;
		}
		if(k=='B'){
			if(ntn[country].tgold<=BRIBE){
				mvaddstr(21,0,"NOT ENOUGH GOLD");
				refresh();
				getch();
				return;
			}
			mvaddstr(20,0,"BRIBES WORK 50% (only the update will show)");
			mvaddstr(21,0,"WHAT NATION NUMBER:");
			refresh();
			nation = get_number();
			if(ntn[nation].active<2){
				mvaddstr(22,0,"NOT NON PLAYER COUNTRY");
				refresh();
				getch();
				return;
			}
			/*use maxntn here as cant change with nomads...*/
			if((nation<=0)||(nation>MAXNTN)){
				if(isgod==TRUE) country=0;
				return;
			}
			if((isgod!=1)
			    &&((ntn[nation].dstatus[country]==ALLIED)
			    ||(ntn[nation].dstatus[country]==JIHAD)
			    ||(ntn[nation].dstatus[country]==CONFEDERACY))){
				mvaddstr(22,0,"CANT CHANGE THEIR STATUS");
				refresh();
				getch();
				return;
			}
			ntn[nation].dstatus[country]--;
			ntn[country].tgold-=BRIBE;
			ntn[nation].tgold+=BRIBE;
			EADJDIP;
			oldnat=country;
			country=nation;
			nation=oldnat;
			fprintf(fexe,"L_NGOLD\t%hd \t%d \t%ld \t0 \t0 \t%s\n",XNAGOLD ,country,ntn[country].tgold,"null");
			EADJDIP;
			country=oldnat;
			return;
		}
		mvaddstr(21,0,"WHAT NATION NUMBER:");
		refresh();
		nation = get_number();
		/*use maxntn here as cant change with nomads...*/
		if((nation<=0)||(nation>MAXNTN)){
			if(isgod==TRUE) country=0;
			return;
		}
		if((isgod==FALSE)
		    &&(((ntn[country].dstatus[nation]==CONFEDERACY)
		    &&(ntn[nation].dstatus[country]<WAR))
		    ||(ntn[country].dstatus[nation]==JIHAD))) {
			mvprintw(23,0,"SORRY, Can't change status on ntn %s -- hit return",ntn[nation].name);
			refresh();
			getch();
			return;
		}
		else{
			clear();
			mvaddstr(0,0,"WHAT NEW STATUS");
			mvaddstr(2,0,"1) CONFEDERACY (irrevocable)");
			mvaddstr(3,0,"2) ALLIED");
			mvaddstr(4,0,"3) FRIENDLY");
			mvaddstr(5,0,"4) NEUTRAL");
			mvaddstr(6,0,"5) HOSTILE");
			mvaddstr(7,0,"6) WAR");
			mvaddstr(8,0,"7) JIHAD (irrevocable):");
			mvaddstr(12,0,"INPUT:");
			refresh();
			temp = get_number();
			if((temp<1)||(temp>7)){
				mvprintw(23,0,"SORRY, Invalid inputs -- hit return");
				refresh();
				getch();
				return;
			}
			ntn[country].dstatus[nation]=temp;
			EADJDIP;

			/*prevent ron from being sneaky*/
			if((temp>HOSTILE)&&(ntn[nation].active>2)
			&&(ntn[nation].dstatus[country]<WAR)) {
				olddip=ntn[nation].dstatus[country];
				oldnat=country;
				country=nation;
				nation=oldnat;
				ntn[country].dstatus[nation]=WAR;
				EADJDIP;
				ntn[country].dstatus[nation]=olddip;
				oldnat=country;
				country=nation;
				nation=oldnat;
			}
			else if((temp>HOSTILE)&&(ntn[nation].active==1)
			&&(ntn[nation].dstatus[country]<WAR)) {
	 			sprintf(tempifile,"%s%d",msgfile,nation);
				if ((fp=fopen(tempifile,"a+"))==NULL) {
					mvprintw(4,0,"error opening %s",tempifile);
					refresh();
					if (isgod==TRUE) country=0;
					getch();
					return;
				}
				fprintf(fp,"%s Message to %s from CONQUER\n",ntn[nation].name,ntn[nation].name);
				fprintf(fp,"%s \n",ntn[nation].name);
				fprintf(fp,"%s \n",ntn[nation].name);
				fprintf(fp,"%s       %s has declared war on you\n",ntn[nation].name,ntn[country].name);
				fputs("END\n",fp);
				fclose(fp);
			}
		}
	}
}

void
change()
{
	char string[10], command[80];
	int i, cbonus;
	short armynum;
	char passwd[8];
	short isgod=FALSE;

	if(country==0) {
		isgod=TRUE;
		clear();
		mvaddstr(0,0,"SUPER USER; FOR WHAT NATION NUMBER:");
		refresh();
		country = get_number();
	}
	clear();
	mvaddstr(0,(COLS/2)-10,"NATION STATS SUMMARY");
	mvprintw(5,0,"1. nation name is %s   ",ntn[country].name);
	mvprintw(6,0,"2. password is XXXXXXXX");
	if (isgod==TRUE)
		mvprintw(7,0,"active is %d",ntn[country].active);
	mvprintw(11,0,"capitol loc: x is %d",ntn[country].capx);
	mvprintw(12,0,"             y is %d",ntn[country].capy);
	mvprintw(13,0,"leader is %s",ntn[country].leader);
	mvprintw(14,0,"class is %s",*(Class+ntn[country].class));
	mvprintw(15,0,"nations mark is...%c ",ntn[country].mark);
	for(i=1;i<8;i++) if(ntn[country].race==*(races+i)[0])
		mvprintw(16,0, "nation race is....%s  ",*(races+i));
	mvprintw(17,0,"score currently...%ld",ntn[country].score);

	mvprintw(4,(COLS/2), "attack bonus...........+%2d",ntn[country].aplus);
	mvprintw(5,(COLS/2), "defense bonus..........+%2d",ntn[country].dplus);
	mvprintw(6,(COLS/2), "maximum move rate.......%2d",ntn[country].maxmove);
	mvprintw(7,(COLS/2), "reproduction rate......%2d%%",ntn[country].repro);
	mvprintw(9,(COLS/2), "gold talons......$%8ld",ntn[country].tgold);
	mvprintw(10,(COLS/2),"jewels ..........$%8ld",ntn[country].jewels);
	mvprintw(11,(COLS/2),"iron & minerals...%8ld",ntn[country].tiron);
	if(ntn[country].tfood<2*ntn[country].tciv) standout();
	mvprintw(12,(COLS/2),"food in granary...%8ld",ntn[country].tfood);
	standend();

	mvprintw(13,(COLS/2),"total soldiers....%8ld",ntn[country].tmil);
	mvprintw(14,(COLS/2),"total civilians...%8ld",ntn[country].tciv);
	mvprintw(15,(COLS/2),"total ships..........%5d",ntn[country].tships);
	mvprintw(16,(COLS/2),"total sectors........%5d",ntn[country].tsctrs);
	mvprintw(17,(COLS/2),"spell points.........%5d",ntn[country].spellpts);

	standout();
	mvaddstr(19,(COLS/2)-9, "HIT ANY KEY TO CONTINUE");
	mvaddstr(20,(COLS/2)-15,"HIT 1 or 2 TO CHANGE NAME or PASSWD");
	if(magic(country,VAMPIRE)!=1)
		mvaddstr(21,(COLS/2)-14,"HIT 3 TO ADD TO YOUR COMBAT BONUS");
	else	mvaddstr(21,(COLS/2)-17,"( VAMPIRES MAY NOT ADD TO COMBAT BONUS )");

#ifdef OGOD
 	if(isgod==TRUE) mvaddstr(22,(COLS/2)-21,"HIT 4 TO DESTROY NATION OR 5 TO CHANGE TREASURY");
#else OGOD
  	if(isgod==TRUE) mvaddstr(22,(COLS/2)-9,"HIT 4 TO DESTROY NATION");
#endif OGOD
	standend();
	refresh();
	switch(getch()){
	case '1': /*get name*/
		clear();
		mvaddstr(0,0,"what name would you like:");
		clrtoeol();
		refresh();
		get_nname(string);
		if((strlen(string)<=1)||(strlen(string)>NAMELTH)){
			beep();
			mvaddstr(2,0,"invalid name--hit return");
			refresh();
			getch();
			if(isgod==TRUE) country=0;
			return;
		}
		/*check if already used*/
		else for(i=1;i<(country-1);i++){
			if((strcmp(ntn[i].name,string)==0)&&(i!=country)) {
				mvaddstr(2,0,"name already used--hit return");
				beep();
				refresh();
				getch();
				if(isgod==TRUE) country=0;
				return;
			}
		}
		mvaddstr(2,0,"new name can be used following next update--hit return");
		refresh();
		getch();
		strcpy(ntn[country].name,string);
		ECHGNAME;
		break;
	case '2': /*change password */
		clear();
		if(isgod!=1){
			mvaddstr(0,0,"what is your current password:");
			refresh();
			getstr(string);
			strcpy(passwd,crypt(string,SALT));
			if((strncmp(passwd,ntn[0].passwd,PASSLTH)!=0)
			    &&(strncmp(passwd,ntn[country].passwd,PASSLTH)!=0)){
				beep();
				mvaddstr(2,0,"invalid password--hit return");
				refresh();
				getch();
				if(isgod==TRUE) country=0;
				return;
			}
		}
		mvaddstr(2,0,"what is your new password:");
		refresh();
		getstr(string);
		if((strlen(string)>=8)||(strlen(string)<2)) {
			beep();
			mvaddstr(2,0,"invalid new password--hit return");
			refresh();
			getch();
			if(isgod==TRUE) country=0;
			return;
		}
		mvaddstr(4,0,"reenter your new password:");
		refresh();
		getstr(passwd);
		if(strcmp(passwd,string)!=0) {
			if(isgod==TRUE) country=0;
			return;
		}
		mvaddstr(6,0,"new password can be used following next update--hit return");
		refresh();
		getch();
		ECHGPAS;
		strncpy(ntn[country].passwd,crypt(string,SALT),PASSLTH);
		break;
	case '3':
		if(magic(country,VAMPIRE)==1) {
			mvaddstr(0,0,"VAMPIRES CAN'T ADD TO COMBAT BONUS (hit return)");
			refresh();
			getch();
			break;
		}
		if(magic(country,WARLORD)==1)      cbonus=30;
		else if(magic(country,CAPTAIN)==1) cbonus=20;
		else if(magic(country,WARRIOR)==1) cbonus=10;
		else cbonus=0;
		i=0;
		for(armynum=0;armynum<MAXARM;armynum++) if(ASOLD>0) i+=ASOLD;
		i = max ( i, 1500);
		armynum = max ( ntn[country].aplus-cbonus, 10 ) / 10;
		mvprintw(0,0,"DO YOU WISH SPEND %d IRON ON ATTACK (enter y or n):",IRONORE*i*armynum*armynum);
		refresh();
		if(getch()=='y'){
			if(ntn[country].tiron> (long)IRONORE*i*armynum*armynum){
				ntn[country].aplus+=1;
				I_APLUS;
				ntn[country].tiron-= (long) IRONORE*i*armynum*armynum;
			}
			else {
				mvaddstr(0,0,"SORRY - hit any key");
				clrtoeol();
				refresh();
				getch();
			}
		}
		i=0;
		for(armynum=0;armynum<MAXARM;armynum++) if(ASOLD>0) i+=ASOLD;
		i = max ( i, 1500);
		armynum = max ( ntn[country].dplus-cbonus, 10 ) / 10;
		mvprintw(0,0,"DO YOU WISH SPEND %d IRON ON DEFENSE (enter y or n):",IRONORE*i*armynum*armynum);
		refresh();
		if(getch()=='y'){
			if(ntn[country].tiron>IRONORE*i*armynum*armynum){
				ntn[country].dplus+=1;
				I_DPLUS;
				ntn[country].tiron-= (long) IRONORE*i*armynum*armynum;
			}
			else {
				mvaddstr(0,0,"SORRY - hit any key");
				clrtoeol();
				refresh();
				getch();
			}
		}
		break;
	case '4':
		if(isgod==TRUE){
			clear();
			mvaddstr(0,0,"DO YOU WANT TO DESTROY THIS NATION (y or n)");
			refresh();

			if(getch()=='y') {
				if ((fnews=fopen(newsfile,"a+"))==NULL) {
					printf("error opening news file\n");
					exit(FAIL);
				}
				destroy(country);
				fclose(fnews);
				sprintf(command,"sort -n -o %s %s ",
					newsfile, newsfile);
				system(command);
			}
		}
		break;
#ifdef OGOD
	case '5':
		if (isgod==TRUE) {
			/* adjust treasury */
			mvaddstr(0,0,"WHAT IS NEW TOTAL OF TREASURY?");
			refresh();
			i = get_number();
			/* as god it will be saved nothing else needed */
			ntn[country].tgold = (long) i;
		}
		break;
#endif OGOD
	default:
		if(isgod==TRUE) country=0;
		return;
	}
	if(isgod==TRUE) country=0;
	change();
}

void
help()
{
	int lineno;
	FILE *fp, *fopen();
	int done=FALSE;
	char line[80];

	/*open .help file*/
	if ((fp=fopen(helpfile,"r"))==NULL) {
		mvprintw(0,0,"\nerror on read of %s\n",helpfile);
		refresh();
		getch();
		return;
	}

	while(done==FALSE){
		/*read in screen (until DONE statement)*/
		fgets(line,80,fp);
		if(strncmp(line,"DONE",4)==0) done=TRUE;
		else {
			clear();
			lineno=0;
			while(strncmp(line,"END",3)!=0) {
				mvaddstr(lineno,0,line);
				lineno++;
				if(lineno>LINES-3) strcpy(line,"END");
				else fgets(line,80,fp);
			}
			standout();
			mvaddstr(LINES-2,(COLS/2)-24,"HIT ANY KEY TO CONTINUE HELP SCREENS");
			mvaddstr(LINES-1,(COLS/2)-19,"TO END HELP HIT SPACE KEY");
			standend();
			refresh();
			if(getch()==' ') done=TRUE;
		}
	}
	fclose(fp);
}

void
newspaper()
{
	int lineno;
	FILE *fp, *fopen();
	int newpage,done;
	short pagenum=1;
	char line[80];
	char title[80];

	clear();
	if ((fp=fopen(newsfile,"r"))==NULL) {
		mvprintw(0,0,"error on read of %s",newsfile);
		refresh();
		getch();
		return;
	}

	/*open and read one page */
	done=FALSE;
	newpage=FALSE;
	if(fgets(title,80,fp)==NULL) done=TRUE;
	while(done==FALSE){
		if(newpage==FALSE){
			clear();
			lineno=5;
			newpage=TRUE;
			standout();
			mvprintw(0,20,"CONQUER NEWS REPORT   page %d",pagenum);
			mvaddstr(1,23,"ALL THE NEWS THAT FITS");
			mvprintw(3,20,"%s",title+2);
			standend();
		}

		if(fgets(line,80,fp)==NULL) done=TRUE;
		else {
			if(line[1]!='.') {
				strcpy(title,line);
				newpage=FALSE;
				pagenum++;
			}
			else {
				mvaddstr(lineno++,0,line+2);
				if(todigit(line[0])!=pagenum) {
					newpage=FALSE;
					pagenum=todigit(line[0]);
				}
				else if(lineno>LINES-3) newpage=FALSE;
			}
		}

		if(newpage==FALSE||done==TRUE){
			standout();
			mvaddstr(LINES-2,(COLS/2)-13,"HIT ANY KEY TO CONTINUE");
			mvaddstr(LINES-1,(COLS/2)-12,"TO END NEWS HIT SPACE");
			standend();
			refresh();
			if(getch()==' ') done=TRUE;
		}
	}
	fclose(fp);
}
