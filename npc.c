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

extern short country;
extern int attr[MAPX][MAPY];     /*sector attactiveness*/
extern short movecost[MAPX][MAPY];
int peace;	/*is 8 if at peace, 12 if at war*/

/*ntn 1 is nation you are updating*/
void
newdip(ntn1,ntn2)
{
	if(ntn[ntn1].active<2) {
		if(ntn[ntn1].dstatus[ntn2]==UNMET)
			ntn[ntn1].dstatus[ntn2]=NEUTRAL;
	}
	else if(ntn[ntn1].race==ntn[ntn2].race){
		if(rand()%5<3) ntn[ntn1].dstatus[ntn2]=FRIENDLY;
		else if(rand()%2==0) ntn[ntn1].dstatus[ntn2]=ALLIED;
		else ntn[ntn1].dstatus[ntn2]=NEUTRAL;
	}
	else switch(ntn[ntn1].race) {
	case DWARF:
		switch(ntn[ntn2].race) {
		case ELF:
			ntn[ntn1].dstatus[ntn2]=HOSTILE;
			break;
		case HUMAN:
			ntn[ntn1].dstatus[ntn2]=NEUTRAL;
			break;
		case ORC:
		default:
			ntn[ntn1].dstatus[ntn2]=WAR;
			break;
		}
		break;
	case ELF:
		switch(ntn[ntn2].race) {
		case DWARF:
			ntn[ntn1].dstatus[ntn2]=HOSTILE;
			break;
		case HUMAN:
			ntn[ntn1].dstatus[ntn2]=NEUTRAL;
			break;
		case ORC:
		default:
			ntn[ntn1].dstatus[ntn2]=WAR;
			break;
		}
		break;
	case ORC:
		switch(ntn[ntn2].race) {
		case HUMAN:
			ntn[ntn1].dstatus[ntn2]=NEUTRAL;
			break;
		case DWARF:
		case ELF:
		default:
			ntn[ntn1].dstatus[ntn2]=WAR;
			break;
		}
		break;
	case HUMAN:
		switch(ntn[ntn2].race) {
		case DWARF:
		case ELF:
			ntn[ntn1].dstatus[ntn2]=NEUTRAL;
			break;
		case ORC:
			ntn[ntn1].dstatus[ntn2]=HOSTILE;
			break;
		default:
			ntn[ntn1].dstatus[ntn2]=WAR;
			break;
		}
		break;
	default:
		ntn[ntn1].dstatus[ntn2]=WAR;
		break;
	}
}

void
monster()
{
	short nvynum,armynum;
	int x, y;

	/*move nomads randomly until within 2 of city then attack*/
	country=NNOMAD;
	for(armynum=0;armynum<MAXARM;armynum++) if(ASOLD>0){
		ASOLD *= 11;
		ASOLD /= 10;
		x=AXLOC+rand()%4-2;
		y=AYLOC+rand()%4-2;
		if(ONMAP&&(is_habitable(x,y))
		&&(land_reachp(AXLOC,AYLOC,x,y,ntn[country].arm[armynum].smove,country))){
			AXLOC=x;
			AYLOC=y;
			/*if owned & unoccupied you take & people flee*/
			if( (sct[x][y].owner) == 0 || solds_in_sector( x, y, sct[x][y].owner) == 0 ) {
				if(sct[x][y].owner==0)
					fprintf(fnews,"3.\tnomads take sector %d,%d\n",x,y);
				else fprintf(fnews,"3.\tnomads capture sector %d,%d\n",x,y);
				if(sct[x][y].owner!=0) flee(x,y,1,FALSE);
				sct[x][y].owner=NNOMAD;
				sct[x][y].designation=DDEVASTATED;
			}
		}
	}
	else {
		/*place a new Nomad army*/
		x=(rand()%(MAPX-8))+4;
		y=(rand()%(MAPY-8))+4;
		if((rand()%4==0)&&(is_habitable(x,y))) {
			AXLOC=x;
			AYLOC=y;
			ASOLD=100+100*(rand()%10);
			ATYPE=A_INFANTRY;
			ASTAT=ATTACK;
		}
	}
	country=NLIZARD;
	for(armynum=0;armynum<MAXARM;armynum++) if(ASOLD>0){
		ASOLD*=105;
		ASOLD/=100;
	}
	country=NPIRATE;
	fprintf(fnews,"2.\t%s are updated\n",ntn[NPIRATE].name);
	/*if pirate fleet within 3 attack if outnumber any fleets */
	for(nvynum=0;nvynum<MAXNAVY;nvynum++){
		for(x=1;x<MAXNTN;x++) for(y=0;y<MAXNAVY;y++)
		if((ntn[x].nvy[y].xloc-NXLOC<2)
		&&(sct[ntn[x].nvy[y].xloc][ntn[x].nvy[y].yloc].altitude==WATER)
		&&(ntn[x].nvy[y].xloc-NXLOC>(-2))
		&&(ntn[x].nvy[y].yloc-NYLOC<2)
		&&(ntn[x].nvy[y].yloc-NYLOC>(-2))){
			NXLOC= ntn[x].nvy[y].xloc;
			NYLOC= ntn[x].nvy[y].yloc;
		}
		/*add one warship to random pirate fleet*/
		if((NWAR>0)&&(rand()%3==0)) {
			NWAR++;
			NCREW+=SHIPCREW;
		}
	}
}

