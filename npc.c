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

#include "header.h"
#include "data.h"
#include <ctype.h>

extern FILE *fnews;

int	stx, sty, endx, endy;	/* npc range of operations */
extern	short country;
extern	int **attr;     	/*sector attactiveness*/
extern	int	dissarray;	/* has nation lost its leader */
int	peace;	/*is 8 if at peace, 12 if at war*/

void
prtattr()
{
#ifdef DEBUG
	int x,y;
/*	FOR TESTING OF WHAT THE ATTRACTIVENESS ENDS UP LIKE  */
	printf("Attractiveness for sectors around CAPITAL at %d %d veg alt des\n",curntn->capx,curntn->capy);
	for(y=curntn->capy-3;y<curntn->capy+4;y++) {
		printf("\n");
		for(x=curntn->capx-3;x<curntn->capx+4;x++) if(ONMAP(x,y)) {
			if( sct[x][y].altitude != WATER ) {
				printf("[%2d,%2d]   ",x,y);
			}
		}
		printf("\n");
		for(x=curntn->capx-3;x<curntn->capx+4;x++) if(ONMAP(x,y)) {
			if( sct[x][y].altitude != WATER ) {
				printf("%5d %c%c%c "
					,attr[x][y]
					,sct[x][y].vegetation
					,sct[x][y].altitude
					,sct[x][y].designation);
			} else	printf("0      ~  ");
		}
		printf("\n");
	}
#endif DEBUG
}

/*newdip() diplomacy if unmet - ntn 1 is nation you are updating*/
void
newdip(ntn1,ntn2)
int	ntn1,ntn2;
{
	if( ispc( ntn[ntn1].active ) ) {
		if( ntn[ntn2].race==ORC ) 
			ntn[ntn1].dstatus[ntn2]=HOSTILE;
		else	ntn[ntn1].dstatus[ntn2]=NEUTRAL;
		return;
	}
	if((ntn[ntn1].race==ORC)
	||( ntn[ntn2].race==ORC)) {
		if(ntn[ntn1].dstatus[ntn2]==UNMET)
			if((rand()%2==0)|| ispc(ntn[ntn1].active))
				ntn[ntn1].dstatus[ntn2]=HOSTILE;
			else
				ntn[ntn1].dstatus[ntn2]=WAR;
	} else if(ismonst(ntn[ntn2].active)) {
		ntn[ntn1].dstatus[ntn2]=WAR;
	} else if(ispc(ntn[ntn1].active)) {
		if(ntn[ntn1].dstatus[ntn2]==UNMET)
			ntn[ntn1].dstatus[ntn2]=NEUTRAL;
	} else if(ntn[ntn1].race==ntn[ntn2].race){
		if(rand()%2<1) ntn[ntn1].dstatus[ntn2]=FRIENDLY;
		else ntn[ntn1].dstatus[ntn2]=NEUTRAL;
	} else ntn[ntn1].dstatus[ntn2]=NEUTRAL;
}

#ifdef MONSTER
void
monster()
{
	for(country=1;country<NTOTAL;country++) {
		curntn = &ntn[country];
		if( curntn->active==NPC_NOMAD ) do_nomad();
		else if( curntn->active==NPC_PIRATE ) do_pirate();
		else if( curntn->active==NPC_BARBARIAN ) do_barbarian();
		else if( curntn->active==NPC_LIZARD ) do_lizard(); /* update.c */
	}
}

void
do_nomad()
{
	int	count;
	short	armynum;
	int	x, y;

	/*move nomads */
	printf("updating nomad (nation %d)\n",country);
	for(armynum=0;armynum<MAXARM;armynum++) if(P_ASOLD>0){
		P_ASTAT=ATTACK;
		P_AMOVE=(curntn->maxmove * *(unitmove+P_ATYPE%UTYPE))/10;
		if(P_ATYPE<MINLEADER) {
			P_ASOLD *= 102;
			P_ASOLD /= 100;
		}
		count=0;
		while( TRUE ) {
			x=P_AXLOC+rand()%5-2;
			y=P_AYLOC+rand()%5-2;

			if( count++ > 100 ) {
				P_ASOLD=0;
				break;
			}
			/* nomads cant stay in the same spot */
			if(( x==P_AXLOC && y==P_AYLOC )
			||(!ONMAP(x,y)
			||(!is_habitable(x,y))) 
			||(!land_reachp(P_AXLOC,P_AYLOC,x,y,curntn->maxmove,country))) continue;

			P_AXLOC=x;
			P_AYLOC=y;
			/*if owned & unoccupied you take & people flee*/
			if( ((sct[x][y].owner) == 0
			|| solds_in_sector( x, y, sct[x][y].owner) == 0 )
			&& (ntn[sct[x][y].owner].active!=NPC_NOMAD) ) {
				fprintf(fnews,"3.\tnomads capture sector %d,%d\n",x,y);
				if(sct[x][y].owner!=0) flee(x,y,1,FALSE);
				sct[x][y].owner=country;
				DEVASTATE(x,y);
			}
			break;
		}
	}
	/* place a few new Nomad armies */
	for(armynum=0;armynum<MAXARM;armynum++) if(P_ASOLD<=0){
		if(rand()%4!=0) continue;
		x=(rand()%(MAPX-8))+4;
		y=(rand()%(MAPY-8))+4;
		if(is_habitable(x,y)) {
			P_AXLOC=x;
			P_AYLOC=y;
			P_ASOLD=100+100*(rand()%6);
			P_ATYPE=A_LT_CAV;
			P_ASTAT=ATTACK;
		}
	}
}

void
do_barbarian()
{
	short armynum;
	int x, y;

	printf("updating barbarian (nation %d)\n",country);
	for(armynum=0;armynum<MAXARM;armynum++) if(P_ASOLD>0){
		P_ASTAT=ATTACK;
		if(P_ATYPE<MINLEADER) {
			P_ASOLD *= 102;
			P_ASOLD /= 100;
		}
		P_AMOVE=(curntn->maxmove * *(unitmove+P_ATYPE%UTYPE))/10;
		x=P_AXLOC+rand()%2-1;
		y=P_AYLOC+rand()%2-1;
		if(ONMAP(x,y)&&(is_habitable(x,y)) 
		&&(land_reachp(P_AXLOC,P_AYLOC,x,y,P_AMOVE,country))){
			P_AXLOC=x;
			P_AYLOC=y;
			/*if owned & unoccupied you take & people flee*/
			if( ((sct[x][y].owner == 0)
			|| (solds_in_sector( x, y, sct[x][y].owner) == 0))
			&& (ntn[sct[x][y].owner].active != NPC_BARBARIAN)) {
				fprintf(fnews,"3.\tbarbarians capture sector %d,%d\n",x,y);
				if(P_ATYPE<MINLEADER) {
					if(sct[x][y].owner!=0) flee(x,y,1,FALSE);
					sct[x][y].owner=country;
				}
				DEVASTATE(x,y);
			}
		}
	}
	/* place a few new Barbarian armies */
	for(armynum=0;armynum<MAXARM;armynum++) if(P_ASOLD<=0){
		x=(rand()%(MAPX-8))+4;
		y=(rand()%(MAPY-8))+4;
		if((rand()%4!=0) 
		||( sct[x][y].altitude == PEAK)
		||( sct[x][y].altitude == WATER))
			continue;
		if(sct[x][y].owner==0 || sct[x][y].owner==country 
		||(sct[x][y].people< 50)) {
			P_AXLOC=x;
			P_AYLOC=y;
			P_ASOLD=100+100*(rand()%3);
			P_ATYPE=defaultunit(country);
			P_ASTAT=ATTACK;
		}
	}
}

