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

#include "header.h"
#include "data.h"
#include <ctype.h>

FILE *fnews, *fopen();

extern short country;
int attr[MAPX][MAPY];     /*sector attactiveness*/
extern short movecost[MAPX][MAPY];

/*update nation file*/
update()
{
	register struct s_sector	*sptr;
	register struct nation		*nptr;
	FILE *fpmsg;
	register int i, j;
	register int x,y;
	int moved,armynum,nvynum,done,finis=0, number=0;
	int food,iron;
	char command[80];
	int execed[MAXNTN];
	long	city_pop, cap_pop;

	if ((fnews=fopen(newsfile,"w"))==NULL) {
		printf("error opening news file\n");
		exit(1);
	}

	/*run each nation in a random order*/
	country=0;
	execute();
	for(i=0;i<MAXNTN;i++) execed[i]=0;
	system("date");
	while(finis==0){
		/*get random active nation*/
		country=(rand()%(MAXNTN-1))+1;
		if(ntn[country].active <= 0)
			continue;

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

		if(finis==1)
			continue;

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
		/*run npc nations*/
		if(ntn[country].active>=2) {
			nationrun();
			/*do magic*/
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
		}
#endif

		/*update movement array*/
		updmove( ntn[country].race );

		/*THIS IS WHERE YOU ZERO THE ATTR MATRIX*/
		/*calculate sector attractiveness*/
		for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) {
			sptr = &sct[x][y];
			if((sptr->owner==country)
			&&(isdigit(sptr->vegetation)!=0)){
				attr[x][y]=attract(x,y,ntn[country].race);
			}
			else if(((magic(country,DERVISH)==1)
			||(magic(country,DESTROYER)==1))
			&&((sptr->vegetation==ICE)
			||(sptr->vegetation==DESERT))) {
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
		for(x=0; x<MAPX; x++ ) {
			for(y=0; y<MAPY; y++) {
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
#if 0
if( country == 18 ) {
	printf( "moving %d people from (%d,%d) a = %d to (%d,%d) a = %d\n",
		moved, x, y, attr[x][y], i, j, attr[i][j] );
}
#endif
						if( moved <= 0 )
							continue;

						sct[i][j].people += moved;
						sptr->people -= moved;
					} /* for */
				} /* for */
			} /* for */
		} /* for */
	} /* while */

	/* run npc nations */
#ifdef LZARD
	puts("updating lizards\n ");
	country = NLIZARD;
	execute();
	armynum=0;
	/*move to lizard castle*/
	for(armynum=0;armynum<MAXARM;armynum++) if(ASTAT!=GARRISON){
		x = AXLOC;
		y = AYLOC;
		for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++){
			if((i>=0)&&(j>=0)&&(i<MAPX)&&(j<MAPY)
			&&(sct[i][j].designation==DCASTLE)){
				x = i;
				y = j;
				break;
			}
		}
		for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++){
			if((i>=0)&&(j>=0)&&(i<MAPX)&&(j<MAPY)
			&&(sct[i][j].altitude!=WATER) 
			&&(sct[i][j].altitude!=PEAK) 
			&&(sct[i][j].owner != NLIZARD) 
			&&(rand()%2==0)){
				x = i;
				y = j;
				break;
			}
		}
		AXLOC = x;
		AYLOC = y;
	}
#endif

#ifdef MONSTER
	monster();
#endif

	/*run random events */
#ifdef RANEVENT
	randomevent(); 
