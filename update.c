/* Conquer: Copyright (c) 1988 by Edward M Barlow
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
#include "data.h"
#include <ctype.h>

extern FILE *fnews;

extern short country;
int attr[MAPX][MAPY];     /*sector attactiveness*/
extern short movecost[MAPX][MAPY];

/*update nation */
void
update()
{
	char command[80];

	if ((fnews=fopen(newsfile,"w"))==NULL) {
		printf("error opening news file\n");
		exit(FAIL);
	}

	/*run each nation in a random order*/
	updexecs();
#ifdef TRADE
	uptrade();
#endif
#ifdef LZARD
	/* run lizard nations */
	updlizards();
#endif

#ifdef MONSTER
	/* update monster nations */
	monster();
#endif

	/*run combat*/
	combat();

	/* capture unoccupied sectors */
	updcapture();

	/*for whole map, update one sector at a time*/
	updsectors();

	/*reset military stuff for whole world*/
	updmil();

	/*commodities: feed the people, too much gold?, validate iron*/
	updcomodities();

#ifdef RANEVENT
	/*run random events */
	randomevent();
#endif RANEVENT

	fprintf(fnews,"1\tIMPORTANT WORLD NEWS\n");
	fclose(fnews);

#ifdef CHEAT
	cheat();
#endif CHEAT

	/* score all nations */
	score();

	sprintf(command,"/bin/rm -f %s*",exefile);
	printf("%s\n",command);
	system(command);

	sprintf( command, "sort -n -o %s %s", newsfile, newsfile );
	printf("%s\n",command);
	system(command);
}

/* returns attractiventess */
int
attract(x,y,race)
{
	register struct s_sector	*sptr = &sct[x][y];
	int	Attr = 1;

	if((sptr->designation==DGOLDMINE)&&(sptr->gold>3)){
		if(ntn[sptr->owner].jewels<=ntn[sptr->owner].tgold*GOLDTHRESH)
			Attr+=120;
		else if(sptr->gold>5) Attr+=120;
		else Attr+=75;
	}
	else if((sptr->designation==DFARM)&&(tofood(sptr->vegetation,sptr->owner)>=6)){
		if(ntn[sptr->owner].tfood<=ntn[sptr->owner].tciv*FOODTHRESH)
			Attr+=300;
		else if(tofood(sptr->vegetation,sptr->owner)==9) Attr+=100;
		else Attr+=40;
	}
	else if(sptr->designation==DCAPITOL) Attr+=200;
	else if(sptr->designation==DCITY) Attr+=125;
	else if((sptr->designation==DMINE)&&(sptr->iron>3)) {
		if(ntn[sptr->owner].tiron<=ntn[sptr->owner].tciv)
			Attr+=120;
		else if(sptr->iron>5) Attr+=100;
		else Attr+=50;
	}

	switch(race){
	case DWARF:
		if((sptr->designation==DGOLDMINE)&&(sptr->gold>=5))
			Attr+=100;
		else if((sptr->designation==DMINE)&&(sptr->iron>=5))
			Attr+=100;

		if(sptr->altitude==MOUNTAIN) Attr+=40;
		else if(sptr->altitude==HILL) Attr+=20;
		else if(sptr->altitude==CLEAR) Attr+=0;
		else Attr=0;
		break;
	case ELF:
		if(sptr->vegetation==JUNGLE) Attr+=40;
		else if(sptr->vegetation==WOOD) Attr+=90;
		else if(sptr->vegetation==FOREST) Attr+=50;

		if((sptr->designation==DGOLDMINE)&&(sptr->gold>=5))
			Attr+=75;

		if(sptr->altitude==MOUNTAIN) Attr-=20;
		else if(sptr->altitude==HILL) Attr-=10;
		else if(sptr->altitude==CLEAR) Attr+=0;
		else Attr=0;
		break;
	case HUMAN:
		Attr+=tofood(sptr->vegetation,sptr->owner)*4;

		if((sptr->designation==DGOLDMINE)&&(sptr->gold>=5))
			Attr+=75;
		else if((sptr->designation==DMINE)&&(sptr->iron>=5))
			Attr+=75;
		else if((sptr->designation==DFARM)&&(tofood(sptr->vegetation,sptr->owner)>=6))
			Attr+=55;
		else if(sptr->designation==DCAPITOL) Attr+=70;
		else if(sptr->designation==DCITY) Attr+=50;

		if(sptr->altitude==MOUNTAIN) Attr-=10;
		else if(sptr->altitude==HILL) Attr+=00;
		else if(sptr->altitude==CLEAR) Attr+=10;
		else Attr=0;
		break;
	case ORC:
		if(sptr->designation==DCAPITOL) Attr+=120;
		else if(sptr->designation==DCITY) Attr+=75;
		else if((sptr->designation==DGOLDMINE)&&(sptr->gold>=5))
			Attr+=75;
		else if((sptr->designation==DMINE)&&(sptr->iron>=5))
			Attr+=75;

		if(sptr->altitude==MOUNTAIN) Attr+=20;
		else if(sptr->altitude==HILL) Attr+=10;
		else if(sptr->altitude==CLEAR) Attr+=0;
		else Attr=0;
		break;
	default:
		break;
	}
	if((Attr<0)||(movecost[x][y]<0)) Attr=0;
	return(Attr);
}