void
do_pirate()
{
	short nvynum,shipsize;
	int x, y, campx, campy;

	printf("updating pirate (nation %d)\n",country);

	/* if pirate fleet within 3 attack if outnumber any fleets */
	/* automatically find their base first */
	for(nvynum=0;nvynum<MAXNAVY;nvynum++) if(P_NWSHP!=0) {
		int temp=TRUE;

		campx=P_NXLOC; campy=P_NYLOC;
		for(x=P_NXLOC-PRTZONE;x<=P_NXLOC+PRTZONE;x++)
		for(y=P_NYLOC-PRTZONE;y<=P_NYLOC+PRTZONE;y++) {
			if((ONMAP(x,y)) && sct[x][y].designation==DBASECAMP ) {
				temp=FALSE;
				campx=x;
				campy=y;
			}
		}
		if(temp==TRUE) {
			fprintf(stderr,"Pirate fleet %d away from base\n",nvynum);
		} else {
			if (sct[campx][campy].designation!=DBASECAMP) {
				fprintf(stderr,"BASECAMP NOT FOUND!!!\n");
			} else {
				P_NXLOC=campx;
				P_NYLOC=campy;
			}
		}
	}
		
	for(nvynum=0;nvynum<MAXNAVY;nvynum++) if (P_NWSHP!=0) {
		for(x=1;x<NTOTAL;x++) if(isntn(ntn[x].active))
		for(y=0;y<MAXNAVY;y++)
		if(ntn[x].nvy[y].warships!=0 || ntn[x].nvy[y].merchant!=0 
		  || ntn[x].nvy[y].galleys!=0) {
			if((abs(ntn[x].nvy[y].xloc-P_NXLOC)<=PRTZONE)
			&&(abs(ntn[x].nvy[y].yloc-P_NYLOC)<=PRTZONE)) {
				P_NXLOC= ntn[x].nvy[y].xloc;
				P_NYLOC= ntn[x].nvy[y].yloc;
			}
		}
		if(rand()%15==0) {
			/*randomly add one warship to pirate fleet*/
			shipsize = rand()%(N_HEAVY-N_LIGHT+1);
			(void) NADD_WAR(1);
		}
	}
}
#endif MONSTER

#ifdef NPC
void
n_redes(x,y,goldthresh,metalthresh,citythresh,hunger)
int	x,y,goldthresh,metalthresh,citythresh;
float	hunger;
{
	register struct s_sector	*sptr = &sct[x][y];

	if((sptr->designation == DCAPITOL)
	||(sptr->designation == DCITY)) return;

	/*large enough for a city now?*/
	if(((sptr->people > (spread.civilians/CITYLIMIT))
	||((spread.civilians<30000)&&(sptr->people>1000)))
	&&( hunger > P_EATRATE*1.5 )
	&&( spread.incity+spread.incap < spread.civilians * CITYPERCENT / 100)
	&&( spread.sectors > 10)
	&&( sptr->tradegood == TG_none )){
		sptr->designation=DTOWN;
		spread.incity+=sptr->people;
		spread.infarm-=sptr->people;
	}

	/* large enough for city and not enough food*/
	if((sptr->designation==DTOWN)
	&&( hunger < P_EATRATE)
	&&( tofood(sptr,sptr->owner) > citythresh )){
		sptr->designation=DFARM;
		spread.incity-=sptr->people;
		spread.infarm+=sptr->people;
	}

	if((sptr->designation==DTOWN)
	&&( spread.incity+spread.incap > spread.civilians * CITYPERCENT / 66)){
		sptr->designation=DFARM;
		spread.incity-=sptr->people;
		spread.infarm+=sptr->people;
	}

	/*what if it is not a city*/
	if((sptr->designation!=DTOWN)
	&&(sptr->designation!=DCITY)
	&&(sptr->designation!=DCAPITOL)){
		if(( sptr->tradegood != TG_none )
		&&( tg_ok( sptr->owner, sptr ))) {
			if(( metalthresh+goldthresh > 8 )
			||(( sptr->metal < metalthresh )
			  &&( sptr->metal != 0 ))
			||(( sptr->jewels < goldthresh )
			  &&( sptr->jewels != 0 ))) {
				sptr->designation = DFARM;
			} else	sptr->designation= *(tg_stype+sptr->tradegood);

			if(( sptr->metal < metalthresh )
			  &&( sptr->metal != 0 ))
				sptr->designation=DBLKSMITH;

			if(( sptr->designation== 'x' )
			||(( sptr->designation== DCITY )&&(sptr->people<1000)))
				sptr->designation=DFARM;
		} else if( tofood(sptr,sptr->owner) >= 4 ){
			sptr->designation=DFARM;
		} else	sptr->designation=DSTOCKADE;
	}
	if(( sptr->designation==DFARM)
	&&( hunger > P_EATRATE*1.5 )
	&&(  tofood(sptr,sptr->owner) <= 6 )){
		if(( rand()%2 == 0 )&&( curntn->mine_ability<30 ))
			sptr->designation=DBLKSMITH;
		else if(( rand()%2 == 0 )
		&&( sptr->people<100 )
		&&( curntn->spoilrate >15))
			sptr->designation=DGRANARY;
		else if(( rand()%2 == 0 )&&( curntn->popularity<50 ))
			sptr->designation=DCHURCH;
		else if( sptr->people>1000 )
			sptr->designation=DTOWN;
	}
}

