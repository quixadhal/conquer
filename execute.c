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

/*EXECUTE THE PROGRAM*/
#include "header.h"
extern short country;

execute()
{
	FILE *fp, *fopen();
	int cmd;
	char comment[20];
	char temp[10];
	int armynum;
	short nation;
	short int x,y;
	int execed=0;
	int done=0;
	char line[80];

	/*execute in random order*/
	/*open EXEFILE file*/
	if ((fp=fopen(EXEFILE,"r"))==NULL) {
		printf("\nerror on read \n");
		return(0);
	}

	/*read in file*/
	if(fgets(line,80,fp)==NULL) done=1;
	while(done==0) {
		/*read and parse a new line*/
		sscanf(line,"%s %d %hd %d %hd %hd %s",temp,&cmd,&nation,&armynum,&x,&y,comment);
		if(country==nation) {
			execed=1;
			switch(cmd){
			case XASTAT:		/*Aadjstat*/
				if((x>0)&&(x<6))  ASTAT=x;
				break;
			case XAMEN:	/*Aadjmen*/
				ASOLD=x;
				break;
			case XALOC:	/*Aadjloc*/
				AXLOC=x;
				AYLOC=y;
				break;
			case XNLOC: /*nadjloc*/
				ntn[country].nvy[armynum].xloc=x;
				ntn[country].nvy[armynum].yloc=y;
				break;
			case XNASHP: /*nadjshp*/
				ntn[country].nvy[armynum].merchant=x;
				ntn[country].nvy[armynum].warships=y;
				break;
			case XECNAME:	/*Nadjname*/
				strcpy(ntn[country].name,comment);
				break;
			case XECPAS:	/*Nadjpas*/
				strncpy(ntn[country].passwd,comment,PASSLTH);
				break;
			case XECMARK:	/*Echgmark*/
				ntn[country].mark=comment[0];
				break;
			case XSADES:	/*Sadjdes*/
				if((sct[x][y].owner==country)||(country==0)) {
					sct[x][y].designation=comment[0];
				}
				if(sct[x][y].designation==DCAPITOL){
					ntn[country].capx=x;
					ntn[country].capy=y;
				}
				break;
			case XSACIV:	/*Sadjciv*/
				sct[x][y].people=armynum;
				break;
			case XSIFORT:	/*Sincfort*/
				sct[x][y].fortress++;
				break;
			case XNAGOLD:	/*Nadjgold:*/
				ntn[country].tgold = armynum;
				break;
			case XAMOV:
				AMOVE=x;
				break;
			case XNMOV:
				ntn[country].nvy[armynum].smove=x;
				break;
			case XSAOWN:
				sct[x][y].owner=country;
				break;
			case EDADJ:
				ntn[country].dstatus[armynum]=x;
				break;
			case XNARGOLD:
				ntn[country].jewels = armynum;
				break;
			case XNAIRON:
				ntn[country].tiron = armynum;
				break;
			case INCAPLUS:
				ntn[country].aplus++;
				break;
			case INCDPLUS:
				ntn[country].dplus++;
				break;
			case CHG_MGK:
				ntn[country].powers*=x;
				if(ntn[country].powers!=armynum)
					printf("ERROR ON MAGIC READ");
				exenewmgk(x);
			}
		}
		if(fgets(line,80,fp)==NULL) done=1;
	}
	fclose(fp);
	/*return 1 if it did something*/
	if(execed==1) return(1);
	else return(0);
}
