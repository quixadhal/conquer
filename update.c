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

#include "header.h"
#include <ctype.h>

FILE *fnews, *fopen();

extern short country;
int attr[MAPX][MAPY];     /*sector attactiveness*/
extern short occ[MAPX][MAPY];
extern short movecost[MAPX][MAPY];

/*update nation file*/
update()
{
	register int x,y;
	int moved,armynum,nvynum,done,finis=0,j,i,number=0;
	int food,iron;
	char command[80];
	int execed[MAXNTN];

	if ((fnews=fopen(NEWSFILE,"w"))==NULL) {
		printf("error opening news file\n");
		exit(1);
	}

	/*run each nation in a random order*/
	country=0;
	execute();
	for(i=0;i<MAXNTN;i++) execed[i]=0;
	system("/bin/date");
	while(finis==0){
		/*get random active nation*/
		country=(rand()%(MAXNTN-1))+1;
		if(ntn[country].active>0) {
			done=0;
			number=0;
			/*Find the next unupdated nation*/
			while(done==0){
				if((ntn[country].active>0)&&(execed[country]==0)) {
					done=1;
					execed[country]=1;
				}
				else {
					country++;
					number++;
					if(number>MAXNTN) {
						finis=1;
						done=1;
					}
					else if(country>=MAXNTN) country=1;
				}
			}

			if(finis==0){
				printf("updating nation number %d -> %s\n",country,ntn[country].name);

				/*if execute is 0 and PC nation then they did not move*/
				if((execute()==0)&&(ntn[country].active==1)){
					printf("nation %s did not move\n",ntn[country].name);
#ifdef CMOVE
					printf("the computer will move for %s\n",ntn[country].name);
					fprintf(fnews,"1.\tthe computer will move for %s\n",ntn[country].name);
					nationrun();
#endif
				}
#ifdef NPC
				/*run npc nations*/
				if(ntn[country].active>=2) nationrun();
#endif
				/*do magic*/
				if(magic(country,MI_MONST)==1){
					if(magic(country,AV_MONST)==1) {
						if(magic(country,MA_MONST)==1)
							takeover(5,0);
						else takeover(3,0);
					}
					else takeover(1,0);
				}

				/*update movement array*/
				updmove(ntn[country].race);

				/*THIS IS WHERE YOU ZERO THE ATTR MATRIX*/
				/*calculate sector attractiveness*/
				for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) {
					if((sct[x][y].owner==country)&&(isdigit(sct[x][y].vegitation)!=0)){
						attr[x][y]=attract(x,y,ntn[country].race);
					}
					else if(((magic(country,DERVISH)==1)||(magic(country,DESTROYER)==1))&&((sct[x][y].vegitation==ICE)||(sct[x][y].vegitation==DESERT))) {
						attr[x][y]=36;
					}
					else attr[x][y]=0;
				}

				/*if near capital add to attr*/
				for(x=ntn[country].capx-2;x<=ntn[country].capx+2;x++)
					for(y=ntn[country].capy-2;y<=ntn[country].capy+2;y++)
						if(attr[x][y]>0) attr[x][y]+=20;

/*MOVE CIVILIANS based on the ratio of attractivenesses
	 *
	 * EQUILIBRIUM(1) = A1/(A1+A2) * (P1+P2)
	 * EQUILIBRIUM(2) = A2/(A1+A2) * (P1+P2)
	 * MOVE 1/5 of way to equilibrium each turn
	 * DELTA(1) = (EQUILIBRIUM(1)-P1)/5 = (A1P2-P1A2)/5(A1+A2)
	 * DELTA(2) = (EQUILIBRIUM(2)-P2)/5 = (A2P1-P2A1)/5(A1+A2) = -DELTA(1)
	 * ij is refered to as 1, xy as 2
	 * NOTE AM ADDING 1 to divisor to prevent floating exception errors
	 */
				for(x=0;x<MAPX-1;x++) for(y=0;y<MAPY-1;y++) for(i=(-2);i<=2;i++) for(j=(-2);j<=2;j++)
				if((x+i>0)&&(x+i<MAPX)&&(x+j>0)&&(x+j<MAPY)&&(sct[x+i][y+j].owner==country)&&(sct[x][y].owner==country)) {
					moved=(sct[x][y].people*attr[x+i][y+j]-sct[x+i][y+j].people*attr[x][y])/(1+5*(attr[x+i][y+j]+attr[x][y]));
					sct[x+i][y+j].people += moved;
					sct[x][y].people -= moved;
				}
			}
		}
	}

	/* run npc nations */