void
redomil()
{
	short x,y,armynum,nvynum;
	int i, free, done;
	long militia=0l,ideal;
	long diff=0l;
	int ok;

	check();
	/* check out any ship crews */
	for(nvynum=1;nvynum<MAXNAVY;nvynum++) {
		/* definite cheat -- add some random */
		if((P_NMSHP!=0)||(P_NWSHP!=0)||(P_NGSHP != 0))
			if(rand()%2==0) P_NCREW = SHIPCREW;
	}
	check();
	for(armynum=1;armynum<MAXARM;armynum++) if(P_ASOLD>0){
		/* move army back if too far out */
		ok = 0;
		for(x=P_AXLOC-3;x<=P_AXLOC+3;x++)
			for(y=P_AYLOC-3;y<=P_AYLOC+3;y++)
				if((ONMAP(x,y))&&(sct[x][y].owner==country)) ok=1;
		if(ok==0){
			P_AXLOC=curntn->capx;
			P_AYLOC=curntn->capy;
		}

		/* count and verify militia */
		if(P_ATYPE==A_MILITIA) {
			/* eliminate invalid militia */
			if(((sct[P_AXLOC][P_AYLOC].designation!=DTOWN)
			&&(sct[P_AXLOC][P_AYLOC].designation!=DCAPITOL)
			&&(sct[P_AXLOC][P_AYLOC].designation!=DCITY))
			||(sct[P_AXLOC][P_AYLOC].owner!=country)){
#ifdef DEBUG
				printf("\teliminating %s army %d as %d %d is des:%c alt:%c own:%d\n",unittype[P_ATYPE],armynum,P_AXLOC,P_AYLOC,sct[P_AXLOC][P_AYLOC].designation,sct[P_AXLOC][P_AYLOC].altitude,sct[P_AXLOC][P_AYLOC].owner);
#endif DEBUG
				if(sct[P_AXLOC][P_AYLOC].owner == country)
					sct[P_AXLOC][P_AYLOC].people+=P_ASOLD;
				else sct[curntn->capx][curntn->capy].people+=P_ASOLD;
				P_ASOLD=0;
			} else militia+=P_ASOLD;
		}

		/* set default status */
		if(P_ASTAT<NUMSTATUS) switch(P_ASTAT) {
		case MILITIA:
		case SIEGED:
		case TRADED:
		case ONBOARD:
		case GENERAL:
			break;
		default:
			P_ASTAT=DEFEND;
			break;
		}
	}
	curntn->tmil -= militia;

	/*make sure enough men in army 0 -- garrison duty in capitol*/
	armynum=0;
	P_ASTAT=GARRISON;
	P_ATYPE=defaultunit(country);
	P_AXLOC=curntn->capx;
	P_AYLOC=curntn->capy;

	/*Ideally P_ASOLD[0]*MILINCAP=tmil*peace/10*/
	ideal = curntn->tmil * peace / (10L*MILINCAP);
	if(curntn->tgold < 0) ideal/=2L;
#ifdef DEBUG
	if(peace==8)
		printf("\t%s IS AT PEACE - garrison in cap is %ld, ideal is %ld\n",curntn->name,P_ASOLD,ideal);
	else if(peace==12)
		printf("\t%s IS AT WAR - garrison in cap is %d, ideal is %ld\n",curntn->name,P_ASOLD,ideal);
	else printf("error - incap is %d ideal is %ld\n",P_ASOLD,ideal);
#endif DEBUG

	/*MILRATIO ratio mil:civ for non player countries*/
	/*MILINCAP ratio (mil in cap):mil for NPCs*/
	check();

	if((P_ASOLD*10) < (9*ideal)){

	/*too few soldiers on garrison*/
	/*diff is number to change mil in cap (>0)*/
	if(curntn->tgold<0L) diff=0;
	else diff = (long) min(ideal-P_ASOLD,(int) (curntn->metals / *(u_enmetal + (P_ATYPE%UTYPE))));

	diff=(long) min((int) diff, sct[curntn->capx][curntn->capy].people/2L);

	if(curntn->tgold<0L || curntn->metals<0L) if(diff > 0L) diff=0;

#ifdef DEBUG
	printf("\tadding %d men to garrison (too few men on garrison)\n",diff);
#endif DEBUG

	sct[curntn->capx][curntn->capy].people-=diff;
	P_ASOLD+=diff;
	curntn->tciv-=diff;
	curntn->tmil+=diff;
	if(magic(country,WARRIOR)==1)	/* take WARRIOR power into account */
		curntn->tgold -= (diff * *(u_encost + (P_ATYPE%UTYPE))) / 2;
	else curntn->tgold-=diff* *(u_encost + (P_ATYPE%UTYPE));
	curntn->metals-=(diff* *(u_enmetal + (P_ATYPE%UTYPE)));
#ifdef DEBUG
	if(P_ASOLD < 0L) printf("error 2... P_ASOLD=%d <0\n",P_ASOLD);
#endif DEBUG
	}
	/*else split garrison army if 1.25* needed number*/
	else if(P_ASOLD *4L > 5L*ideal){
		/*diff here is a negative number*/
		diff=((4L*P_ASOLD)-(5L*ideal))/4L;
#ifdef DEBUG
		printf("\tsplit garrison of %d men\n",diff);
#endif DEBUG
		free=FALSE;
		P_ASOLD-=diff;
#ifdef DEBUG
		if(P_ASOLD < 0) printf("error... subtracting %d from %d\n",diff,P_ASOLD);
#endif DEBUG
		curntn->tmil-=diff;
		curntn->tciv+=diff;
		sct[curntn->capx][curntn->capy].people+=diff;
		/*I add back gold as armies get redone anyway*/
		curntn->metals += (diff* *(u_enmetal + (P_ATYPE%UTYPE)));
		if(magic(country,WARRIOR)==TRUE) { /* WARRIOR power */
			curntn->tgold+=(diff* *(u_encost + (P_ATYPE%UTYPE))) / 2;
		} else curntn->tgold+=diff* *(u_encost + (P_ATYPE%UTYPE));
	}
#ifdef DEBUG
	else printf("\tno action - P_ASOLD (%d) ~= ideal (%d)\n",P_ASOLD,ideal);
	printf("\tFinal Garrison Army %d (%s) type is %s men is %d\n",armynum,curntn->name,*(unittype+(P_ATYPE)),P_ASOLD);
#endif DEBUG

	/*build ships and/or armies*/
	done=FALSE;
	ideal = curntn->tciv * peace / (10 * MILRATIO);
	if(curntn->tgold<0) { ideal*=4; ideal/=5; }
#ifdef DEBUG
	printf("\t%s total military is %d -> ideal is %d\n",curntn->name,curntn->tmil,ideal);
#endif DEBUG
	check();

	/* find leader and place on RULE in capitol */
	for(armynum=0;armynum<MAXARM;armynum++)
		if (P_ATYPE==getleader(curntn->class)-1) {
			P_ASTAT=RULE;
			P_AXLOC=curntn->capx;
			P_AYLOC=curntn->capy;
			break;
		}

	/* add to partial armies */
	for(armynum=1;armynum<MAXARM;armynum++)
	if((P_ASOLD>0)
	&&( P_ATYPE!=A_MILITIA )
	&&( P_ATYPE<MINLEADER )
	&&( P_ASOLD < TAKESECTOR )
	&&( curntn->tgold > 0 )
	&&( fort_val(&sct[P_AXLOC][P_AYLOC]) > 0)
	&&( sct[P_AXLOC][P_AYLOC].owner == country )) {
#ifdef DEBUG
		printf("\tadding %d men to weakened army %d\n",TAKESECTOR+20-P_ASOLD,armynum);
#endif DEBUG
		if(magic(country,WARRIOR)==TRUE) /* WARRIOR power */
		curntn->tgold-=((TAKESECTOR+20-P_ASOLD)*
			*(u_encost + (P_ATYPE%UTYPE))) / 2;
		else curntn->tgold-=(TAKESECTOR+20-P_ASOLD)*
			*(u_encost + (P_ATYPE%UTYPE));
		curntn->tmil += TAKESECTOR+20-P_ASOLD;
		P_ASOLD = TAKESECTOR+20;
	}

	/*if < ideal build new army in the capitol - if possible*/
	if(curntn->tmil < ((4*ideal)/5)) {
		for(armynum=1;armynum<MAXARM;armynum++)
		if((done==FALSE)&&(P_ASOLD==0)) {
			done=TRUE;
			P_ATYPE = defaultunit(country);
			P_ASOLD = min ((int) (ideal-curntn->tmil), (int) (curntn->metals/ (*(u_enmetal + (P_ATYPE%UTYPE)))));

			P_ASOLD = min (P_ASOLD,sct[curntn->capx][curntn->capy].people/2);
			P_ASOLD = min (P_ASOLD, (int) (curntn->tgold/ *(u_encost+(P_ATYPE%UTYPE))));
			if(P_ASOLD>0){
#ifdef DEBUG
				printf("\tnot enough soldiers - build new army %d with %d men\n",armynum,P_ASOLD);
#endif DEBUG
				curntn->metals-=(P_ASOLD* *(u_enmetal + (P_ATYPE%UTYPE)));
				P_AXLOC= curntn->capx;
				P_AYLOC= curntn->capy;
				curntn->tmil += P_ASOLD;
				curntn->tciv -= P_ASOLD;
				if(magic(country,WARRIOR)==TRUE) /* WARRIOR power */
					curntn->tgold-=(P_ASOLD* *(u_encost + (P_ATYPE%UTYPE))) / 2;
				else curntn->tgold-=P_ASOLD* *(u_encost + (P_ATYPE%UTYPE));
				sct[P_AXLOC][P_AYLOC].people-=P_ASOLD;
				P_ASTAT= DEFEND;
				P_AMOVE=0;
			}
			else P_ASOLD=0;
		}
		check();
	} else if(curntn->tmil > (6*ideal/5)){
		check();
		/*disband a pseudo-random army*/
		done=FALSE;
		diff=curntn->tmil-(6*ideal/5);
		for(armynum=1;done==FALSE && armynum<MAXARM;armynum++){
			if((P_ASOLD<=0)
			||(P_ATYPE==A_MILITIA)
			||(P_ATYPE>=MINLEADER)
			||(P_ASTAT==ONBOARD)
			||(P_ASTAT==TRADED)) continue;

			if((sct[P_AXLOC][P_AYLOC].owner==country)
			&&((sct[P_AXLOC][P_AYLOC].jewels>4)
			  ||(sct[P_AXLOC][P_AYLOC].metal>4)
			  ||(ISCITY(sct[P_AXLOC][P_AYLOC].designation)))){
#ifdef DEBUG
				printf("\ttoo many soldiers eliminate army %d (%d men)\n",armynum,P_ASOLD);
#endif DEBUG
				diff-=P_ASOLD;
				sct[P_AXLOC][P_AYLOC].people+=P_ASOLD;
				curntn->tmil -= P_ASOLD;
				curntn->tciv += P_ASOLD;
				P_ASOLD=0;
				if(diff<=50) done=TRUE;
			}
		}
	}
	check();
#ifdef DEBUG
	printf("\twhew... new tmil is %d\n",curntn->tmil);
#endif DEBUG

	/*resize armies */
	for(armynum=1;armynum<MAXARM;armynum++) if(P_ATYPE < MINLEADER) {
		/*maximum npc army is 3*TAKESECTOR or 3*tmil/MAXARM */
		/* also let militia get big */
		if((P_ASOLD>(2*TAKESECTOR))
		&&(P_ATYPE!=A_MILITIA && P_ASTAT!=ONBOARD && P_ASTAT!=TRADED)) {
			free=FALSE;
			for(i=1;free==FALSE && i<MAXARM;i++){
				if(curntn->arm[i].sold==0){
					free=TRUE;
					P_ASOLD/=2;
#ifdef DEBUG
	printf("\tSplitting %ld troops from army %d forming %s army %d \n"
		,P_ASOLD,armynum,unittype[P_ATYPE],i);
#endif DEBUG
					curntn->arm[i].sold  = P_ASOLD;
					curntn->arm[i].unittyp = P_ATYPE;
					curntn->arm[i].smove = P_AMOVE;
					curntn->arm[i].stat  = P_ASTAT;
					curntn->arm[i].xloc  = P_AXLOC;
					curntn->arm[i].yloc  = P_AYLOC;
				}
			}
		}
	/*minimum npc army is TAKESECTOR, merge them*/
		else if(P_ASOLD>0 && (P_ASOLD<TAKESECTOR || P_ASTAT==MILITIA)
		&& P_ASTAT!=ONBOARD && P_ASTAT!=TRADED) {
			free=FALSE;
			for(i=1;free==FALSE && i<MAXARM;i++){
				if((curntn->arm[i].sold>0)
				&&(curntn->arm[i].stat!=ONBOARD)
				&&(i!=armynum) 	/* don't use same army */
						/* or it will be deleted */
				&&(curntn->arm[i].stat!=TRADED)
				&&(curntn->arm[i].xloc==P_AXLOC)
				&&(curntn->arm[i].yloc==P_AYLOC)
				&&(curntn->arm[i].unittyp==P_ATYPE)) {
					free=TRUE;
					curntn->arm[i].sold += P_ASOLD;
#ifdef DEBUG
	printf("\tMerge %ld men from army %d to make %ld troops in %s army %d \n"
		,P_ASOLD,armynum,curntn->arm[i].sold,unittype[P_ATYPE],i);
#endif DEBUG
					P_ASOLD=0;
				}

			}
		}
	}
	check();

	/* assure that a militia unit resides in each city */
	if(curntn->tgold > 0)
	for(x=stx;x<endx;x++) for(y=sty;y<endy;y++)
	if((sct[x][y].owner==country)
	&&((sct[x][y].designation==DTOWN)||(sct[x][y].designation==DCITY)||(sct[x][y].designation==DCAPITOL))){
		free=FALSE;
		for(armynum=0;armynum<MAXARM;armynum++){
			if((P_ASOLD>0)
			&&(P_AXLOC==x)&&(P_AYLOC==y)
			&&(P_ATYPE==A_MILITIA)) {
				free=TRUE;
				break;
			}
		}
		if(free==FALSE) {	 /* draft new militia army */
			for(armynum=0;armynum<MAXARM;armynum++) if(P_ASOLD==0){
				P_AXLOC=x;
				P_AYLOC=y;
				free=TRUE;
				break;
			}
		}
		if((free==TRUE)) {
			/* want to add ideal troops */
			ideal = sct[x][y].people/MILINCITY - P_ASOLD;
			ideal = min(ideal, 250);
			if (P_ASOLD < 50)	/* make the militia at least 50 */
				ideal = max(ideal,50-P_ASOLD);
			if (P_ASOLD + ideal < 50) /*don't let ideal bring it below 50*/
				continue;
			if(ideal>0){
			if(magic(country,WARRIOR)==TRUE){ /* WARRIOR power */
				curntn->tgold-=
					(ideal* *(u_encost+P_ATYPE))/2;
			} else {
				curntn->tgold-=
					ideal* *(u_encost + P_ATYPE);
			}
			}
			P_ASOLD+=ideal;
			P_ATYPE=A_MILITIA;
			P_ASTAT=MILITIA;
#ifdef DEBUG
		printf("\tadding %ld troops to %s army %d (now %ld men - populace %ld)\n",ideal,unittype[P_ATYPE],armynum,P_ASOLD,sct[x][y].people);
#endif DEBUG
		}
	}
	check();

  	/* setup default units */
  	for(armynum=1;armynum<MAXARM;armynum++) 
	if((P_ASOLD>0)&&(P_ATYPE!=A_MILITIA)&&(P_ATYPE<MINLEADER)) 
		P_ATYPE=defaultunit(country);
}