#ifdef NPC

void
n_redes(x,y,goldthresh,ironthresh,citythresh)
int goldthresh,ironthresh,citythresh;
{
	register struct s_sector	*sptr = &sct[x][y];

	if(sptr->designation == DCAPITOL) return;

	/*large enough for a city now?*/
	if(((sptr->people > (spread.civilians/CITYLIMIT))
	||((spread.civilians<30000)&&(sptr->people>1000)))
	&&( spread.food > spread.civilians*3)
	&&( sptr->iron < ironthresh)
	&&( spread.incity+spread.incap < spread.civilians * CITYPERCENT / 100)
	&&( spread.sectors > 10)
	&&( sptr->gold < goldthresh)){
		sptr->designation=DCITY;
		spread.incity+=sptr->people;
		spread.infarm-=sptr->people;
	}

	/*not large enough for city and not enough food*/
	if((sptr->designation==DCITY)
	&&( spread.food < spread.civilians*2)
	&&( tofood(sptr->vegetation,sptr->owner) > citythresh )){
		sptr->designation=DFARM;
		spread.incity-=sptr->people;
		spread.infarm+=sptr->people;
	}

	if((sptr->designation==DCITY)
	&&( spread.incity+spread.incap > spread.civilians * CITYPERCENT / 66)){
		sptr->designation=DFARM;
		spread.incity-=sptr->people;
		spread.infarm+=sptr->people;
	}

	/*what if it is not a city*/
	if((sptr->designation!=DCITY)&&(sptr->designation!=DCAPITOL)){
		if(( sptr->gold >= goldthresh+2 )
		&&( sptr->iron < sptr->gold )){
			sptr->designation=DGOLDMINE;
		}
		else if( sptr->iron >= ironthresh ){
			sptr->designation=DMINE;
		}
		else if( sptr->gold >= goldthresh ){
			sptr->designation=DGOLDMINE;
		}
		else sptr->designation=DFARM;
	}
}