void
armymove(armynum)
int armynum;
{
	int		sum, where;
	register int	x, y;

	sum=0;
	for(x=AXLOC-2;x<=AXLOC+2;x++)
		for(y=AYLOC-2;y<=AYLOC+2;y++)
			if(ONMAP) sum+=attr[x][y];

	if(sum==0) {
		AXLOC=ntn[country].capx;
		AYLOC=ntn[country].capy;
	} else {
		where=rand()%sum;
		for(x=AXLOC-2;x<=AXLOC+2;x++) for(y=AYLOC-2;y<=AYLOC+2;y++) {
			if( x < 0 || x >= MAPX || y < 0 || y >= MAPY )
				continue;

			where -= attr[x][y];
			if( (where < 0 )
			&& movecost[x][y]>=1
			&& movecost[x][y]<=AMOVE
		        &&(land_reachp(AXLOC,AYLOC,x,y,AMOVE,country))){
				AXLOC=x;
				AYLOC=y;
				/* CHANGE SO ARMIES MOVE PSEUDO INDEPENDANTLY */
				if((sct[x][y].designation != DCAPITOL)
				&&(sct[x][y].designation != DCITY)
				&&(sct[x][y].owner==country))
					attr[x][y]/=2;

				if(sct[x][y].owner==0){
					sct[x][y].owner=country;
					attr[x][y]/=2;
				}
		
				return;
			} /* if */
		} /* for for */

		/*do again - have this block if lots of bad terrain*/
		/*what could happen is that it won't find a move first time*/
		for(x=AXLOC-2;x<=AXLOC+2;x++) for(y=AYLOC-2;y<=AYLOC+2;y++) {
			if( x < 0 || x >= MAPX || y < 0 || y >= MAPY )
				continue;

			where -= attr[x][y];
			if( (where < 0 )
			&& movecost[x][y]>=1
			&& movecost[x][y]<=AMOVE
		        &&(land_reachp(AXLOC,AYLOC,x,y,AMOVE,country))){
				AXLOC=x;
				AYLOC=y;
				if(sct[x][y].owner==0)
					sct[x][y].owner=country;
				return;
			} /* if */
		} /* for for */
	} /* if */
}

void
score()
{
	int x;
	printf("\nupdating scores for all nations\n");
	for(x=1;x<MAXNTN;x++) if(ntn[x].active!=0) ntn[x].score += score_one(x);
}