/* getdstatus() - do diplomacy for current nation */
void
getdstatus()
{
	int x,oldstat[NTOTAL];
	int X,Y;
	int svhostile,hostile;	/* chance nation will react hostile */
	int friendly;		/* chance nation will react favorably */

	if(!isnpc(curntn->active)) return;

	if(( curntn->active==GOOD_6FREE ) 
	     ||( curntn->active==ISOLATIONIST )
	     ||( curntn->active==NEUTRAL_6FREE )
	     ||( curntn->active==EVIL_6FREE )) svhostile=5;
	else if(( curntn->active==GOOD_4FREE ) 
	     ||( curntn->active==NEUTRAL_4FREE )
	     ||( curntn->active==EVIL_4FREE )) svhostile=10;
	else if(( curntn->active==GOOD_2FREE ) 
	     ||( curntn->active==NEUTRAL_2FREE )
	     ||( curntn->active==EVIL_2FREE )) svhostile=20;
	else if(( curntn->active==GOOD_0FREE ) 
	     ||( curntn->active==NEUTRAL_0FREE )
	     ||( curntn->active==EVIL_0FREE )) svhostile=35;
	else svhostile=5;

	for(x=1;x<NTOTAL;x++) if( isntn(ntn[x].active) ){
		hostile = svhostile;
		if(npctype(curntn->active) != npctype(ntn[x].active)) 
			hostile+=20;	/* not same allignment */
		friendly = 60-hostile;
	     	if( curntn->active==ISOLATIONIST ) friendly -= 20;
		/* negate impact of above line on neutrals */
		if(isneutral(ntn[x].active)) {
			friendly-=10;
			hostile-=10;
		}
		if(ntn[x].race==curntn->race){
			friendly+=10;
			hostile-=10;
		}
		if(isneutral(curntn->active)) {
			friendly-=20;
			hostile-=20;
		}
		/* if next to capitol, they dont like you */
		for(X=curntn->capx-1;X<=curntn->capx+1;X++)
		for(Y=curntn->capy-1;Y<=curntn->capy+1;Y++) if(ONMAP(X,Y)) {
			if(sct[X][Y].owner == x) {
				friendly-=10;
				hostile +=10;
			}
		}

		if( friendly < 0 )	friendly=0;
		if( hostile < 0 )	hostile=0;

		oldstat[x] = curntn->dstatus[x];

		/* break bad treaties */
		if(curntn->dstatus[x] == TREATY) {
			if(ntn[x].dstatus[country]>=WAR) 
				curntn->dstatus[x] = JIHAD;
			continue;
		}
		
		if((curntn->dstatus[x] == JIHAD)
		||(curntn->dstatus[x]==UNMET)
		||(ispc(curntn->active)))
			continue;

		/*if 4* mil and 4* score then not like them*/
		if((ntn[x].tmil>4*curntn->tmil)
		&&(ntn[x].score>4*curntn->score)){
			if(curntn->dstatus[x]<WAR){
				if(rand()%100<=hostile)
					curntn->dstatus[x]++;
			}
		}
		/*adjust based on your status with them*/
		if((curntn->dstatus[x]==WAR)
		&&(ntn[x].dstatus[country]<WAR))
			if(rand()%100<=friendly) curntn->dstatus[x]--;

		if((curntn->dstatus[x]<WAR)
		&&(curntn->dstatus[x]>ALLIED)){
			if(ntn[x].dstatus[country]>1+curntn->dstatus[x]){
				if(rand()%100<=hostile)
					curntn->dstatus[x]++;
			} else if(ntn[x].dstatus[country]+1<curntn->dstatus[x]){
				if(rand()%100<=friendly)
					curntn->dstatus[x]--;
			}
		}
		if(rand()%100<= hostile) {
			if((curntn->dstatus[x]!=JIHAD)
			&&(curntn->dstatus[x]!=TREATY))
				curntn->dstatus[x]++;
		}
		if((rand()%100<= friendly)
		&&(curntn->dstatus[x]!=TREATY)
		&&(curntn->dstatus[x]!=JIHAD)
		&&(curntn->dstatus[x]!=WAR)) curntn->dstatus[x]--;
	}

	for(x=1;x<NTOTAL;x++) if(isntn( ntn[x].active ) ){
		if((rand()%5==0)
		&&(ntn[x].dstatus[country]==WAR)
		&&(curntn->dstatus[x]==WAR)) {
			ntn[x].dstatus[country]=HOSTILE;
			curntn->dstatus[x]=HOSTILE;
			fprintf(fnews,"2.\tnation %s and %s announce ceasefire\n",curntn->name,ntn[x].name);
			if( isnotpc(ntn[x].active) ) continue;
			mailopen(x);
			fprintf(fm,"nation %s and you negotiate a ceasefire\n",curntn->name);
			mailclose();
		} else if((oldstat[x]==WAR)&&(curntn->dstatus[x]==WAR)){
			fprintf(fnews,"2.\tnation %s stays at war with %s\n",curntn->name,ntn[x].name);
		} else if((oldstat[x]<WAR)&&(curntn->dstatus[x]==WAR)){
			fprintf(fnews,"2.\tnation %s goes to war with %s\n",curntn->name,ntn[x].name);
			if( isnotpc(ntn[x].active) ) continue;
			mailopen(x);
			fprintf(fm,"nation %s goes to war with you\n",curntn->name);
			mailclose();
		} else if((oldstat[x]!=JIHAD)&&(curntn->dstatus[x]==JIHAD)){
			fprintf(fnews,"2.\tnation %s announces a jihad with %s\n",curntn->name,ntn[x].name);
			if( isnotpc(ntn[x].active) ) continue;
			mailopen(x);
			fprintf(fm,"nation %s announces a jihad with you\n",curntn->name);
			mailclose();
		}
	}
}

