#include	<stdio.h>
#include	"header.h"
#include "data.h"
#ifdef RANEVENT

extern FILE *fnews;
extern short country;
char eventstr[80];

char *randevents[] = {
/* 0 */  "province rebels -- disolve 10% of nation",
/* 1 */  "evil wizard sets up -- disolve 10% of nation",
/* 2 */  "tax revolt -- disolve 20% of nation",
/* 3 */  "rebelion  -- disolve 30% of nation",
/* 4 */  "army revolts -- disolve 40% of nation",
/* 5 */  "religious schism -- disolve 40% of nation",
/* 6 */  "peasant revolt - lose 40% of unsupported sectors",
/* 7 */  "peasant revolt - lose 40% of unsupported sectors",
/* 8 */  "peasant revolt -- lose 40% of unsupported sectors",
/* 9 */  "dragon raid -- lose 30% of food",
/* 10 */  "famine -- food in granaries reduced 75%",
/* 11 */  "hurricane",
/* 12 */  "tornado",
/* 13 */  "volcano erupts -- all flee and 30% die",
/* 14 */  "royal wedding (absorb neighbor nation)",
/* 15 */  "new alloy gives new fighter power)",
/* 16 */  "royal advisor discovered to be spy -- lose power",
/* 17 */  "gold strike one sector ",
/* 18 */  "gold strike one sector ",
/* 19 */  "gold vein runs out one sector ",
/* 20 */  "gold vein runs out one sector ",
/* 21 */  "flood",
/* 22 */  "earthquake",
/* 23 */  "frost -- crops ruined",
/* 24 */  "dragon killed, you gain his jewel hoard",
/* 25 */  "large nomad army raids in your area",
/* 26 */  "city burns to the ground",
/* 27 */  "black plague -- 30% of populace dies",
/* 28 */  "pirate raid on harbor",
/* 29 */  "barbarian raid",
/* 30 */  "new magician offers you magic power",
/* 31 */  "new magic item give you magic power",
/* 32 */  "ores in iron mine run out",
/* 33 */  "new architect strengthens castle walls ",
/* 34 */  "new ores discovered + 4-10 iron",
/* 35 */  "charismatic leader declares peace",
/* 36 */  "severe winter",
/* 37 */  "severe winter",
/* 38 */  "tidal wave -- abandon all coastlands ",
/* 39 */  "ninja destroy general staff - 1/2 armies paralyzed",
/* 40 */  "general found to be spy -- many armies paralyzed",
/* 41 */  "general prosperity +20% gold"
};
#define MAXRANEVENT 41

/*finds unused nation and sets it up partially*/
findnewnation()
{
int newntn=0,nationis;
for ( nationis=MAXNTN ; nationis >= 1; nationis--) 
	if (ntn[nationis].active == 0) newntn=nationis;
if (newntn == 0) return (0);
strcpy(ntn[newntn].leader,"rebel");
strcpy(ntn[newntn].passwd,ntn[0].passwd);
ntn[newntn].class=0;
ntn[newntn].score=0;
ntn[newntn].tsctrs=0;
ntn[newntn].active=2;
return(newntn);
}