#endif
	/*run combat*/
	combat();

	fprintf(fnews,"3\tNEWS ON WHAT SECTORS HAVE BEEN CAPTURED");
	/*look for any areas where armies alone in sector*/
	prep();
	for(country=1;country<NTOTAL;country++) if(ntn[country].active!=0){
		for(armynum=0;armynum<MAXARM;armynum++) if(ASOLD>75){
			sptr = &sct[AXLOC][AYLOC];
			if(sptr->owner==0){
				sptr->owner=country;
			}
			else if((sptr->owner!=country)
			&&(ntn[country].dstatus[sptr->owner]>=WAR)
			&&(occ[AXLOC][AYLOC]==country)){

				if((sptr->owner!=0)
				&&(ntn[sptr->owner].race!=ntn[country].race)) 
					flee(AXLOC,AYLOC,1);
				fprintf(fnews,"\n3.\tarea %d,%d captured by %s from %s",AXLOC,AYLOC,ntn[country].name,ntn[sptr->owner].name);
				sptr->owner=country;
			}
		}
	}
	fprintf(fnews,"\n1\tIMPORTANT WORLD NEWS\n");

	for(country=1;country<MAXNTN;country++) 
		if((ntn[country].active>=2)
		&&((ntn[country].tciv==0)
		||(sct[ntn[country].capx][ntn[country].capy].owner!=country)))
			destroy();

	/*zero out all recalculated values*/
	for(i=0;i<MAXNTN;i++){
		ntn[i].tsctrs=0;
		ntn[i].tships=0;
		ntn[i].tciv=0;
		ntn[i].tmil=0;
	}

	/*for whole map, update one sector at a time, owned sectors only*/
	printf("\nupdating all sectors\n");
	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) {
		sptr = &sct[x][y];
		if(sptr->owner==0)
			continue;
		nptr = &ntn[ sptr->owner ];

		/*check all adjacent sectors and decide if met*/
		for(i=x-1;i<=x+1;i++) for(j=y-2;j<=y+2;j++) {
			if(i>=0&&i<MAPX&&j>=0&&j<MAPY&&(sct[i][j].owner!=0)) {
				if(sptr->owner!=sct[i][j].owner) {
					if(nptr->dstatus[sct[i][j].owner]==UNMET) newdip(sptr->owner,sct[i][j].owner);
					if(ntn[sct[i][j].owner].dstatus[sptr->owner]==UNMET) newdip(sct[i][j].owner,sptr->owner);
				}
			}
		}

		/*update nation file for owner*/
		nptr->tsctrs++;
		nptr->tciv += sptr->people;

		/*grow populations*/
		if(sptr->people<100) sptr->people+=sptr->people/10;
		else sptr->people+=(nptr->repro*sptr->people)/100;

		/*PRODUCE*/
		/*increase tmin based on mined stuff...*/
		if(sptr->designation==DMINE) {
			iron=sptr->iron*sptr->people;
			if(magic(sptr->owner,MINER)==1) iron*=2;
			if(magic(sptr->owner,STEEL)==1) iron*=2;
			nptr->tiron += iron;
			nptr->tgold += iron*TAXIRON/100;
		}
		/*harvest food*/
		else if(sptr->designation==DFARM) {
			food= todigit(sptr->vegetation)*sptr->people;
			nptr->tfood += food;
			nptr->tgold += food*TAXFOOD/100;
		}
		/*gold mines produce gold*/
		else if(sptr->designation==DGOLDMINE) {
			if(magic(sptr->owner,MINER)==1){
				nptr->tgold +=  2*sptr->gold * sptr->people * TAXGOLD/100;
				nptr->jewels += 2*sptr->gold * sptr->people;
			}
			else {
				nptr->tgold += sptr->gold * sptr->people * TAXGOLD/100;
				nptr->jewels += sptr->gold * sptr->people;
			}
		}
		else if(sptr->designation==DCAPITOL) {
			if((x!=nptr->capx)&&(y!=nptr->capy)) {
				cap_pop = 0;
				city_pop = 3 * sptr->people;
				sptr->designation = DCITY;
			} else {
				city_pop = 0;
				cap_pop = sptr->people;
			}

			if( magic(sptr->owner, ARCHITECT ) ) {
				city_pop *= 2;
				cap_pop *= 2;
			}

			nptr->tgold += (long) city_pop * TAXCITY / 100;
			nptr->tgold += (long) cap_pop * TAXCAP / 100;
		}
		else if(sptr->designation==DCITY) {
			city_pop = sptr->people;
			if( magic(sptr->owner, ARCHITECT ) )
				city_pop *= 2;
			nptr->tgold += (long) city_pop * TAXCITY / 100;
		}
		else if(((magic(country,DERVISH)==1)
		||(magic(country,DESTROYER)==1))
		&&((sptr->vegetation==ICE)
		||(sptr->vegetation==DESERT))
		&&(sptr->people>0)) {
			food=6*sptr->people;
			nptr->tfood += food;
			nptr->tgold += food*TAXFOOD/100;
		}
	}

	/*reset military stuff*/
	printf("updating armies and navies\n");
	for(country=1;country<NTOTAL;country++) if(ntn[country].active!=0){
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
			if( NMER + NWAR > 0 ) {
				if(sct[NXLOC][NYLOC].altitude==WATER) {
#if 0
/*
 *	Storms should stay around and slowly move
 *	around the world.
 */
					/*all ships sunk on 0 (d12)*/
					/*pirates never are sunk (implicitly)*/
					if( country != NPIRATE && rand() % 12 == 0 ) {
						x = NXLOC;
						y = NYLOC;
						fprintf(fnews,"3.\tstorm in %d,%d\n",x,y);
						NWAR=0;
						NMER=0;
					}
#endif
				}
				NMOVE = 3 * ntn[country].maxmove;
				ntn[country].tships += NWAR + NMER;
				ntn[country].tgold -= (NWAR + NMER) * SHIPMAINT;
			} else {
				NWAR=0;
				NMER=0;
			}
		} /* for */
	}

	/*commodities: feed the people, too much gold?, validate iron*/
	fprintf(fnews,"2\tSTATUS OF THE WORLDS FOOD SUPPLY\n");
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
					city_pop = sptr->people/3;
					sptr->people -= city_pop;
				}
				fprintf(fnews,"2.\tfamine hits city at %d,%d in %s.\n",x,y,ntn[country].name);
				sprintf(command,"%s%d",msgfile,country);
				if(ntn[country].active==1)
				if((fpmsg=fopen(command,"a+"))==NULL) {
				printf("error opening %s\n",command);
				} else {
				fprintf(fpmsg,"%s notice from program\n%s\n",ntn[country].name,ntn[country].name);
				fprintf(fpmsg,"%s famine hits city at %d,%d in %s.-> %d people reduced by %d\n%s\n",ntn[country].name,x,y,ntn[country].name,sptr->people,city_pop,ntn[country].name);
				fprintf(fpmsg,"END\n");
				fclose(fpmsg);
				}
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
		else if(ntn[country].tgold > JEWELTHRESH * ntn[country].jewels){
			fprintf(fnews,"3.\tTAX REVOLT IN NATION %s\n",ntn[country].name);
		}
	}

	fclose(fnews);
	score();

	sprintf(command,"rm %s*",exefile);
	printf("%s\n",command);
	system(command);

	sprintf( command, "sort -n -o %s %s", newsfile, newsfile );
	printf("%s\n",command);
	system(command);
  
}

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
	else if((sptr->designation==DFARM)&&(todigit(sptr->vegetation)>6)){
		if(ntn[sptr->owner].tfood<=ntn[sptr->owner].tciv*FOODTHRESH) 
			Attr+=300;
		else if(todigit(sptr->vegetation)==9) Attr+=100;
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
		else if(sptr->vegetation==FORREST) Attr+=70;

		if((sptr->designation==DGOLDMINE)&&(sptr->gold>=5))
			Attr+=75;

		if(sptr->altitude==MOUNTAIN) Attr-=20;
		else if(sptr->altitude==HILL) Attr-=10;
		else if(sptr->altitude==CLEAR) Attr+=0;
		else Attr=0;
		break;
	case HUMAN:
		Attr+=todigit(sptr->vegetation)*4;

		if((sptr->designation==DGOLDMINE)&&(sptr->gold>=5))
			Attr+=75;
		else if((sptr->designation==DMINE)&&(sptr->iron>=5))
			Attr+=75;
		else if((sptr->designation==DFARM)&&(todigit(sptr->vegetation)>=6))
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

todigit(character)
register int	character;
{
	if( character >= '0' && character <= '9' )
		return( character - '0' );

	return( 0 );
}

armymove(armynum)
{
	int		sum, place;
	register int	x, y;

	sum=0;
	for(x=AXLOC-2;x<=AXLOC+2;x++)
		for(y=AYLOC-2;y<=AYLOC+2;y++)
			if(ONMAP) sum+=attr[x][y];

	if(sum==0) {
		AXLOC=ntn[country].capx;
		AYLOC=ntn[country].capy;
	} else {
		place=rand()%sum;
		for(x=AXLOC-2;x<=AXLOC+2;x++) for(y=AYLOC-2;y<=AYLOC+2;y++) {
			if( x < 0 || x >= MAPX || y < 0 || y >= MAPY )
				continue;

			place -= attr[x][y];
			if( (place < 0 )
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

		/*do again - have this block if lots of bad terrain*/
		/*what could happen is that it won't find a move first time*/
		for(x=AXLOC-2;x<=AXLOC+2;x++) for(y=AYLOC-2;y<=AYLOC+2;y++) {
			if( x < 0 || x >= MAPX || y < 0 || y >= MAPY )
				continue;

			place -= attr[x][y];
			if( (place < 0 )
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

score()
{
  	int x;
  	printf("\nupdating scores for all nations\n");
  	for(x=1;x<MAXNTN;x++) if(ntn[x].active!=0) ntn[x].score += score_one(x);
}