void
nationrun()
{
	int goldthresh,metalthresh,citythresh,useful;
	int armynum,loop;
	int x,y,i,p;
	float	hunger;
	long zz;
	check();
	prep(country,FALSE);

	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) attr[x][y]=0;

	/* is there an error*/
	if((sct[curntn->capx][curntn->capy].owner==country)
	&&(sct[curntn->capx][curntn->capy].designation!=DCAPITOL)){
		sct[curntn->capx][curntn->capy].designation=DCAPITOL;
	}

	if( ispc( curntn->active )) {
		stx=sty=0;
		endx=MAPX;
		endy=MAPY;
	} else {
		if( curntn->capx > NPCTOOFAR )
			stx=curntn->capx-NPCTOOFAR;
		else	stx=0;
		if( curntn->capy > NPCTOOFAR )
			sty=curntn->capy-NPCTOOFAR;
		else	sty=0;
		if( curntn->capx + NPCTOOFAR < MAPX )
			endx=curntn->capx+NPCTOOFAR;
		else	endx=MAPX;
		if( curntn->capy + NPCTOOFAR < MAPY )
			endy=curntn->capy+NPCTOOFAR;
		else	endy=MAPY;
	}

	getdstatus();

#ifdef SPEW
	for(x=1;x<NTOTAL;x++) if(isntn( ntn[x].active )) {
		/* here is the bit which will occasionally send a randomly
		   generated message from a hostile NPC to a PC (25% chance) */
		if((curntn->dstatus[x] >= HOSTILE) 
		&& (ispc(ntn[x].active))) 
		if (rand()%4 == 0) {	/* send the message!! */
			printf("Sent message to %s\n",ntn[x].name);
			mailopen(x);
			fprintf(fm,"Message to %s from %s (%s of year %d)\n\n"
		 	,ntn[x].name,curntn->name,PSEASON(TURN),YEAR(TURN));
			makemess(rand()%5 +1,fm);
			mailclose();
		}
	}