/* disolve a certain percent of a nation */
/*returns value of new nation */
disolve(percent, target)
short target;
int percent;
{
int nosplit, defaultx=0, defaulty=0, realx=0, realy=0, newnation, dist;
int i,j,armynum,posi,posj;

nosplit =  ntn[target].tsctrs * percent / 100;
if (nosplit==0) {
	strcpy(eventstr,"no sectors to split");
	return(0);
}
if (nosplit<=5) {
	strcpy(eventstr,"nation is too small -> no sectors will be split");
	return(0);
}
/* find starting city */
for(posi=0; posi<MAPX; posi++) for(posj=0; posj<MAPY; posj++) {
if((sct[posi][posj].designation == DCITY )&&(sct[posi][posj].owner == target)){
		if ( rand()%5 == 0 ) {
			realx = posi;
			realy = posj;
		} else {
			defaultx = posi;
			defaulty = posj;
		}
	}
}
if ((realx == 0 ) && (realy == 0)){
	realx = defaultx;
	realy = defaultx;
}
if ((realx == 0 ) && (realy == 0)) {
	strcpy(eventstr,"can not disolve nation -> no cities available");
	return(0);
}
newnation=findnewnation();
if(newnation == 0) {
	strcpy(eventstr,"no nations available");
	return(0);
}
sprintf(eventstr,"new nation created at %d,%d and %d sectors",realx,realy,nosplit);
sct[realx][realy].owner=newnation;
ntn[newnation].capx=realx;
ntn[newnation].capy=realy;
sct[realx][realy].designation=DCAPITOL;
strcpy(ntn[newnation].name,ntn[target].name);
strncat(ntn[newnation].name,"-rebel",min(NAMELTH-strlen(ntn[country].name),6));
ntn[newnation].race= ntn[target].race;
ntn[newnation].tgold= ntn[target].tgold* percent / 100;
ntn[newnation].tfood= ntn[target].tfood* percent / 100;
ntn[newnation].jewels= ntn[target].jewels* percent / 100;
ntn[newnation].tiron= ntn[target].tiron* percent / 100;
ntn[newnation].tciv= ntn[target].tciv* percent / 100;
ntn[newnation].tmil= ntn[target].tmil* percent / 100;
ntn[target].tgold -= ntn[newnation].tgold;
ntn[target].tfood -= ntn[newnation].tfood;
ntn[target].jewels -= ntn[newnation].jewels;
ntn[target].tiron -= ntn[newnation].tiron;
ntn[target].tciv -= ntn[newnation].tciv;
ntn[target].tmil -= ntn[newnation].tmil;
ntn[newnation].repro= ntn[target].repro;
ntn[newnation].maxmove= ntn[target].maxmove;
ntn[newnation].aplus= ntn[target].aplus;
ntn[newnation].dplus= ntn[target].dplus;
ntn[newnation].location= ntn[target].location;
ntn[newnation].powers= ntn[target].powers;
ntn[newnation].mark= ntn[target].mark;
for ( dist=2 ; dist < 10; dist++) if (nosplit > 0) 
for (i=defaultx-dist; i<defaultx+dist; i++) 
	for (j=defaulty-dist; j<defaulty+dist; j++){
		if (i>=0 && j>=0 && i<MAPX && j<MAPY
		&&(nosplit>0) 
		&&( sct[i][j].designation != DCAPITOL )
		&&( sct[i][j].owner == target)){
				nosplit--;
				sct[i][j].owner=newnation;
		}
	}
for (armynum=0;armynum<MAXNAVY;armynum++) {
	ntn[newnation].nvy[armynum].warships = 0;
	ntn[newnation].nvy[armynum].merchant = 0;
}
for (armynum=0;armynum<MAXARM;armynum++) 
	if((ntn[target].arm[armynum].sold>0)
	&&(sct[ntn[target].arm[armynum].xloc][ntn[target].arm[armynum].yloc].owner==newnation)){
		ntn[newnation].arm[armynum].sold=ntn[target].arm[armynum].sold;
		ntn[newnation].arm[armynum].xloc=ntn[target].arm[armynum].xloc;
		ntn[newnation].arm[armynum].yloc=ntn[target].arm[armynum].yloc;
		ntn[newnation].arm[armynum].stat=ntn[target].arm[armynum].stat;
		ntn[newnation].arm[armynum].smove=ntn[target].arm[armynum].smove;
		ntn[target].arm[armynum].sold = 0;
	} else {
		ntn[newnation].arm[armynum].sold = 0;
	}
armynum=0;
ntn[newnation].arm[0].sold = 300;
ntn[newnation].arm[0].xloc = realx;
ntn[newnation].arm[0].yloc = realy;
ntn[newnation].arm[0].stat = GARRISON;
ntn[newnation].arm[0].smove = 0;
for (dist=0;dist<MAXNTN;dist++) if(dist!=newnation) {
	ntn[newnation].dstatus[dist]=NEUTRAL;
	ntn[dist].dstatus[newnation]=NEUTRAL;
}
for (dist=MAXNTN;dist<NTOTAL;dist++) {
	ntn[newnation].dstatus[dist]=WAR;
	ntn[dist].dstatus[newnation]=WAR;
}
ntn[newnation].dstatus[target]=WAR;
ntn[target].dstatus[newnation]=WAR;
return(newnation);
}