#ifdef CHEAT
/* this routine cheats in favor of npc nations */
void
cheat()
{
	int x,y;
	int bonus=0, count=0, npcavg, pcavg, avgscore=0;
	/* add gold */
	for(x=1;x<MAXNTN;x++) if(ntn[x].active>1) {
		if((ntn[x].tgold<ntn[x].tciv)
		&&( rand()%5==0)){
			ntn[x].tgold+=10000;
			printf("npc cheat routine - add $10000 to nation %s\n",ntn[x].name);
		}
	}

	for(x=1;x<MAXNTN;x++) if(ntn[x].active!=0)
		if(ntn[x].active==1)  {
			bonus+=ntn[x].aplus+ntn[x].dplus;
			avgscore+=ntn[x].score;
			count++;
		}

	if(count==0) return;
	pcavg = bonus / count;
	avgscore /= count;
	printf("pc average score is %d count is %d\n",avgscore,count);

	bonus=0;
	count=0;
	for(x=1;x<MAXNTN;x++) if(ntn[x].active!=0)
		if(ntn[x].active!=1)  {
			bonus+=ntn[x].aplus+ntn[x].dplus;
			count++;
		}
	if(count==0) return;
	npcavg = bonus / count;
	for(x=1;x<MAXNTN;x++) 
	if((ntn[x].active > 1) 
	&&(ntn[x].score < avgscore)
	&&(rand()%100 < (pcavg-npcavg))) {
		if(ntn[x].aplus>ntn[x].dplus) ntn[x].dplus+=1;
		else ntn[x].aplus+=1;
		printf("npc cheat routine - add 1%% to nation %s combat skill\n",ntn[x].name);
	}

	/* cheat by making npc's frendlier to each other if they are */
	/* of the same race */
	for(x=1;x<MAXNTN;x++) if(ntn[x].active>=2)
		for(y=1;y<MAXNTN;y++) if(ntn[y].active>=2)
			if((ntn[x].dstatus[y]!=CONFEDERACY)
			&&(ntn[x].dstatus[y]!=UNMET)){
				if(ntn[x].race == ntn[y].race){
					ntn[x].dstatus[y]--;
				} else {
					if(ntn[x].dstatus[y]!=JIHAD)
						if(rand()%4==0)
							ntn[x].dstatus[y]--;
				}
			}
}
#endif CHEAT