#endif SPEW
	/*move units */
	/*are they at war with any normal countries*/
	peace=0;
	for(i=1;i<NTOTAL;i++) 
	if(isntn(ntn[i].active)&&(curntn->dstatus[i]>peace)) {
		peace=curntn->dstatus[i];
		if( peace>= WAR) break;
	}

	if(peace<WAR){
		peace=8;
		pceattr();
	} else {
	/*if war then attack &/or expand */
		peace=12;
		/*are they attacking or defending */
		if(curntn->tmil==0) defattr();
		else for(x=0;x<NTOTAL;x++) 
		if(isntn( ntn[x].active ) && (curntn->dstatus[x]>HOSTILE)){
			if(100*(curntn->tmil*(curntn->aplus+100))/((curntn->tmil*(curntn->aplus+100))+(ntn[x].tmil*(ntn[x].dplus+100)))>rand()%100){
				/*attacker*/
				for(armynum=1;armynum<MAXARM;armynum++)
					if((P_ASOLD>0)&&(P_ATYPE!=A_MILITIA)
					&&(P_ASTAT!=ONBOARD)&&(P_ASTAT!=TRADED)
					&&(P_ASTAT<NUMSTATUS)&&(P_ASTAT!=GENERAL))
						P_ASTAT=ATTACK;
				atkattr();
			} else {	/*defender*/
				for(armynum=1;armynum<MAXARM;armynum++)
					if((P_ASOLD>0)&&(P_ATYPE!=A_MILITIA)
					&&(P_ASTAT!=ONBOARD)&&(P_ASTAT!=TRADED)
					&&(P_ASTAT<NUMSTATUS)&&(P_ASTAT!=GENERAL)){
						if(P_ASOLD<350) P_ASTAT=DEFEND;
						else P_ASTAT=ATTACK;
					}
				defattr();
			}
		}
	}
	check();

	/* move infantry then leader/monsters */
	n_people(TRUE);			/* add to attr for people */

	if( country < 5 ) prtattr();

	loop=0;
	for(armynum=1;armynum<MAXARM;armynum++)
		if((P_ASOLD!=0)&&(P_ATYPE<MINLEADER)) loop+=armymove(armynum);
	n_people(FALSE);		/* subtract to attr for people */
	for(armynum=1;armynum<MAXARM;armynum++)
		if((P_ASOLD!=0)&&(P_ATYPE>=MINLEADER)) loop+=armymove(armynum);

	/* NPC ACTIVE STATUS CHANGE */
	if(isnpc(curntn->active)
	&&(curntn->active != ISOLATIONIST)) {
		if(isgood(curntn->active)) {
			if(loop<=1)	curntn->active=GOOD_0FREE;
			else if(loop>=6) curntn->active=GOOD_6FREE;
			else if(loop>=4) curntn->active=GOOD_4FREE;
			else	curntn->active=GOOD_2FREE;
		} else if(isneutral(curntn->active)) {
			if(loop<=1)	curntn->active=NEUTRAL_0FREE;
			else if(loop>=6) curntn->active=NEUTRAL_6FREE;
			else if(loop>=4) curntn->active=NEUTRAL_4FREE;
			else	curntn->active=NEUTRAL_2FREE;
		} else if(isevil(curntn->active)) {
			if(loop<=1)	curntn->active=EVIL_0FREE;
			else if(loop>=6) curntn->active=EVIL_6FREE;
			else if(loop>=4) curntn->active=EVIL_4FREE;
			else	curntn->active=EVIL_2FREE;
		}
	}

	if( curntn->tgold > curntn->tciv ) curntn->charity=10;
	else curntn->charity=0;

	/* INTELLIGENT SECTOR REDESIGNATION */
	/* note that only redesignate pc's if not designated yet */
	goldthresh=4;
	metalthresh=4;
	citythresh=10;
	hunger = 5.0;
	for(loop=1;loop<5;loop++) {
#ifdef DEBUG
		printf("\tnpcredes(): country %s gold=%d metal=%d, city=%d hunger=%f\n",curntn->name,goldthresh,metalthresh,citythresh,hunger);
#endif DEBUG

		useful=FALSE;
		for(x=stx;x<endx;x++) for(y=sty;y<endy;y++)
		if((sct[x][y].owner==country)
		&&(is_habitable(x,y))
		&&((isnotpc(curntn->active)) 
		  ||(sct[x][y].designation==DNODESIG))) {
			n_redes(x,y,goldthresh,metalthresh,citythresh,hunger);
			useful=TRUE;
		}
		if(useful==FALSE) break;

		spreadsheet(country);

		hunger = spread.food/((float)(spread.civilians+2*curntn->tmil));
		if(hunger < P_EATRATE ) {
			goldthresh++;
			metalthresh++;
			citythresh--;
		} else if(hunger > 2*P_EATRATE) {
			if( goldthresh==1 && metalthresh==1 ) break;
			goldthresh-=2;
			metalthresh-=2;
			citythresh+=2;
		} else {
			if( goldthresh==1 && metalthresh==1 ) break;
			if(rand()%2==0) goldthresh--;
			else metalthresh--;
			if(goldthresh==0) goldthresh=1;
			if(metalthresh==0) metalthresh=1;
			citythresh++;
		}
		if(goldthresh<=0) goldthresh=1;
		if(metalthresh<=0) metalthresh=1;
	}

	/*build forts in any cities*/
	if(isnotpc(curntn->active)) for(x=stx;x<endx;x++) for(y=sty;y<endy;y++)
		if((sct[x][y].owner==country)&&
			((sct[x][y].designation==DTOWN)
			||(sct[x][y].designation==DCITY)
			||(sct[x][y].designation==DCAPITOL)
			||(sct[x][y].designation==DFORT))
			&&(sct[x][y].fortress<10)
			&&(curntn->tgold>10000)
			&&(rand()%5==0)
			&&(sct[x][y].fortress<(sct[x][y].people%1000)))
				sct[x][y].fortress++;
	check();
	/*redo mil*/
	if(isnotpc(curntn->active)) redomil();
	check();

	/*buy new powers and/or new weapons*/
	if(getmgkcost(M_MIL,country) < getmgkcost(M_CIV,country)){
		if(curntn->jewels > getmgkcost(M_MIL,country)) {
			curntn->jewels-=getmgkcost(M_MIL,country);
			if((zz=getmagic(M_MIL))!=0){
				for(p=S_MIL;p<E_MIL;p++) if(powers[p]==zz){
					fprintf(fnews,"1.\tnation %s gets combat power %s\n",curntn->name,pwrname[p]);
					printf("\tnation %s gets combat power %s\n",curntn->name,pwrname[p]);
					break;
				}
				exenewmgk(zz);
			} else if((zz=getmagic(M_MIL))!=0){
				for(p=S_MIL;p<E_MIL;p++) if(powers[p]==zz){
					fprintf(fnews,"1.\tnation %s gets combat power %s\n",curntn->name,pwrname[p]);
					printf("\tnation %s gets combat power %s\n",curntn->name,pwrname[p]);
					break;
				}
				exenewmgk(zz);
			}
			else	curntn->jewels+=getmgkcost(M_MIL,country);
		}
	} else {
		if(curntn->jewels > getmgkcost(M_CIV,country)) {
			curntn->jewels-=getmgkcost(M_CIV,country);
			if((zz=getmagic(M_CIV))!=0){
				for(p=S_CIV;p<S_CIV+E_CIV;p++) if(powers[p]==zz){
					fprintf(fnews,"1.\tnation %s gets civilian power %s\n",curntn->name,pwrname[p]);
					printf("\tnation %s gets civilian power %s\n",curntn->name,pwrname[p]);
					break;
				}
				exenewmgk(zz);
			}
			else if((zz=getmagic(M_CIV))!=0){
				for(p=S_CIV;p<S_CIV+E_CIV;p++) if(powers[p]==zz){
					fprintf(fnews,"1.\tnation %s gets civilian power %s\n",curntn->name,pwrname[p]);
					printf("\tnation %s gets civilian power %s\n",curntn->name,pwrname[p]);
					break;
				}
				exenewmgk(zz);
			}
			else	curntn->jewels+=getmgkcost(M_CIV,country);
		}
	}

	check();
	if(magic(country,VAMPIRE)!=TRUE) {
		i=0;
		if(magic(country,WARLORD)==TRUE) i=30;
		else if(magic(country,CAPTAIN)==TRUE) i=20;
		else if(magic(country,WARRIOR)==TRUE) i=10;
		x = max ( curntn->aplus-i, 10 ) / 10;
		x *= x;
		if( curntn->race==ORC) x*=2;
		/* SHOULD USE spread.metal but it didnt work right */
		if(rand()%2==0)
		if(curntn->metals >  3 * METALORE * curntn->tmil*x){
			curntn->aplus+=1;
			curntn->metals-=METALORE*curntn->tmil*x;
			printf("\tnation %s buys +1 percent attack\n",curntn->name);
		}
		x = max ( curntn->dplus-i, 10 ) / 10;
		x *= x;
		if( curntn->race==ORC) x*=2;
		if(curntn->metals >  3 * METALORE * curntn->tmil*x){
			curntn->dplus+=1;
			curntn->metals-=METALORE*curntn->tmil*x;
			printf("\tnation %s buys +1 percent defence\n",curntn->name);
		}
	}
	/* don't allow status ATTACK from own city */
	for(armynum=0;armynum<MAXARM;armynum++) {
		if (P_ASOLD<=0 || P_ASTAT!=ATTACK) continue;
		if ((sct[P_AXLOC][P_AYLOC].owner==country)
		&& (fort_val(&sct[P_AXLOC][P_AYLOC]) > 0)) {
			if(rand()%2==0) P_ASTAT=DEFEND;
			else P_ASTAT=GARRISON;
		}
	}
}