#ifdef LZARD
	puts("updating lizards\n ");
	country = NLIZARD;
	armynum=0;
	/*move to lizard castle*/
	for(armynum=0;armynum<MAXARM;armynum++) if(ASTAT!=GARRISON){
		for(i=AXLOC-1;i<=AXLOC+1;i++) for(j=AYLOC-1;j<=AYLOC+1;j++){
			if((i>=0)&&(j>=0)&&(i<MAPX)&&(j<MAPX)&&(sct[i][j].designation==DCASTLE)){
				AXLOC=i;
				AYLOC=j;
			}
		}
		for(i=AXLOC-1;i<=AXLOC+1;i++) for(j=AYLOC-1;j<=AYLOC+1;j++){
			if((i>=0)&&(j>=0)&&(i<MAPX)&&(j<MAPX)&&(sct[i][j].altitude!=WATER)&&(sct[i][j].owner != NLIZARD)&&(rand()%2==0)){
				AXLOC=i;
				AYLOC=j;
			}
		}
	}
#endif

#ifdef MONSTER
	monster();
#endif

	/*run combat*/
	combat();

	fprintf(fnews,"3\tNEWS ON WHAT SECTORS HAVE BEEN CAPTURED");
	/*look for any areas where armies alone in sector*/
	prep();
	for(country=1;country<NTOTAL;country++) if(ntn[country].active!=0){
		for(armynum=0;armynum<MAXARM;armynum++) if(ASOLD>75){
			if(sct[AXLOC][AYLOC].owner==0){
				sct[AXLOC][AYLOC].owner=country;
			}
			else if((sct[AXLOC][AYLOC].owner!=country)&&(ntn[country].dstatus[sct[AXLOC][AYLOC].owner]>=WAR)&&(occ[AXLOC][AYLOC]==country)){

				if((sct[AXLOC][AYLOC].owner!=0)&&(ntn[sct[AXLOC][AYLOC].owner].race!=ntn[country].race)) flee(AXLOC,AYLOC,1);
				fprintf(fnews,"\n3.\tarea %d,%d captured by %s from %s",AXLOC,AYLOC,ntn[country].name,ntn[sct[AXLOC][AYLOC].owner].name);
				sct[AXLOC][AYLOC].owner=country;
			}
		}
	}
	fprintf(fnews,"\n1\tIMPORTANT WORLD NEWS\n");

	for(country=1;country<MAXNTN;country++) 
		if((ntn[country].active>=2)&&((ntn[country].tciv==0)||(sct[ntn[country].capx][ntn[country].capy].owner!=country))) destroy();

	/*zero out all recalculated values*/
	for(i=0;i<MAXNTN;i++){
		ntn[i].tsctrs=0;
		ntn[i].tships=0;
		ntn[i].tciv=0;
		ntn[i].tmil=0;
	}

	/*for whole map, update one sector at a time, owned sectors only*/
	printf("\nupdating all sectors\n");
	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) if(sct[x][y].owner!=0) {

		/*check all adjacent sectors and decide if met*/
		for(i=x-1;i<=x+1;i++) for(j=y-2;j<=y+2;j++) {
			if(i>=0&&i<MAPX&&j>=0&&j<MAPY&&(sct[i][j].owner!=0)) {
				if(sct[x][y].owner!=sct[i][j].owner) {
					if(ntn[sct[x][y].owner].dstatus[sct[i][j].owner]==UNMET) newdip(sct[x][y].owner,sct[i][j].owner);
					if(ntn[sct[i][j].owner].dstatus[sct[x][y].owner]==UNMET) newdip(sct[i][j].owner,sct[x][y].owner);
				}
			}
		}

		/*update nation file for owner*/
		ntn[sct[x][y].owner].tsctrs++;
		ntn[sct[x][y].owner].tciv += sct[x][y].people;

		/*grow populations*/
		if(sct[x][y].people<100) sct[x][y].people+=sct[x][y].people/10;
		else sct[x][y].people+=(ntn[sct[x][y].owner].repro*sct[x][y].people)/100;

		/*PRODUCE*/
		/*increase tmin based on mined stuff...*/
		if(sct[x][y].designation==DMINE) {
			iron=sct[x][y].iron*sct[x][y].people;
			if(magic(sct[x][y].owner,MINER)==1) iron*=2;
			if(magic(sct[x][y].owner,STEEL)==1) iron*=2;
			ntn[sct[x][y].owner].tiron += iron;
			ntn[sct[x][y].owner].tgold += iron*TAXIRON/100;
		}
		/*harvest food*/
		else if(sct[x][y].designation==DFARM) {
			food=todigit(sct[x][y].vegitation)*sct[x][y].people;
			ntn[sct[x][y].owner].tfood += food;
			ntn[sct[x][y].owner].tgold += food*TAXFOOD/100;
		}
		/*gold mines produce gold*/
		else if(sct[x][y].designation==DGOLDMINE) {
			if(magic(sct[x][y].owner,MINER)==1){
				ntn[sct[x][y].owner].tgold+=  2*sct[x][y].gold * sct[x][y].people * TAXGOLD/100;
				ntn[sct[x][y].owner].jewels+= 2*sct[x][y].gold * sct[x][y].people;
			}
			else {
				ntn[sct[x][y].owner].tgold+= sct[x][y].gold * sct[x][y].people * TAXGOLD/100;
				ntn[sct[x][y].owner].jewels+= sct[x][y].gold * sct[x][y].people;
			}
		}
		else if(sct[x][y].designation==DCAPITOL) {
			if((x!=ntn[sct[x][y].owner].capx)&&(y!=ntn[sct[x][y].owner].capy)) {
				ntn[sct[x][y].owner].tgold+= 2*sct[x][y].people * TAXCITY/100;
				sct[x][y].designation=DCITY;
				if(magic(sct[x][y].owner,ARCHITECT)==1){
					ntn[sct[x][y].owner].tgold+= 2 * sct[x][y].people * TAXCITY/100;
				}
				else {
					ntn[sct[x][y].owner].tgold+= sct[x][y].people * TAXCITY/100;
				}
			}
			else if(magic(sct[x][y].owner,ARCHITECT)==1){
				ntn[sct[x][y].owner].tgold+= 2 * sct[x][y].people * TAXCAP/100;
			}
			else 
				ntn[sct[x][y].owner].tgold+= sct[x][y].people * TAXCAP/100;
		}
		else if(sct[x][y].designation==DCITY) {
			if(magic(sct[x][y].owner,ARCHITECT)==1){
				ntn[sct[x][y].owner].tgold+= 2*sct[x][y].people * TAXCITY/100;
			}
			else {
				ntn[sct[x][y].owner].tgold+= sct[x][y].people * TAXCITY/100;
			}
		}
		else if(((magic(country,DERVISH)==1)||(magic(country,DESTROYER)==1))&&((sct[x][y].vegitation==ICE)||(sct[x][y].vegitation==DESERT))&&(sct[x][y].people>0)) {
			food=6*sct[x][y].people;
			ntn[sct[x][y].owner].tfood += food;
			ntn[sct[x][y].owner].tgold += food*TAXFOOD/100;
		}
	}

	/*reset military stuff*/
	printf("updating armies and navies\n");
	for(country=1;country<MAXNTN;country++) if(ntn[country].active!=0){
		for(armynum=0;armynum<MAXARM;armynum++){
			if(ASOLD>0) {
				ntn[country].tmil+=ASOLD;
				if(magic(country,VAMPIRE)!=1)
					ntn[country].tgold-=ASOLD*SOLDMAINT;

				/*add movement to all armies */
				switch(ASTAT) {
				case MARCH:
					AMOVE=2*ntn[country].maxmove;
					break;
				case SCOUT:
				case ATTACK:
				case DEFEND:
					AMOVE=ntn[country].maxmove;
					break;
				case GARRISON:
					AMOVE=0;
					break;
				default:
					ASTAT=DEFEND;
					AMOVE=ntn[country].maxmove;
				}
			}
		}
		/*add to movement of fleets*/
		for(nvynum=0;nvynum<MAXNAVY;nvynum++) {
			/*update sea sectors*/
			if(NMER+NWAR>0) {
				if(sct[NXLOC][NYLOC].altitude==WATER) {
					/*all ships sunk on 0 (d12)*/
					/*pirates never are sunk (implicitly)*/
					if(rand()%12==0){
						fprintf(fnews,"3.\tstorm in %d,%d\n",x,y);
						NWAR=0;
						NMER=0;
					}
				}
				NMOVE=3*ntn[country].maxmove;
				ntn[country].tships+=NWAR+NMER;
				ntn[country].tgold-=(NWAR+NMER)*SHIPMAINT;
			}
			else {
				NWAR=0;
				NMER=0;
			}
		}
	}

	/*commodities: feed the people, too much gold?, validate iron*/
	fprintf(fnews,"2\tSTATUS OF THE WORLDS FOOD SUPPLY\n");
	for(country=1;country<MAXNTN;country++) if(ntn[country].active!=0){
		/*soldiers eat  2*/
		ntn[country].tfood-=ntn[country].tmil*2;
		/*civilians eat 1*/
		ntn[country].tfood-=ntn[country].tciv;

		/*starve people*/
		if(ntn[country].tfood<0) for(x=0;x<MAPX;x++) for(y=0;y<MAPX;y++) {
			if((sct[x][y].owner==country)&&((sct[x][y].designation==DCITY)||(sct[x][y].designation==DCAPITOL))&&(ntn[country].tfood<0)){
				/*lose one person in city per three food*/
				/*maximum of 1/3 people in city lost*/
				if(sct[x][y].people>(-1)*ntn[country].tfood){
					sct[x][y].people+=ntn[country].tfood/3;
					ntn[country].tfood=0;
				}
				else {
					ntn[country].tfood+=sct[x][y].people;
					sct[x][y].people*=(2/3);
				}
				fprintf(fnews,"2.\tfamine hits city at %d,%d in %s.\n",x,y,ntn[country].name);
			}
		}

		/*this state can occur if few people live in cities*/
		if(ntn[country].tfood<0) {
			ntn[country].tfood=0;
		}
		else if(ntn[country].tfood>FOODTHRESH*ntn[country].tciv) {
			ntn[country].tgold+=ntn[country].tfood-FOODTHRESH*ntn[country].tciv;
			ntn[country].tfood=FOODTHRESH*ntn[country].tciv;
		}

		if(ntn[country].tgold>GOLDTHRESH*ntn[country].jewels){
			x=ntn[country].tgold-GOLDTHRESH*ntn[country].jewels;
			ntn[country].jewels += x/GOLDTHRESH;
			ntn[country].tgold  -= x;
		}
		else if(ntn[country].tgold<((-1)*JEWELTHRESH)*ntn[country].jewels){
			fprintf(fnews,"3.\tTAX REVOLT IN NATION %s\n",ntn[country].name);
		}
	}

	fclose(fnews);
	score();
	strcpy(command,"> ");
	strcat(command,EXEFILE);
	system(command);
	strcpy(command,"/bin/sort -n -o ");
	strcat(command,NEWSFILE);
	strcat(command," ");
	strcat(command,NEWSFILE);
	printf("sort done\n");
	system(command);
}