void
redomil()
{
	short x,y,armynum;
	int i, free, done, ideal, militia=0;
	long diff=0;
	int ok;

#ifdef DEBUG
	check(__LINE__);
#endif DEBUG
	/* check out any ship crews */
	for(armynum=1;armynum<MAXNAVY;armynum++) {
		if((ntn[country].nvy[armynum].merchant>0)
		|| (ntn[country].nvy[armynum].warships>0))
		ntn[country].nvy[armynum].crew =
		(ntn[country].nvy[armynum].merchant+ntn[country].nvy[armynum].warships)*SHIPCREW;
	}
#ifdef DEBUG
	check(__LINE__);
#endif DEBUG
	/* code to check and move armies back if they are too far out */
	for(armynum=1;armynum<MAXARM;armynum++) if(ASOLD>0){
		ok = 0;
		for(x=AXLOC-3;x<=AXLOC+3;x++)
			for(y=AYLOC-3;y<=AYLOC+3;y++)
				if((ONMAP)&&(sct[x][y].owner==country)) ok=1;
		if(ok==0){
			AXLOC=ntn[country].capx;
			AYLOC=ntn[country].capy;
		}
	}
	for(armynum=1;armynum<MAXARM;armynum++)
	if((ASOLD>0)&&(ATYPE==A_MILITIA)){
		militia+=ASOLD;
	}
	ntn[country].tmil -= militia;

	for(armynum=1;armynum<MAXARM;armynum++)
	if((ASOLD>0)&&(ATYPE==A_INFANTRY)) ASTAT = DEFEND;

	/*make sure enough men in army 0 -- garrison duty in capitol*/
	armynum=0;
	ASTAT=GARRISON;
	ATYPE=A_INFANTRY;
	AXLOC=ntn[country].capx;
	AYLOC=ntn[country].capy;

	/*Ideally ASOLD[0]*MILINCAP=tmil*peace/10*/
	ideal = ntn[country].tmil * peace / (10*MILINCAP);
	if(ntn[country].tgold < 0) ideal/=2;
#ifdef DEBUG
	if(peace==8)
		printf("\t%s IS AT PEACE - garrison in cap is %d, ideal is %d\n",ntn[country].name,ASOLD,ideal);
	else if(peace==12)
		printf("\t%s IS AT WAR - garrison in cap is %d, ideal is %d\n",ntn[country].name,ASOLD,ideal);
	else printf("error - incap is %d ideal is %d\n",ASOLD,ideal);
#endif DEBUG

	/*MILRATIO ratio mil:civ for non player countries*/
	/*MILINCAP ratio (mil in cap):mil for NPCs*/
#ifdef DEBUG
	check(__LINE__);
#endif DEBUG

	if((ASOLD*10) < (9*ideal)){

	/*too few soldiers on garrison*/
	/*diff is number to change mil in cap (>0)*/
	if(ntn[country].tgold<0L) diff=0;
	else diff = (long) min(ideal-ASOLD,(int) (ntn[country].tiron / *(u_eniron + (ATYPE%100))));

	diff = (long) min ((int) diff, sct[ntn[country].capx][ntn[country].capy].people/2);

	if(ntn[country].tgold<0 || ntn[country].tiron<0) if(diff > 0) diff=0;

#ifdef DEBUG
	printf("\tadding %d men to garrison (too few men on garrison)\n",diff);
#endif DEBUG

	sct[ntn[country].capx][ntn[country].capy].people-=diff;
	ASOLD+=diff;
	ntn[country].tciv-=diff;
	ntn[country].tmil+=diff;
	if(magic(country,WARRIOR)==1)	/* take WARRIOR power into account */
		ntn[country].tgold -= (diff * *(u_encost + (ATYPE%100))) / 2;
	else ntn[country].tgold-=diff* *(u_encost + (ATYPE%100));
	ntn[country].tiron-=(diff* *(u_eniron + (ATYPE%100)));
#ifdef DEBUG
	if(ASOLD < 0) printf("error 2... ASOLD=%d <0\n",ASOLD);
#endif DEBUG
	}
	/*else split garrison army if 1.25* needed number*/
	else if(ASOLD *4 > 5*ideal){
		/*diff here is a negative number*/
		diff=((4*ASOLD)-(5*ideal))/4;
#ifdef DEBUG
		printf("\tsplit garrison of %d men\n",diff);
#endif DEBUG
		free=0;
		ASOLD-=diff;
#ifdef DEBUG
		if(ASOLD < 0) printf("error... subtracting %d from %d\n",diff,ASOLD);
#endif DEBUG
		ntn[country].tmil-=diff;
		ntn[country].tciv+=diff;
		sct[ntn[country].capx][ntn[country].capy].people+=diff;
		/*I add back gold as armies get redone anyway*/
		ntn[country].tiron += (diff* *(u_eniron + (ATYPE%100)));
		if(magic(country,WARRIOR)==1) /* WARRIOR power */
			ntn[country].tgold+=(diff* *(u_encost + (ATYPE%100))) / 2;
		else ntn[country].tgold+=diff* *(u_encost + (ATYPE%100));
	}
#ifdef DEBUG
	else printf("\tno action - ASOLD (%d) ~= ideal (%d)\n",ASOLD,ideal);
	printf("\tFinal Garrison Army %d (%s) type is %s men is %d\n",armynum,ntn[country].name,*(unittype+(ATYPE)),ASOLD);
#endif DEBUG

	/*build ships and/or armies*/
	done=FALSE;
	ideal = ntn[country].tciv * peace / (10 * MILRATIO);
	if(ntn[country].tgold<0) { ideal*=4; ideal/=5; }
#ifdef DEBUG
	printf("\t%s total military is %d -> ideal is %d\n",ntn[country].name,ntn[country].tmil,ideal);
	check(__LINE__);
#endif DEBUG
	/*if < ideal build new army in the capitol - if possible*/
	if(ntn[country].tmil < ((4*ideal)/5)) {
		for(armynum=1;armynum<MAXARM;armynum++)
		if((done==FALSE)&&(ASOLD==0)) {
			done=TRUE;
			ATYPE=A_INFANTRY;
			ASOLD = min ((int) (ideal-ntn[country].tmil), (int) (ntn[country].tiron/ (*(u_eniron + (ATYPE%100)))));

			ASOLD = min (ASOLD,sct[ntn[country].capx][ntn[country].capy].people/2);
			ASOLD = min (ASOLD, (int) (ntn[country].tgold/ *(u_encost+(ATYPE%100))));
			if(ASOLD>0){
#ifdef DEBUG
printf("\tnot enough soldiers - build new army %d with %d men\n",armynum,ASOLD);
#endif DEBUG
			ntn[country].tiron-=(ASOLD* *(u_eniron + (ATYPE%100)));
			AXLOC= ntn[country].capx;
			AYLOC= ntn[country].capy;
			ntn[country].tmil += ASOLD;
			ntn[country].tciv -= ASOLD;
			if(magic(country,WARRIOR)==1) /* WARRIOR power */
				ntn[country].tgold-=(ASOLD* *(u_encost + (ATYPE%100))) / 2;
			else ntn[country].tgold-=ASOLD* *(u_encost + (ATYPE%100));
			sct[AXLOC][AYLOC].people-=ASOLD;
			ASTAT= DEFEND;
			AMOVE=0;
			}
			else ASOLD=0;
		}
#ifdef DEBUG
	check(__LINE__);
#endif DEBUG
	} else if(ntn[country].tmil > (6*ideal/5)){
#ifdef DEBUG
	check(__LINE__);
#endif DEBUG
		/*disband a pseudo-random army*/
		done=FALSE;
		diff=ntn[country].tmil-(6*ideal/5);
#ifdef DEBUG
		if(ASOLD < 0) printf("\ndisbanding pseudo random army of about %d men\n",diff);
#endif DEBUG
		for(armynum=1;armynum<MAXARM;armynum++){
			if((done==FALSE)
			&&(ASOLD>0)
			&&(ATYPE!=A_MILITIA)
			&&(sct[AXLOC][AYLOC].owner==country)
			&&((sct[AXLOC][AYLOC].gold>4)
				||(sct[AXLOC][AYLOC].iron>4)
				||(sct[AXLOC][AYLOC].designation==DCITY))
			&&(ASOLD<=diff)){
#ifdef DEBUG
				printf("\ttoo many soldiers eliminate army %d (%d men)\n",armynum,ASOLD);
#endif DEBUG
				diff-=ASOLD;
				sct[AXLOC][AYLOC].people+=ASOLD;
				ntn[country].tmil -= ASOLD;
				ntn[country].tciv += ASOLD;
				ASOLD=0;
				if(diff<=50) done=TRUE;
			}
#ifdef DEBUG
			if(ASOLD < 0) printf("error X... ASOLD=%d <0\n",ASOLD);
#endif DEBUG
		}
	}
#ifdef DEBUG
	check(__LINE__);
#endif DEBUG
#ifdef DEBUG
	printf("\twhew... new tmil is %d\n",ntn[country].tmil);
#endif DEBUG

	/*resize armies */
	/*maximum npc army is 200 or tmil/20, minimum is 50*/
	for(armynum=1;armynum<MAXARM;armynum++){
		if((ASOLD>200)&&(ASOLD>ntn[country].tmil/20)) {
			free=FALSE;
			for(i=1;i<MAXARM;i++){
				if((free==0)&&(ntn[country].arm[i].sold==0)){
					free=1;
					ASOLD/=2;
					ATYPE=A_INFANTRY;
					ntn[country].arm[i].sold  = ASOLD;
					ntn[country].arm[i].unittyp = A_INFANTRY;
					ntn[country].arm[i].smove = AMOVE;
					ntn[country].arm[i].stat  = DEFEND;
					ntn[country].arm[i].xloc  = AXLOC;
					ntn[country].arm[i].yloc  = AYLOC;
				}
			}
		}
	/*minimum npc army is 50, merge them*/
		else if(ASOLD<50) {
			free=0;
			for(i=1;i<MAXARM;i++){
				if((ntn[country].arm[i].sold>0)
				&&(ntn[country].arm[i].xloc==AXLOC)
				&&(ntn[country].arm[i].yloc==AYLOC)
				&&(ntn[country].arm[i].unittyp==ATYPE)
				&&(i!=armynum)&&(free==0)){
					free=1;
					ntn[country].arm[i].sold += ASOLD;
					ASOLD=0;
				}
			}
		}
	}
#ifdef DEBUG
	check(__LINE__);
#endif DEBUG

	/* assure that militia unit resides in each city */
		for(armynum=0;armynum<MAXARM;armynum++)
		if((ASOLD>0)&&(ATYPE==A_MILITIA)){
#ifdef DEBUG
		printf("\t\t%s army %d (now %d men - populace %d)\n",unittype[ATYPE],armynum,ASOLD,sct[AXLOC][AYLOC].people);
#endif DEBUG
			if((sct[AXLOC][AYLOC].designation!=DCITY)
			&&(sct[AXLOC][AYLOC].designation!=DCAPITOL)){
#ifdef DEBUG
				printf("\teliminating %s army %d as %d %d is des:%c alt:%c own:%d\n",unittype[ATYPE],armynum,AXLOC,AYLOC,sct[AXLOC][AYLOC].designation,sct[AXLOC][AYLOC].altitude,sct[AXLOC][AYLOC].owner);
#endif DEBUG
				if(sct[AXLOC][AYLOC].owner == country)
				sct[AXLOC][AYLOC].people+=ASOLD;
				else
				sct[ntn[country].capx][ntn[country].capy].people+=ASOLD;
				ASOLD=0;
			}
		}

	if(ntn[country].tgold > 0)
	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++)
	if((sct[x][y].owner==country)
	&&((sct[x][y].designation==DCITY)||(sct[x][y].designation==DCAPITOL))){
		free=FALSE;
		for(armynum=0;armynum<MAXARM;armynum++){
			if((ASOLD>0)
			&&(AXLOC==x)&&(AYLOC==y)
			&&(ATYPE==A_MILITIA)) {
				free=TRUE;
				break;
			}
		}
		if(free==FALSE) {	 /* draft a new militia army */
			for(armynum=0;armynum<MAXARM;armynum++) if(ASOLD==0){
				AXLOC=x;
				AYLOC=y;
				free=TRUE;
				break;
			}
		}
		if((free==TRUE)) {
			/* want to add ideal troops */
			ideal = sct[x][y].people/MILINCITY - ASOLD;
			ideal = min(ideal, 250);
			if(ideal>0){
			if(magic(country,WARRIOR)==TRUE){ /* WARRIOR power */
				ntn[country].tgold-=
					(ideal* *(u_encost+ATYPE))/2;
			} else {
				ntn[country].tgold-=
					ideal* *(u_encost + ATYPE);
			}
			}
			ASOLD+=ideal;
			ATYPE=A_MILITIA;
			ASTAT=GARRISON;
#ifdef DEBUG
		printf("\tadding %d troops to %s army %d (now %d men - populace %d)\n",ideal,unittype[ATYPE],armynum,ASOLD,sct[x][y].people);
#endif DEBUG
		}
	}