/* update all nations in a random order, move civilians of that nation */
updexecs()
{
	register struct s_sector	*sptr;
	register int i, j;
	register int x,y;
	int moved,done, number=0;

	int finis=FALSE;
	int execed[MAXNTN];

	for(country=0;country<MAXNTN;country++) execed[country]=FALSE;

	system("date");

	while(finis==FALSE){

		/*get random active nation*/
		country=(rand()%(MAXNTN-1))+1;
		if(ntn[country].active <= 0) continue;

		done=FALSE;
		number=0;
		/*Find the next unupdated nation*/
		while(done==FALSE){
			if((ntn[country].active>0)
			&&(execed[country]==FALSE)) {
				done=TRUE;
				execed[country]=TRUE;
			} else {
				country++;
				number++;
				if(number>MAXNTN) {
					finis=TRUE;
					done=TRUE;
				}
				else if(country>=MAXNTN) country=1;
			}
		}

		if(finis==TRUE) continue;

		printf("updating nation number %d -> %s\n",country,ntn[country].name);

		/*if execute is 0 and PC nation then they did not move*/
		if((execute()==0)&&(ntn[country].active==1)){
			printf("\tnation %s did not move\n",ntn[country].name);
#ifdef CMOVE
			printf("\tthe computer will move for %s\n",ntn[country].name);
			fprintf(fnews,"1.\tthe computer will move for %s\n",ntn[country].name);
			nationrun();
#endif
		}
#ifdef NPC
		/* run npc nations */
		if(ntn[country].active>=2) {
			nationrun();
			/*do magic*/
#ifdef ORCTAKE
			if(magic(country,MA_MONST)==1) {
				if(x=takeover(5,0)==1)
				printf("SUCCESSFUL TAKEOVER OF %d",x);
			} else if(magic(country,AV_MONST)==1) {
				if(x=takeover(3,0)==1)
				printf("SUCCESSFUL TAKEOVER OF %d",x);
			} else if(magic(country,MI_MONST)==1){
				if(x=takeover(1,0)==1)
				printf("SUCCESSFUL TAKEOVER OF %d",x);
			}
#endif ORCTAKE
		}
#endif

		/*update movement array*/
		updmove( ntn[country].race,country );

		/*THIS IS WHERE YOU ZERO THE ATTR MATRIX*/
		/*calculate sector attractiveness*/
		for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) {
			sptr = &sct[x][y];
			if((sptr->owner==country)
			&&(tofood(sptr->vegetation,sptr->owner)!=0)){
				attr[x][y]=attract(x,y,ntn[country].race);
			}
			else if(((magic(sptr->owner,DERVISH)==1)
			||(magic(sptr->owner,DESTROYER)==1))
			&&((sptr->vegetation==ICE)
			||(sptr->vegetation==DESERT))) {
				attr[x][y]=36;
			}
			else attr[x][y]=0;
		}

		/*if near capitol add to attr*/
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
		for(x=0; x<MAPX; x++ ) for(y=0; y<MAPY; y++) {

			sptr = &sct[x][y];
			if( sptr->owner != country )
				continue;
			if( sptr->people == 0 )
				continue;

			for(i=x-2;i<=x+2;i++) {
				if( i < 0 || i >= MAPX  )
					continue;

				for(j=y-2;j<=y+2;j++) {
					if( j < 0 || j >= MAPY )
						continue;
					if( sct[i][j].owner != country)
						continue;
					moved=(sptr->people*attr[i][j]-sct[i][j].people*attr[x][y])/(1+5*(attr[i][j]+attr[x][y]));
					if( moved <= 0 )
						continue;

					sct[i][j].people += moved;
					sptr->people -= moved;
				} /* for */
			} /* for */
		} /* for */
	} /* while */

	/*zero out all recalculated values*/
	for(country=0;country<MAXNTN;country++){
		ntn[country].tships=0;
		ntn[country].tmil=0;
		if(magic(country,SUMMON)==TRUE) {
			if(rand()%4==0) ntn[country].spellpts/=2;
			ntn[country].spellpts+=4;
			if(magic(country,WYZARD)==TRUE)
				ntn[country].spellpts+=3;
			if(magic(country,SORCERER)==TRUE)
				ntn[country].spellpts+=3;
		}
	}

}

#ifdef LZARD
/* update lizards and monsters */
updlizards()
{
	register int i, j;
	int armynum;

	puts("updating lizards\n ");
	country = NLIZARD;
	/*move to lizard castle*/
	for(armynum=0;armynum<MAXARM;armynum++)
	if((ASOLD>0)&&(ASTAT==ATTACK)) {
		if(ntn[NLIZARD].arm[armynum-1].sold<=0) {
			ASOLD=0;
			continue;
		}
		AMOVE =20;	/* just in case god wants to move them */
		AXLOC = ntn[NLIZARD].arm[armynum-1].xloc;
		AYLOC = ntn[NLIZARD].arm[armynum-1].yloc;
		for(i=ntn[NLIZARD].arm[armynum-1].xloc-1;i<=ntn[NLIZARD].arm[armynum-1].xloc+1;i++) {
			for(j=ntn[NLIZARD].arm[armynum-1].yloc-1;j<=ntn[NLIZARD].arm[armynum-1].yloc+1;j++) {
				if((i>=0)&&(j>=0)&&(i<MAPX)&&(j<MAPY)
				&&(sct[i][j].altitude!=WATER)
				&&(sct[i][j].altitude!=PEAK)
				&&(sct[i][j].owner != NLIZARD)
				&&(rand()%3==0)){
					AXLOC = i;
					AYLOC = j;
				}
			}
		}
	}
#ifdef DEBUG
	for(armynum=0;armynum<MAXARM;armynum++) {
		if((ASOLD>0)&&(sct[AXLOC][AYLOC].altitude==WATER))
			printf("ERROR line %d... %s army %d in water (army %d: x: %d y: %d)\n",__LINE__,ntn[NLIZARD].name,armynum,armynum-1, ntn[NLIZARD].arm[armynum-1].xloc, ntn[NLIZARD].arm[armynum-1].yloc);
	}
#endif DEBUG
}
#endif