attract(x,y,race)
{
	int Attr=0;

	if((sct[x][y].designation==DGOLDMINE)&&(sct[x][y].gold>3)){
		if(ntn[sct[x][y].owner].jewels<=ntn[sct[x][y].owner].tgold*GOLDTHRESH) Attr+=120;
		else if(sct[x][y].gold>5) Attr+=120;
		else Attr+=75;
	}
	else if((sct[x][y].designation==DFARM)&&(todigit(sct[x][y].vegitation)>6)){
		if(ntn[sct[x][y].owner].tfood<=ntn[sct[x][y].owner].tciv*FOODTHRESH) Attr+=300;
		else if(todigit(sct[x][y].vegitation)==9) Attr+=100;
		else Attr+=40;
	}
	else if(sct[x][y].designation==DCAPITOL) Attr+=200;
	else if(sct[x][y].designation==DCITY) Attr+=125;
	else if((sct[x][y].designation==DMINE)&&(sct[x][y].iron>3)) {
		if(ntn[sct[x][y].owner].tiron<=ntn[sct[x][y].owner].tciv)
			Attr+=120;
		else if(sct[x][y].iron>5) Attr+=100;
		else Attr+=50;
	}

	switch(race){
	case DWARF:
		if((sct[x][y].designation==DGOLDMINE)&&(sct[x][y].gold>=5))
			Attr+=100;
		else if((sct[x][y].designation==DMINE)&&(sct[x][y].iron>=5))
			Attr+=100;

		if(sct[x][y].altitude==MOUNTAIN) Attr+=40;
		else if(sct[x][y].altitude==HILL) Attr+=20;
		else if(sct[x][y].altitude==CLEAR) Attr+=0;
		else Attr=0;
		break;
	case ELF:
		if(sct[x][y].vegitation==JUNGLE) Attr+=40;
		else if(sct[x][y].vegitation==WOOD) Attr+=90;
		else if(sct[x][y].vegitation==FORREST) Attr+=70;

		if((sct[x][y].designation==DGOLDMINE)&&(sct[x][y].gold>=5))
			Attr+=75;

		if(sct[x][y].altitude==MOUNTAIN) Attr-=20;
		else if(sct[x][y].altitude==HILL) Attr-=10;
		else if(sct[x][y].altitude==CLEAR) Attr+=0;
		else Attr=0;
		break;
	case HUMAN:
		Attr+=todigit(sct[x][y].vegitation)*4;

		if((sct[x][y].designation==DGOLDMINE)&&(sct[x][y].gold>=5))
			Attr+=75;
		else if((sct[x][y].designation==DMINE)&&(sct[x][y].iron>=5))
			Attr+=75;
		else if((sct[x][y].designation==DFARM)&&(todigit(sct[x][y].vegitation)>=6))
			Attr+=55;
		else if(sct[x][y].designation==DCAPITOL) Attr+=70;
		else if(sct[x][y].designation==DCITY) Attr+=50;

		if(sct[x][y].altitude==MOUNTAIN) Attr-=10;
		else if(sct[x][y].altitude==HILL) Attr+=00;
		else if(sct[x][y].altitude==CLEAR) Attr+=10;
		else Attr=0;
		break;
	case ORC:
		if(sct[x][y].designation==DCAPITOL) Attr+=120;
		else if(sct[x][y].designation==DCITY) Attr+=75;
		else if((sct[x][y].designation==DGOLDMINE)&&(sct[x][y].gold>=5))
			Attr+=75;
		else if((sct[x][y].designation==DMINE)&&(sct[x][y].iron>=5))
			Attr+=75;

		if(sct[x][y].altitude==MOUNTAIN) Attr+=20;
		else if(sct[x][y].altitude==HILL) Attr+=10;
		else if(sct[x][y].altitude==CLEAR) Attr+=0;
		else Attr=0;
		break;
	default:
		break;
	}
	if((Attr<0)||(movecost[x][y]<0)) Attr=0;
	return(Attr);
}