#ifdef DEBUG
	check(__LINE__);
#endif DEBUG

	/* if archer power - all units are archers */
	if(magic(country,ARCHER)==1)
	for(armynum=1;armynum<MAXARM;armynum++)
		if((ASOLD>0)&&(ATYPE!=A_MILITIA)) ATYPE=A_ARCHER;
}

void
getdstatus()
{
	int x,oldstat[MAXNTN];

	if(ntn[country].active<=1) return;

	for(x=1;x<MAXNTN;x++) {
		oldstat[x] = ntn[country].dstatus[x];
		/*if they at war with you go to war with them*/
		if(ntn[x].dstatus[country]>=WAR) {
			if(ntn[country].dstatus[x]<WAR)
			fprintf(fnews,"2.\tnation %s returns %s's declaration of war\n",ntn[country].name,ntn[x].name);

			ntn[country].dstatus[x]=WAR;
		} else
		if((ntn[country].dstatus[x]!=UNMET)
		&&(ntn[country].dstatus[x]!=JIHAD)
		&&(ntn[country].dstatus[x]!=CONFEDERACY)
		&&(ntn[country].active!=1)){
			if((ntn[x].tmil>4*ntn[country].tmil)
			&&(ntn[x].score>4*ntn[country].score)){
				if(rand()%3==0)  {
					ntn[country].dstatus[x]=WAR;
				} else if(rand()%8==0)  {
					ntn[country].dstatus[x]=JIHAD;
				}
			}
			/*if 2* mil and 2* score then not like them*/
			else if((ntn[x].tmil>2*ntn[country].tmil)
			&&(ntn[x].score>2*ntn[country].score)){
				if((ntn[country].dstatus[x]!=WAR)
				&&(ntn[x].race!=ntn[country].race)){
					if(rand()%3==0)
						ntn[country].dstatus[x]++;
				}
			}
			/*adjust based on your status with them*/
			if((ntn[country].dstatus[x]==WAR)
			&&(ntn[x].dstatus[country]<WAR))
				if(rand()%3==0) ntn[country].dstatus[x]--;

			if((ntn[country].dstatus[x]<WAR)
			&&(ntn[country].dstatus[x]>ALLIED)){
				if(ntn[x].dstatus[country]>1+ntn[country].dstatus[x]){
					if(rand()%3==0)
						ntn[country].dstatus[x]++;
				}
				else
				if(ntn[x].dstatus[country]+1<ntn[country].dstatus[x]){
					if(rand()%3==0)
						ntn[country].dstatus[x]--;
				}
				if(ntn[x].race==ntn[country].race)
					if(rand()%3==0)
						ntn[country].dstatus[x]--;
			}
			if(rand()%6==0) {
				if(ntn[country].dstatus[x]!=JIHAD)
					ntn[country].dstatus[x]++;
			}
			else if((rand()%5==0)
			&&(ntn[country].dstatus[x]!=CONFEDERACY)) {
				ntn[country].dstatus[x]--;
			}
		}
	}

	for(x=1;x<MAXNTN;x++)
	if(ntn[x].active>1){
		if((rand()%5==0)
		&&(ntn[x].dstatus[country]==WAR)
		&&(ntn[country].dstatus[x]==WAR)) {
			ntn[x].dstatus[country]=HOSTILE;
			ntn[country].dstatus[x]=HOSTILE;
			fprintf(fnews,"2.\tnation %s and %s announce ceasefire\n",ntn[country].name,ntn[x].name);
		} else if((oldstat[x]==WAR)&&(ntn[country].dstatus[x]==WAR)){
			fprintf(fnews,"2.\tnation %s stays at war with %s\n",ntn[country].name,ntn[x].name);
		} else if((oldstat[x]<WAR)&&(ntn[country].dstatus[x]==WAR)){
			fprintf(fnews,"2.\tnation %s goes to war with %s\n",ntn[country].name,ntn[x].name);
		} else if((oldstat[x]!=JIHAD)&&(ntn[country].dstatus[x]==JIHAD)){
			fprintf(fnews,"2.\tnation %s announces a jihad with %s\n",ntn[country].name,ntn[x].name);
		}
	}
}