/* capture unoccupied sectors */
updcapture()
{
	register struct s_sector	*sptr;
	int armynum;

	fprintf(fnews,"3\tNEWS ON WHAT SECTORS HAVE BEEN CAPTURED\n");
	/*look for any areas where armies alone in sector*/
	prep(country);
	for(country=1;country<NTOTAL;country++) if(ntn[country].active!=0){
		for(armynum=0;armynum<MAXARM;armynum++)
/* cheat in favor of npcs as the routines assume 75 man armies */
		if(((ntn[country].active==1)&&(ASOLD>TAKESECTOR))
		||((ntn[country].active>1)&&(ASOLD>75))){
			sptr = &sct[AXLOC][AYLOC];
			if(sptr->owner==0){
				sptr->owner=country;
			}
			else if((sptr->owner!=country)
			&&(ntn[country].dstatus[sptr->owner]>=WAR)
			&&(occ[AXLOC][AYLOC]==country)){

				if((sptr->owner!=0)
				&&(ntn[sptr->owner].race!=ntn[country].race))
					if(magic(country,SLAVER)==TRUE){
					flee(AXLOC,AYLOC,1,TRUE);
					}else{
					flee(AXLOC,AYLOC,1,FALSE);
					}
#ifdef HIDELOC
				fprintf(fnews,"3.\tarea captured by %s from %s\n",ntn[country].name,ntn[sptr->owner].name);
#else
				fprintf(fnews,"3.\tarea %d,%d captured by %s from %s\n",AXLOC,AYLOC,ntn[country].name,ntn[sptr->owner].name);
#endif HIDELOC
				sptr->owner=country;
			}
		}
	}

	/* capture countries */
	for(country=1;country<MAXNTN;country++)
		if((ntn[country].active>=2)
		&&((ntn[country].tciv==0)
		||(sct[ntn[country].capx][ntn[country].capy].owner!=country)))
			destroy(country);
}

/* update sectors */
updsectors()
{
	register struct s_sector	*sptr;
	register struct nation		*nptr;
	register int i, j;
	register int x,y;

	printf("\nupdating all sectors\n");
	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) {
		sptr = &sct[x][y];
		if(sptr->owner == 0) continue;
		nptr = &ntn[sptr->owner];

		/* add to contents of sector */
		if(rand()%100<FINDPERCENT) {
			if(rand()%2==0) sct[x][y].iron++;
			else sct[x][y].gold++;
		}

		/* if huge number dont reproduce */
		if(sptr->people > BIG / 50L) {
			if(sptr->people * sptr->iron > 2*(rand()%100)*TOMUCHMINED)
				sptr->iron--;
			if(sptr->people * sptr->gold > 2*(rand()%100)*TOMUCHMINED)
				sptr->gold--;
		} else if((sptr->people > TOMANYPEOPLE)&&(sptr->designation!=DCITY)&&(sptr->designation!=DCAPITOL)){
			sptr->people += (nptr->repro * sptr->people)/200;
			if(sptr->people * sptr->iron > 2*(rand()%100)*TOMUCHMINED)
				sptr->iron--;
			if(sptr->people * sptr->gold > 2*(rand()%100)*TOMUCHMINED)
				sptr->gold--;
		} else if(sptr->people<100) {
			sptr->people+=sptr->people/10;
		} else {
			sptr->people += (nptr->repro * sptr->people)/100;
			if(sptr->people * sptr->iron > (rand()%100)*TOMUCHMINED)
				sptr->iron--;
			if(sptr->people * sptr->gold > (rand()%100)*TOMUCHMINED)
				sptr->gold--;
		}

		/*check all adjacent sectors and decide if met */
		for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
		if(i>=0&&i<MAPX&&j>=0&&j<MAPY&&(sct[i][j].owner!=0)) {
			if(sptr->owner!=sct[i][j].owner) {
				if(nptr->dstatus[sct[i][j].owner]==UNMET)
					newdip(sptr->owner,sct[i][j].owner);
				if(ntn[sct[i][j].owner].dstatus[sptr->owner]==UNMET)
					newdip(sct[i][j].owner,sptr->owner);
			}
		}

		/* if desert sector... reverts to desert */
		if(tofood(sptr->vegetation,sptr->owner)<DESFOOD){
			sptr->designation=DNODESIG;
		}
	}

	for(country=1;country<MAXNTN;country++) {
		if(ntn[country].active != 0){
			spreadsheet(country);
			ntn[country].tsctrs = spread.sectors;
			ntn[country].tciv=spread.civilians;
			ntn[country].tfood=spread.food;
			ntn[country].tgold=spread.gold;
			ntn[country].tiron=spread.iron;
			ntn[country].jewels=spread.jewels;
		}
	}
}