int
randomevent()
{
int count, totalscore=0, event, newnation, i, j, armynum;
int done;	/*if 1 then event happened */
int newpower;

printf("RANDOM HAPPENINGS ARE NOW BEING CHECKED\n");
for(country=0;country<MAXNTN;country++) if ( ntn[country].active != 0 )
	totalscore+= ntn[country].score;

/* decide what nations get random event */
for(country=0;country<MAXNTN;country++)
	if (( ntn[country].active != 0 )
	&&(ntn[country].score > 20)
	&&((rand()%totalscore) < NORANDEVENTS * ntn[country].score)){
	event = rand()%(MAXRANEVENT+1);
	done=1;
	/* do the event */
	switch(event) {
	case 0:
		/*general/province defects*/
		if(disolve(10, country)==0) done=0;;
		break;
	case 1:
		/*evil wizard sets up -- disolve 10%*/
		if(disolve(10, country)==0) done=0;;
		break;
	case 2:
		/*tax revolt -- disolve 20%*/
		if(disolve(20, country)==0) done=0;;
		break;
	case 3:
		/*rebelion  -- disolve 30%*/
		if(rand()%2==0) if(disolve(30, country)==0) done=0;;
		break;
	case 4:
		/*general takes over province -- disolve 40%*/
		if(rand()%2==0) if(disolve(40, country)==0) done=0;;
		break;
	case 5:
		/*religious schism -- disolve 40%*/
		if(rand()%2==0) if(disolve(40, country)==0) done=0;;
		break;
	case 6:
	case 7:
	case 8:
		/*peasant revolt -- lose 40% unsupported sectors*/
		if(newnation=disolve(10, country) == 0 ) done=0;
		if(done==1) for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
			if(( sct[i][j].owner == country)
			&&( solds_in_sector(i,j,country)==0))
			if(rand()%10<=3) sct[i][j].owner = newnation;
		break;
	case 9:
		/*dragon raid -- lose 30% of food*/
			ntn[country].tfood *= .7;
		break;
	case 10:
		/*famine -- food=0 10% starve*/
			ntn[country].tfood /= 4;
		break;
	case 11:
		/*hurricane*/
		done=0;
		break;
	case 12:
		/*tornado*/
		done=0;
		break;
	case 13:
		/*volcano -- all flee around one mountain -- 30% die*/
		done=0;
		break;
	case 14:
		/*royal wedding (absorb neighbor nation)*/
		takeover ( 100, 0 );
		break;
	case 15:
		/*new alloy +10% combat (WARRIOR...)*/
		if(magic(country,WARRIOR)!=1){
			ntn[country].powers|=WARRIOR;
			exenewmgk(WARRIOR);
		}
		else if(magic(country,WARLORD)!=1){
			ntn[country].powers|=WARLORD;
			exenewmgk(WARLORD);
		}
		else if(magic(country,CAPTAIN)!=1){
			ntn[country].powers|=CAPTAIN;
			exenewmgk(CAPTAIN);
		}
		break;
	case 16:
		/*royal advisor is spy -- lose power*/
		done=0;
		break;
	case 17:
	case 18:
		/*gold strike one sector +4-10 gold*/
		for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
			if(( sct[i][j].owner == country)
			&& (sct[i][j].gold != 0))
				sct[i][j].gold += rand()%7 + 4;
		break;
	case 19:
	case 20:
		/*gold vein runs out one sector >5 gold =0*/
		for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
			if(( sct[i][j].owner == country)
			&& (sct[i][j].gold >= 5)){
				sct[i][j].gold =0;
				sct[i][j].designation = DFARM;
			}
		break;
	case 21:
		/*flood*/
		done=0;
		break;
	case 22:
		/*earthquake*/
		done=0;
		break;
	case 23:
		/*frost -- crops ruined*/
		done=0;
		break;
	case 24:
		/*dragon killed + 10000 jewels*/
		ntn[country].jewels+=10000;
		break;
	case 25:
		/*nomad raid -- put large nomad army in area*/
		for(count=0; count < 100; count++){
		i=(rand()%(MAPX-8))+4;
		j=(rand()%(MAPY-8))+4;
		/* get army number */
		for(newpower=0; newpower<MAXARM; newpower++) 
			if (ntn[NNOMAD].arm[newpower].sold == 0) 
				armynum=newpower;
		if(armynum == 0) return;
		if((is_habitable(i,j)) && ( sct[i][j].owner == country)) {
 			ntn[NNOMAD].arm[armynum].xloc =i;
 			ntn[NNOMAD].arm[armynum].yloc =j;
			ntn[NNOMAD].arm[armynum].sold =300+200*rand()%10;
			ntn[NNOMAD].arm[armynum].stat =ATTACK;
			count = 100;
		}
		}
		break;
	case 26:
		/*city burns -- reduce fort and redesignate*/
		for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
			if(( sct[i][j].owner == country)
			&& ( sct[i][j].designation == DCITY)){
				sct[i][j].designation = DFARM;
				sct[i][j].people *= .5;
			}
		break;
	case 27:
		/*plague -- 30% of populace dies*/
			for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
				if( sct[i][j].owner == country)
				sct[i][j].people *= .7;
		break;
	case 28: /*pirate raid on harbor*/
		done=0;
		break;
	case 29: /*barbarian raid*/
		done=0;
		break;
	case 30: /*new magician + RANDOM POWER*/
	case 31: /*new magic item + RANDOM POWER*/
		/*buy new powers and/or new weapons*/
		if((newpower=getmagic())!=0){
			printf("\tnation %s gets magic power number %d\n",ntn[country].name,newpower);
			fprintf(fnews,"1. \tevent in %s -> gets magic power number %d\n", ntn[country].name,newpower);
			exenewmgk(newpower);
		}
		else done=0;
		break;
	case 32:
		/* ores run out */
		for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
		if(( sct[i][j].owner == country)
			&& (sct[i][j].iron >= 5)){
				sct[i][j].iron =0;
				sct[i][j].designation = DFARM;
		}
	case 33:
		/*new architect strengthens castle walls */
			for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
				if(( sct[i][j].owner == country)
				&& (sct[i][j].designation == DCITY
				|| sct[i][j].designation == DCAPITOL))
				sct[i][j].fortress += 2;
		break;
	case 34:
		/*new ores discovered + 4-10 iron*/
		for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
			if(( sct[i][j].owner == country)
			&& (sct[i][j].iron != 0))
				sct[i][j].iron += rand()%7 + 4;
		break;
	case 35:
		/*new leader sets up peace*/
		for (newnation=0;newnation<MAXNTN;newnation++) if(country!=newnation) {
			if( ntn[newnation].dstatus[country]>NEUTRAL ){
			ntn[newnation].dstatus[country]=NEUTRAL;
			ntn[country].dstatus[newnation]=NEUTRAL;
			}
		}
		break;
	case 36:
	case 37:
		/*severe winter*/
		done=0;
		break;
	case 38:
		/*tidal wave -- abandon all coastlands */
		done=0;
		break;
	case 39: /*ninja attack paralyzes half your armys AMOVE=0*/
	case 40: /*general found to be spy AMOVE=0*/
		for(armynum=0; armynum<MAXARM; armynum++) if(rand()%2==0) 
			ntn[country].arm[armynum].smove =0;
		break;
	case 41:
		/*general prosperity +20% gold*/
		done=0;
		break;
	default:
		printf("error condition -- illegal random event\n");
		break;
	}
	if(done==1) fprintf(fnews,"1. \tevent in %s - %s\n"
		,ntn[country].name,randevents[event]);
	if(done==1) printf("\t%s -- %s\n"
		,ntn[country].name,randevents[event]);
	else printf("\t%s -- (unimplemented) %s\n"
		,ntn[country].name,randevents[event]);
	if(strlen(eventstr)>5) {
		printf("\t\t->%s\n",eventstr);
		if(done==1) fprintf(fnews,"1. \tevent in %s --> %s\n"
			,ntn[country].name,eventstr);
	}
	strcpy(eventstr,"");
	}
}
#endif