int
nationrun()
{
	int goldthresh,ironthresh,citythresh;
	int armynum,loop;
	int x,y,i;
	long zz;

#ifdef DEBUG
	check(__LINE__);
#endif DEBUG
	prep(country);

	/* is there an error*/
	if((sct[ntn[country].capx][ntn[country].capy].owner==country)
	&&(sct[ntn[country].capx][ntn[country].capy].designation!=DCAPITOL)){
		sct[ntn[country].capx][ntn[country].capy].designation=DCAPITOL;
	}

	getdstatus();

	/*move units */
	/*are they at war with any normal countries*/
	peace=0;
	for(i=1;i<MAXNTN;i++) if(ntn[country].dstatus[i]>peace)
		peace=ntn[country].dstatus[i];

#ifdef DEBUG
	check(__LINE__);
#endif DEBUG
	if(peace<WAR){
		peace=8;
		pceattr();
	} else {
	/*if war then attack &/or expand */
		peace=12;
		/*are they attacking or defending */
		if(ntn[country].tmil==0) defattr();
		else for(x=0;x<MAXNTN;x++) if(ntn[country].dstatus[x]>HOSTILE){
			if(100*(ntn[country].tmil*(ntn[country].aplus+100))/((ntn[country].tmil*(ntn[country].aplus+100))+(ntn[x].tmil*(ntn[x].dplus+100)))>rand()%100){
				/*attacker*/
				for(armynum=1;armynum<MAXARM;armynum++)
					if((ASOLD>0)&&(ATYPE!=A_MILITIA))
						ASTAT=ATTACK;
				atkattr();
			}
			/*defender*/
			else {
				for(armynum=1;armynum<MAXARM;armynum++)
					if((ASOLD>0)&&(ATYPE!=A_MILITIA)){
						if(ASOLD<350) ASTAT=DEFEND;
						else ASTAT=ATTACK;
					}
				defattr();
			}
		}
	}
#ifdef DEBUG
	check(__LINE__);
#endif DEBUG
	for(armynum=1;armynum<MAXARM;armynum++)
		if((ASOLD!=0)
		&&(ATYPE!=A_MILITIA)) armymove(armynum);

	/* INTELLIGENT SECTOR REDESIGNATION */
	if(ntn[country].active!=1) {		/* no pc's */
		goldthresh=4;
		ironthresh=4;
		citythresh=10;
		for(loop=1;loop<5;loop++) {
#ifdef DEBUG
			printf("\tnpcredes(): country %s, food %ld gold %d, iron %d, city %d\n",ntn[country].name,spread.food,goldthresh,ironthresh,citythresh);
#endif DEBUG

			for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++)
				if((sct[x][y].owner==country)
				&&(is_habitable(x,y)))
				n_redes(x,y,goldthresh,ironthresh,citythresh);

			spreadsheet(country);

			if(spread.food < spread.civilians * 2L) {
				goldthresh++;
				ironthresh++;
				citythresh--;
			} else if(spread.food > spread.civilians * 3L) {
				if(rand()%2==0) goldthresh--;
				else ironthresh--;
				if(goldthresh==0) goldthresh=1;
				if(ironthresh==0) ironthresh=1;
				citythresh++;
			}
			else break;
		}
	}

	/*build forts in any cities*/
	if(ntn[country].active!=1) for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++)
		if(((sct[x][y].designation==DCITY)
			||(sct[x][y].designation==DCAPITOL))
			&&(sct[x][y].owner==country)
			&&(sct[x][y].fortress<10)
			&&(ntn[country].tgold>10000)
			&&(rand()%5==0)
			&&(sct[x][y].fortress<(sct[x][y].people%1000)))
				sct[x][y].fortress++;