todigit(character)
{
	int j;
	for(j=0;j<=9;j++) if(character==*(numbers+j)) return(j);
	return(0);
}

armymove(armynum)
{
	int sum,done,place;
	register int x,y;
	sum=0;
	for(x=AXLOC-2;x<=AXLOC+2;x++)
		for(y=AYLOC-2;y<=AYLOC+2;y++)
			if(x>=0&&x<MAPX&&y>=0&&y<MAPY)
				sum+=attr[x][y];

	if(sum==0) {
		AXLOC=ntn[country].capx;
		AYLOC=ntn[country].capy;
		sum=1;
	}
	else {
		place=rand()%sum;
		done=0;
		for(x=AXLOC-2;x<=AXLOC+2;x++) for(y=AYLOC-2;y<=AYLOC+2;y++) {
			if(x>=0&&x<MAPX&&y>=0&&y<MAPY) place-=attr[x][y];
			if((done==0)&&(place<0)&&(movecost[x][y]>0)){
				AXLOC=x;
				AYLOC=y;
				done=1;
				if(sct[x][y].owner==0)
					sct[x][y].owner=country;
			}
		}
	}
}

score()
{
	int x,y;

	printf("\nupdating scores for all nations\n");
	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++)
		if(sct[x][y].people>0) ntn[sct[x][y].owner].score += SECTSCORE;
}