/* dont allow npcs to trespass onto other nations land */
void
n_trespass()
{
	register int x,y;
	for(x=stx;x<endx;x++) for(y=sty;y<endy;y++)  {
		if((sct[x][y].owner != country )
		&&( sct[x][y].owner != 0 )
		&&( abs(x-curntn->capx)>2 )
		&&( abs(y-curntn->capy)>2 )
		&&( ntn[country].dstatus[sct[x][y].owner]<WAR)
		&&( ntn[sct[x][y].owner].dstatus[country]<WAR)
		&&( ntn[country].dstatus[sct[x][y].owner]>ALLIED))
			attr[x][y]=1;
	}
}

/* you are too far from capitol */
void
n_toofar()
{
	register int x,y;
	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) {
		if( x<stx || y<sty || x>=endx || y>=endy )
			attr[x][y]=1;
	}
}

/* take undefended land */
void
n_unowned()
{
	register int x,y;

	/* around capitol */
	for(x=curntn->capx-4;x<=curntn->capx+4;x++){
		for(y=curntn->capy-4;y<=curntn->capy+4;y++){
			if((ONMAP(x,y))&&(sct[x][y].owner==0)) {
				attr[x][y] += 700;
			}
		}
	}

	for(x=stx;x<endx;x++) {
		for(y=sty;y<endy;y++) {
			/* add if metal high */
			if(sct[x][y].tradegood != TG_none) {
				if(sct[x][y].metal != 0) attr[x][y]+=500;
				else if(sct[x][y].jewels != 0) attr[x][y]+=500;
				else attr[x][y]+=300;
			}
			/*add to attractiveness for unowned sectors*/
			if(sct[x][y].owner == 0) {
				attr[x][y]+=300;
			} else	{
			if(ntn[sct[x][y].owner].active == NPC_NOMAD)
				attr[x][y]+=100;
			}
			attr[x][y] += 50*tofood(&sct[x][y],country); 
			if(!is_habitable(x,y)) attr[x][y] /= 5;
		}
	}
}
void
n_defend(natn)
register short natn;
{
	int x,y;

	/* add 1/10th of their soldiers in sector */
	for(x=1;x<MAXARM;x++) if(ntn[natn].arm[x].sold > 0)
		if(sct[ntn[natn].arm[x].xloc][ntn[natn].arm[x].yloc].owner==country)
			attr[ntn[natn].arm[x].xloc][ntn[natn].arm[x].yloc] +=
				ntn[natn].arm[x].sold/10;

	/*plus 80 if near your capitol */
	for(x=curntn->capx-1;x<=curntn->capy+1;x++){
		for(y=curntn->capy-1;y<=curntn->capy+1;y++){
			if(ONMAP(x,y)) attr[x][y]+=80;
		}
	}

	/*plus based on defensive value and population */
	for(x=stx;x<endx;x++) for(y=sty;y<endy;y++) {
		if(movecost[x][y]==1) attr[x][y] += 50;
		else if(movecost[x][y]<=3) attr[x][y] += 20;
		else if(movecost[x][y]<=5) attr[x][y] += 10;

 		if(sct[x][y].owner==country){
			if(ISCITY(sct[x][y].designation))
				attr[x][y] += 50;
			/* should spread 3000 points over country */
			attr[x][y]+=3000*sct[x][y].people/ntn[country].tciv;
		}
	}
}