#ifdef DEBUG
	check(__LINE__);
#endif DEBUG
	/*redo mil*/
	if(ntn[country].active!=1) redomil();
#ifdef DEBUG
	check(__LINE__);
#endif DEBUG

	/*buy new powers and/or new weapons*/
	if(getmgkcost(M_MIL,country) < getmgkcost(M_CIV,country)){
		if(ntn[country].jewels > getmgkcost(M_MIL,country)) {
			ntn[country].jewels-=getmgkcost(M_MIL,country);
			if((zz=getmagic(M_MIL))!=0){
				fprintf(fnews,"1.\tnation %s gets combat power number %ld\n",ntn[country].name,zz);
				printf("\tnation %s gets combat power number %ld\n",ntn[country].name,zz);
				exenewmgk(zz);
			} else if((zz=getmagic(M_MIL))!=0){
				fprintf(fnews,"1.\tnation %s gets combat power number %ld\n",ntn[country].name,zz);
				printf("\tnation %s gets combat power number %ld\n",ntn[country].name,zz);
				exenewmgk(zz);
			}
			else	ntn[country].jewels+=getmgkcost(M_MIL,country);
		}
	} else {
		if(ntn[country].jewels > getmgkcost(M_CIV,country)) {
			ntn[country].jewels-=getmgkcost(M_CIV,country);
			if((zz=getmagic(M_CIV))!=0){
				fprintf(fnews,"1.\tnation %s gets civilian power number %ld\n",ntn[country].name,zz);
				printf("\tnation %s gets civilian power number %ld\n",ntn[country].name,zz);
				exenewmgk(zz);
			}
			else if((zz=getmagic(M_CIV))!=0){
				fprintf(fnews,"1.\tnation %s gets civilian power number %ld\n",ntn[country].name,zz);
				printf("\tnation %s gets civilian power number %ld\n",ntn[country].name,zz);
				exenewmgk(zz);
			}
			else	ntn[country].jewels+=getmgkcost(M_CIV,country);
		}
	}