/* reset military stuff */
updmil()
{
	register int x,y;
	int armynum,nvynum;

	printf("updating armies and navies\n");
	for(country=1;country<NTOTAL;country++) if(ntn[country].active!=0){
		for(armynum=0;armynum<MAXARM;armynum++){
			if(ASOLD>0) {
				ntn[country].tmil+=ASOLD;
				/*add movement to all armies */
				/*unitmove is 10 times movement rate*/
				switch(ASTAT) {
				case MARCH:
					AMOVE=(ntn[country].maxmove * *(unitmove+(ATYPE%100)))/5;
					break;
				case SCOUT:
				case ATTACK:
				case DEFEND:
					AMOVE=(ntn[country].maxmove * *(unitmove+(ATYPE%100)))/10;
					break;
				case GARRISON:
					AMOVE=0;
					break;
				default:
					ASTAT=DEFEND;
					AMOVE=(ntn[country].maxmove * *(unitmove+(ATYPE%100)))/10;
				}
				if((magic(country,ROADS)==1)
				&&(sct[AXLOC][AYLOC].owner!=country))
					if(AMOVE>4) AMOVE-=4;

				if((magic(country,VAMPIRE)==1)
				&&(ATYPE<100)){
				ntn[country].tgold -= ASOLD * (*(unitmaint+(ATYPE))) / 4;
				} else
				if((magic(country,SAPPER)==1)
				&&((ATYPE==A_CATAPULT)||(ATYPE==A_SEIGE))){
				ntn[country].tgold -= ASOLD * (*(unitmaint+(ATYPE))) / 2;
				} else
				ntn[country].tgold -= ASOLD * (*(unitmaint+(ATYPE%100)));
				if(ATYPE>=MINMONSTER)
				ntn[country].jewels -= ASOLD * (*(unitmaint+(ATYPE%100))/5);
			}
		}
		/*add to movement of fleets*/
		for(nvynum=0;nvynum<MAXNAVY;nvynum++) {
			/*update sea sectors*/
			if( NMER + NWAR > 0 ) {
				if(sct[NXLOC][NYLOC].altitude==WATER) {
#ifdef STORMS
/*
 *	Storms should stay around and slowly move
 *	around the world.
 */
				/*all ships sunk on percentage PSTORM*/
				/*pirates never are sunk (implicitly)*/
				if( country != NPIRATE &&
				(rand()%100 < PSTORM) ) {
					x = NXLOC;
					y = NYLOC;
#ifdef HIDELOC
					fprintf(fnews,"3.\tstorm sinks %s fleet at sea\n",ntn[country].name);
#else
					fprintf(fnews,"3.\tstorm sinks %s fleet in %d,%d\n",ntn[country].name,x,y);
#endif HIDELOC
					NWAR=0;
					NMER=0;
				}
#endif
				}
				NMOVE = 3 * ntn[country].maxmove * NCREW;
				NMOVE /= ((NWAR+NMER)*SHIPCREW);
				ntn[country].tships += NWAR + NMER;
				ntn[country].tgold -= (NWAR + NMER) * SHIPMAINT;
			} else {
				NWAR=0;
				NMER=0;
			}
		} /* for */
	}
}