void
n_attack(nation)
register short nation;
{
	register int x,y;
	int	armynum;
	long	solds;	/* solds within 1 of capitol or city */

	for(x=stx;x<endx;x++) for(y=sty;y<endy;y++){
		if((sct[x][y].owner==nation)&&
		((sct[x][y].designation==DCITY)
		||(sct[x][y].designation==DCAPITOL)
		||(sct[x][y].designation==DTOWN))){
			solds=0;
			for(armynum=1;armynum<MAXARM;armynum++) 
				if((ntn[country].arm[armynum].sold > 0)
				&&(abs(AXLOC-x)<=1)
				&&(abs(AYLOC-y)<=1)) solds+=ASOLD;

			if(solds_in_sector(x,y,nation)*2 < 3*solds)
				attr[x][y]+=500;
		}
	}
}

/* +100 if undefended sectors of nation, +60 if not */
void
n_undefended( nation )
{
	register int x,y;
	for(x=stx;x<endx;x++) for(y=sty;y<endy;y++) if(sct[x][y].owner==nation){
		if(is_habitable(x,y) == 0) {
			attr[x][y] += 30;
		} else if(occ[x][y]==0) {
			attr[x][y]+=100;
		} else {
			attr[x][y]+=60;
		}
	}
}

/* add 1/2 of people in owned sectors for small armies */
void
n_people(doadd)
int doadd;	/* TRUE if adding, FALSE if subtracting */
{
	register int x,y;
	for(x=stx;x<endx;x++) for(y=sty;y<endy;y++)
	if(sct[x][y].owner==country){
		if(is_habitable(x,y)){
			if(doadd==TRUE) {
				attr[x][y] += sct[x][y].people/4;
			} else	attr[x][y] -= sct[x][y].people/4;
		}
	}
}

/* +60 if between two capitols */
void
n_between(nation)
int nation;
{
	int x1,x2,y1,y2,x,y;

	/*plus if strategic blocking sector*/

	/*+60 if between the two capitol*/
	if (ntn[nation].capx < curntn->capx){
		x1=ntn[nation].capx;
		x2=curntn->capx;
	} else {
		x1=curntn->capx;
		x2=ntn[nation].capx;
	}
	if (ntn[nation].capy < curntn->capy){
		y1=ntn[nation].capy;
		y2=curntn->capy;
	}
	else {
		y1=curntn->capy;
		y2=ntn[nation].capy;
	}

	for(x=x1;x<=x2;x++) for(y=y1;y<=y2;y++) {
		if(ONMAP(x,y)) attr[x][y]+=60;
	}
}

/* if in jeopardy, move to survive
 *	if within two of cap add 1/5th of men
 *	if on cap and war and 2x your garrison go jihad and + 1/2 men
 */
void
n_survive()
{
	int nation,armynum;
	int capx,capy;

	capx=curntn->capx;
	capy=curntn->capy;

	if(sct[capx][capy].owner!=country){
		attr[capx][capy]=1000;
	}

	/*defend your capitol if occupied, +50 more if with their army*/
	for(nation=1;nation<NTOTAL;nation++)
	if((isntn(ntn[nation].active))
	&&((ntn[nation].dstatus[country]>=WAR)
	  ||(curntn->dstatus[nation]>=WAR))){
		for(armynum=1;armynum<MAXARM;armynum++)
		if((ntn[nation].arm[armynum].sold > 0) 
		&&( ntn[nation].arm[armynum].xloc<=capx+2)
		&&( ntn[nation].arm[armynum].xloc>=capx-2)
		&&( ntn[nation].arm[armynum].yloc<=capy+2)
		&&( ntn[nation].arm[armynum].yloc>=capy-2)){
			if((ntn[nation].arm[armynum].xloc==capx)
			&&(ntn[nation].arm[armynum].yloc==capy)){
				attr[capx][capy]+=2*ntn[nation].arm[armynum].sold;
			} else {
				attr[ntn[nation].arm[armynum].xloc][ntn[nation].arm[armynum].yloc]+=ntn[nation].arm[armynum].sold;
			}
		}
	}
}

void
defattr()
{
	int nation;

#ifdef DEBUG
	printf("atkattr()\n");
#endif DEBUG

	n_unowned();

	for(nation=1;nation<NTOTAL;nation++)
	if((isntn(ntn[nation].active))&&(curntn->dstatus[nation]>=WAR)) {
		n_defend(nation);
		n_between(nation);
		n_undefended(nation);
	}

	n_trespass();
	n_toofar();
	n_survive();
}

/*calculate attractiveness of attacking sectors*/
void
atkattr()
{
	int nation;
#ifdef DEBUG
	printf("atkattr()\n");
#endif DEBUG

	n_unowned();

	/*adjust for each nation that you are at war with*/
	for(nation=1;nation<NTOTAL;nation++) if( isntn(ntn[nation].active) ){
		if(curntn->dstatus[nation]==WAR) {
			n_between(nation);
			n_undefended(nation);
			n_attack(nation);
		} else if(curntn->dstatus[nation]==JIHAD) {
			n_attack(nation);
			n_attack(nation);
			n_between(nation);
			n_undefended(nation);
			n_attack(nation);
			n_between(nation);
			n_undefended(nation);
			n_attack(nation);
		}
	}

	n_toofar();
	n_trespass();
	n_survive();
}

/*calculate attractiveness when at peace*/
void
pceattr()
{
#ifdef DEBUG
	printf("pceattr()\n");
#endif DEBUG
	n_unowned();
	n_unowned();
	n_trespass();
	n_toofar();
	n_survive();
}
#endif NPC
