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

/*EXECUTE THE PROGRAM*/
#include "header.h"
#include "data.h"
extern long startgold;
extern short country;
extern FILE *fexe;

int
execute()
{
	FILE *fp, *fopen();
	int cmd,savectry;
	char comment[20];
	char temp[10];
	long longvar,long2var;
	int armynum;
	short int x,y;
	int execed=0;
	int done=FALSE;
	char line[80];

	/* initialize startgold */
	startgold = ntn[country].tgold;

	/* initialize i_people */
	for(x=0;x<MAPX;x++)
		for(y=0;y<MAPX;y++)
			if(( sct[x][y].owner == country)&&
			((sct[x][y].designation == DCITY)
			||( sct[x][y].designation == DCAPITOL)))
				sct[x][y].i_people = sct[x][y].people;
			else
				sct[x][y].i_people = 0;

	/*execute in random order*/
	/*open exefile file*/
	sprintf(line,"%s%d",exefile,country);
	if ((fp=fopen(line,"r"))==NULL) {
		/*THIS MEANS THAT THE NATION HAS NOT MOVED YET*/
		return(0);
	}
	savectry=country;

	/*read in file*/
	if(fgets(line,80,fp)==NULL) done=TRUE;
	while(done==FALSE) {
		/*read and parse a new line*/
		/*CODE IF YOU USE LONG VAR IS L_*/
		if( line[0] == 'L' && line[1] == '_' ) {
			sscanf(line,"%s %d %hd %ld %ld %hd %s",
				temp,&cmd,&country,&longvar,&long2var,&y,comment);
		} else {
			sscanf(line,"%s %d %hd %d %hd %hd %s",
				temp,&cmd,&country,&armynum,&x,&y,comment);
		}
		execed=1;
		switch(cmd){
		case XASTAT:		/*Aadjstat*/
			if((x>0)&&(x<6))  ASTAT=x;
			break;
		case XAMEN:	/*Aadjmen*/
			armynum= (int) longvar;
			ASOLD= (int) long2var;
			ATYPE= y;
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
		case EDSPL:	/*Edecspl*/
			ntn[country].spellpts-=armynum;
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
			ntn[country].tgold = longvar;
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
			ntn[country].jewels = longvar;
			break;
		case XNAIRON:
			ntn[country].tiron = longvar;
			break;
		case INCAPLUS:
			ntn[country].aplus++;
			break;
		case INCDPLUS:
			ntn[country].dplus++;
			break;
		case DESTRY:
			sct[ntn[armynum].capx][ntn[armynum].capy].owner=savectry;
			country=armynum;
			destroy(country);
			country=savectry;
			break;
		case CHG_MGK:
			ntn[country].powers|=long2var;
			if(ntn[country].powers!=longvar){
			printf("\nERROR ON MAGIC READ %ld != %ld (or of %ld)",longvar,ntn[country].powers,long2var);
			getchar();
			}
			exenewmgk(long2var);
			long2var=0;
		}
		if(fgets(line,80,fp)==NULL) done=TRUE;
	}
	fclose(fp);
	/*return 1 if it did something*/
	country=savectry;
	if(execed==1) return(1);
	else return(0);
}
#ifdef CONQUER
void
hangup()
{
	char line[100];
	FILE *fp, *fopen();
	char name[12];
	char realname[12];
	int temp=0;

	if(country==0) writedata();
	else {
		fprintf(fexe,"L_NGOLD\t%d \t%d \t%ld \t0 \t0 \t%s\n",
		XNAGOLD ,country,ntn[country].tgold,"null");
		fprintf(fexe,"L_NIRON\t%d \t%d \t%ld \t0 \t0 \t%s\n",
		XNAIRON ,country,ntn[country].tiron,"null");
		fprintf(fexe,"L_NJWLS\t%d \t%d \t%ld \t0 \t0 \t%s\n",
		XNARGOLD ,country,ntn[country].jewels,"null");
	}
	/*close file*/
	fclose(fexe);
	/*send a message to God*/
	strcpy(name,"God");
	strcpy(realname,"unowned");
	sprintf(line,"%s%d",msgfile,temp);
	if((fp=fopen(line,"a+"))==NULL) {
		exit(FAIL);
	}
	fprintf(fp,"%s Message to %s from CONQUER\n",realname,name);
	fprintf(fp,"%s \n",realname);
	fprintf(fp,"%s      WARNING: Nation %s hungup on me.\n",realname,ntn[country].name);
	fprintf(fp,"%s                 I DEMAND RESPECT!!!!\n",realname);
	fputs("END\n",fp);

	fclose(fp);
	/* exit program */
	exit(FAIL);
}
#endif CONQUER