#ifdef DEBUG
	check(__LINE__);
#endif DEBUG
	if(magic(country,VAMPIRE)!=1) {
		i=0;
		if(magic(country,WARLORD)==1) i=30;
		else if(magic(country,CAPTAIN)==1) i=20;
		else if(magic(country,WARRIOR)==1) i=10;
		x = max ( ntn[country].aplus-i, 10 ) / 10;
		/* SHOULD USE spread.iron but it didnt work right */
		if(ntn[country].tiron >  3 * IRONORE * ntn[country].tmil*x*x){
			ntn[country].aplus+=1;
			ntn[country].dplus+=1;
			ntn[country].tiron-=2*IRONORE*ntn[country].tmil*x*x;
			printf("\tnation %s buys +1 percent attack and defence\n",ntn[country].name);
		}
	}
}

/* make sectors of those with whom you are peaceful attr = 1 */
n_atpeace()
{
	register int x,y;
	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++)
		if((sct[x][y].owner!=0)
		&&(ntn[country].dstatus[sct[x][y].owner]<WAR)
		&&(abs(x-ntn[country].capx)>2)
		&&(abs(y-ntn[country].capy)>2))
			attr[x][y]=1;
}

/* you are too far from capitol */
n_toofar()
{
	register int x,y;
	for(x=ntn[country].capx+NPCTOOFAR;x<MAPX;x++)
		for(y=0;y<MAPY;y++) if(ONMAP)
			attr[x][y]=1;
	for(y=ntn[country].capy+NPCTOOFAR;y<MAPY;y++)
		for(x=0;x<MAPX;x++) if(ONMAP)
			attr[x][y]=1;
	for(x=0;x<ntn[country].capx-NPCTOOFAR;x++)
		for(y=0;y<MAPY;y++) if(ONMAP)
			attr[x][y]=1;
	for(y=0;y<ntn[country].capy-NPCTOOFAR;y++)
		for(x=0;x<MAPX;x++) if(ONMAP)
			attr[x][y]=1;
}

/* take undefended land */
n_unowned()
{
	register int x,y;
	int dist;

	/* around capitol */
	for(x=ntn[country].capx-4;x<=ntn[country].capx+4;x++){
		for(y=ntn[country].capy-4;y<=ntn[country].capy+4;y++){
			if((ONMAP)&&(sct[x][y].owner==0)) {
				/* find distance btwn x,y and cap */
				dist=abs(x-ntn[country].capx)+abs(y,ntn[country].capy);
				if(dist<10) attr[x][y]+= 500 - 50*dist;
			}
		}
	}

	for(x=0;x<MAPX;x++) {
		for(y=0;y<MAPY;y++) {
			/*add to attractiveness for unowned sectors*/
			if(sct[x][y].owner == 0) {
				attr[x][y]+=100;
			} else	{
			if(sct[x][y].owner == NNOMAD)
				attr[x][y]+=25;
			}
		}
	}
	if(is_habitable(x,y) == 0) attr[x][y] /= 5;
}
void
n_defend(natn)
register short natn;
{
	int x,y;

	/* add 1/10th of soldiers in sector */
	for(x=1;x<MAXARM;x++) if(ntn[natn].arm[x].sold > 0)
		if(sct[ntn[natn].arm[x].xloc][ntn[natn].arm[x].yloc].owner==country)
			attr[ntn[natn].arm[x].xloc][ntn[natn].arm[x].yloc] +=
				ntn[natn].arm[x].sold/10;

	/*plus 80 if near your capitol */
	for(x=ntn[country].capx-1;x<=ntn[country].capy+1;x++){
		for(y=ntn[country].capy-1;y<=ntn[country].capy+1;y++){
			if(ONMAP) attr[x][y]+=80;
		}
	}

	/*plus based on defensive value*/
	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) {
		if(movecost[x][y]==1) attr[x][y] += 50;
		else if(movecost[x][y]<=3) attr[x][y] += 20;
		else if(movecost[x][y]<=5) attr[x][y] += 10;

		if((sct[x][y].designation==DCITY)
		&&((sct[x][y].owner==country)||(sct[x][y].owner==natn))){
			attr[x][y] += 50;
		}
	}
}

void
n_attack(nation)
register short nation;
{
	register int x,y;

	/*plus 40 if next to their capitol */
	for(x=ntn[nation].capx-1;x<=ntn[nation].capy+1;x++){
		for(y=ntn[nation].capy-1;y<=ntn[nation].capy+1;y++){
			if((sct[x][y].owner==nation)&&(ONMAP))
				attr[x][y]+=40;
		}
	}

	/*plus 100 if on their capitol*/
	attr[ntn[nation].capx][ntn[nation].capy]+=100;
}