/* update commodities */
updcomodities()
{
	FILE *fpmsg;
	register struct s_sector	*sptr;
	register int x,y;
	long xx;
	char command[80];
	long dead;

	fprintf(fnews,"2\tWORLD ECONOMY & DECLARATIONS OF WAR\n");
	for(country=1;country<MAXNTN;country++) if(ntn[country].active!=0){
		/*soldiers eat  2*/
		ntn[country].tfood-=ntn[country].tmil*2;
		/*civilians eat 1*/
		ntn[country].tfood-=ntn[country].tciv;

		/*starve people*/
		if(ntn[country].tfood<0) for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) {
			sptr = &sct[x][y];
			if((sptr->owner==country)
			&&((sptr->designation==DCITY)
			||(sptr->designation==DCAPITOL))
			&&(ntn[country].tfood<0)){
				/*lose one person in city per three food*/
				/*maximum of 1/3 people in city lost*/
				if(sptr->people < ntn[country].tfood){
					sptr->people+=ntn[country].tfood/3;
					ntn[country].tfood=0;
				}
				else {
					ntn[country].tfood+=sptr->people;
					dead = sptr->people/3;
					sptr->people -= dead;
				}
				fprintf(fnews,"2.\tfamine hits city at %d,%d in %s.\n",x,y,ntn[country].name);
#ifdef HIDELOC
				fprintf(fnews,"2.\tfamine hits town in %s.\n",ntn[country].name);
#else
				fprintf(fnews,"2.\tfamine hits town at %d,%d in %s.\n",x,y,ntn[country].name);
#endif HIDELOC
				printf("famine hits town at %d,%d in %s.\n",x,y,ntn[country].name);
				sprintf(command,"%s%d",msgfile,country);
				if(ntn[country].active==1)
				if((fpmsg=fopen(command,"a+"))==NULL) {
				printf("error opening %s\n",command);
				} else {
				fprintf(fpmsg,"%s notice from program\n%s\n",ntn[country].name,ntn[country].name);
				fprintf(fpmsg,"%s famine hits town at %d,%d in %s.-> %ld people reduced by %ld\n%s\n",ntn[country].name,x,y,ntn[country].name,sptr->people,dead,ntn[country].name);
				fprintf(fpmsg,"END\n");
				fclose(fpmsg);
				}
			}
		}
		/*this state can occur if few people live in cities*/
		if(ntn[country].tfood<0) {
			ntn[country].tfood=0L;
		}
		else if(ntn[country].tfood>FOODTHRESH*ntn[country].tciv) {
			ntn[country].tgold+=ntn[country].tfood-FOODTHRESH*ntn[country].tciv;
			ntn[country].tfood=FOODTHRESH*ntn[country].tciv;
		}

		if(ntn[country].tgold>GOLDTHRESH*ntn[country].jewels){
			xx=ntn[country].tgold-GOLDTHRESH*ntn[country].jewels;
			ntn[country].jewels += xx/GOLDTHRESH;
			ntn[country].tgold  -= xx;
		}
		else if(ntn[country].tgold > JEWELTHRESH * ntn[country].jewels){
			fprintf(fnews,"3.\tTAX REVOLT IN NATION %s\n",ntn[country].name);
		}

		/* fix overflow problems */
     	if(ntn[country].tgold < -1*BIG)  {
			fprintf(fnews,"2.\tVariable Overflow - gold in nation %s\n",ntn[country].name);
			ntn[country].tgold=BIG;
		}
     	if(ntn[country].tfood < -1*BIG)  {
			fprintf(fnews,"2.\tVariable Overflow - food in nation %s\n",ntn[country].name);
			ntn[country].tfood=BIG;
		}
     	if(ntn[country].jewels < -1*BIG) {
			fprintf(fnews,"2.\tVariable Overflow - jewels in nation %s\n",ntn[country].name);
			ntn[country].jewels=BIG;
		}
     	if(ntn[country].tiron < -1*BIG)  {
			fprintf(fnews,"2.\tVariable Overflow - iron in nation %s\n",ntn[country].name);
			ntn[country].tiron=BIG;
		}
	}
}