/* +100 if undefended sectors of nation, +60 if not */
void
n_undefended( nation )
{
	register int x,y;
	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) if(sct[x][y].owner==nation){
		if(is_habitable(x,y) == 0) {
			attr[x][y] += 30;
		} else if(occ[x][y]==0) {
			attr[x][y]+=100;
		} else {
			attr[x][y]+=60;
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
	if (ntn[nation].capx < ntn[country].capx){
		x1=ntn[nation].capx;
		x2=ntn[country].capx;
	} else {
		x1=ntn[country].capx;
		x2=ntn[nation].capx;
	}
	if (ntn[nation].capy < ntn[country].capy){
		y1=ntn[nation].capy;
		y2=ntn[country].capy;
	}
	else {
		y1=ntn[country].capy;
		y2=ntn[nation].capy;
	}

	for(x=x1;x<=x2;x++) for(y=y1;y<=y2;y++) {
		if(ONMAP) attr[x][y]+=60;
	}
}

/* if in jeopardy, move to survive
 *	if within two of cap add 1/5th of men
 *	if on cap and war and 2x your garrison go jihad and + 1/2 men
 */
void
n_survive()
{
	register struct army		*aptr;
	int nation,armynum;

	if(sct[ntn[country].capy][ntn[country].capy].owner!=country){
		attr[ntn[country].capy][ntn[country].capy]=1000;
	}

	/*defend your capitol if occupied, +50 more if with their army*/
	for(nation=1;nation<MAXNTN;nation++)
	if((ntn[nation].active != 0)
	&&((ntn[nation].dstatus[country]>=WAR)
	||(ntn[country].dstatus[nation]>=WAR))){
		for(armynum=1;armynum<MAXARM;armynum++)
		if(ntn[nation].arm[armynum].sold > 0) {

			aptr = &(ntn[nation].arm[armynum]);

			if((aptr->xloc==ntn[nation].capx)
			&&(aptr->yloc==ntn[nation].capy)){
				if(aptr->sold>2*ntn[country].arm[0].sold){
					attr[aptr->xloc][aptr->yloc]+=
						aptr->sold/2;
				} else {
					attr[aptr->xloc][aptr->yloc]+=
						aptr->sold/5;
				}
			}
			else if((aptr->xloc<=ntn[nation].capx+2)
			&&(aptr->xloc>=ntn[nation].capx-2)
			&&(aptr->yloc<=ntn[nation].capy+2)
			&&(aptr->yloc>=ntn[nation].capy-2)){
				attr[aptr->xloc][aptr->yloc]+=aptr->sold/5;

			}
		}
	}
}

void
defattr()
{
	int nation;

	n_unowned();

	for(nation=1;nation<MAXNTN;nation++)
	if((ntn[nation].active!=0)&&(ntn[country].dstatus[nation]>=WAR)) {
		n_defend(nation);
		n_between(nation);
		n_undefended(nation);
	}

	n_toofar();
	n_survive();
}

/*calculate attractiveness of attacking sectors*/
void
atkattr()
{
	int nation;

	n_unowned();

	/*adjust for each nation that you are at war with*/
	for(nation=1;nation<MAXNTN;nation++)
		if(ntn[country].active!=0)
			if(ntn[country].dstatus[nation]==WAR) {
				n_between(nation);
				n_undefended(nation);
				n_attack(nation);
			} else if(ntn[country].dstatus[nation]==JIHAD) {
				n_attack(nation);
				n_attack(nation);
				n_between(nation);
				n_undefended(nation);
				n_attack(nation);
				n_between(nation);
				n_undefended(nation);
				n_attack(nation);
			}

	n_toofar();
	n_survive();
}

/*calculate attractiveness when at peace*/
int
pceattr()
{
	n_unowned();
	n_atpeace();
	n_toofar();
	n_survive();
}
#endif

#ifdef DEBUG
check(line)
int line;
{
	int armynum;
	for(armynum=0;armynum<MAXARM;armynum++){
		if(ASOLD<0) {
		printf("ERROR: line %d army %d nation %s soldier %d\n",line,armynum,ntn[country].name,ASOLD);
		ASOLD=0;
		}
		if((AXLOC>MAPX)||(AYLOC>MAPY)){
		printf("CHECK ERROR: line %d army %d nation %s loc %d %d\n",line,armynum,ntn[country].name,AXLOC,AYLOC);
		AXLOC = ntn[country].capx;
		AYLOC = ntn[country].capy;
		}
	}
	if(ntn[country].tiron < 0L){
		printf("ERROR: line %d nation %s iron is %ld\n",line,ntn[country].name,ntn[country].tiron);
		ntn[country].tiron = 0;
	}
	if(ntn[country].tfood < 0L){
		printf("ERROR: line %d nation %s food is %ld\n",line,ntn[country].name,ntn[country].tfood);
		ntn[country].tfood = 0;
	}
	if(ntn[country].jewels < 0L){
		printf("ERROR: line %d nation %s jewels is %ld\n",line,ntn[country].name,ntn[country].jewels);
		ntn[country].jewels = 0;
	}
}
#endif DEBUG
