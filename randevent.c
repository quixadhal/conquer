/* Conquer: Copyright (c) 1988 by Edward M Barlow */
#include	<stdio.h>
#include	<ctype.h>
#include	"header.h"
#include "data.h"
#ifdef RANEVENT
#ifdef ADMIN
char	*names[] = {		/* must end in single character name */
	"groo","brok","vul","poin","srop","hoga","nobi","bonz","gail",
	"lynn","zorb","theed","urda","X"
};

extern FILE *fnews;
extern short country;

char eventstr[80];
int xpos,ypos;		/* saved x and y position */

char *randevents[] = {
/* 0 */  "province rebels -- disolve 10% of nation",
/* 1 */  "evil wizard sets up -- disolve 10% of nation",
/* 2 */  "tax revolt -- disolve 20% of nation",
/* 3 */  "rebelion  -- disolve 30% of nation",
/* 4 */  "army revolts -- disolve 30% of nation",
/* 5 */  "religious schism -- disolve 30% of nation",
/* 6 */  "peasant revolt ",
/* 7 */  "peasant revolt ",
/* 8 */  "peasant revolt ",
/* 9 */  "dragon raid -- lose 30% of food",
/* 10 */  "famine -- food in granaries reduced 75%",
/* 11 */  "hurricane",
/* 12 */  "tornado",
/* 13 */  "volcano erupts -- all flee, 30% die (in 1 sector range)",
/* 14 */  "royal wedding (absorb neighbor nation)",
/* 15 */  "new alloy gives new fighter power",
/* 16 */  "royal advisor discovered to be spy -- lose power",
/* 17 */  "gold strike one sector ",
/* 18 */  "gold strike one sector ",
/* 19 */  "gold vein runs out in one goldmine sector ",
/* 20 */  "gold vein runs out in one goldmine sector ",
/* 21 */  "flood",
/* 22 */  "earthquake",
/* 23 */  "frost -- crops ruined",
/* 24 */  "dragon killed, you gain his jewel hoard",
/* 25 */  "several nomad armies raid in your area",
/* 26 */  "town burns to the ground",
/* 27 */  "black plague -- 40% of populace & armies die",
/* 28 */  "pirate raid on harbor",
/* 29 */  "barbarian raid",
/* 30 */  "new magician offers you magic power",
/* 31 */  "new magic item give you magic power",
/* 32 */  "ores in one iron mine run out",
/* 33 */  "new architect strengthens castle walls",
/* 34 */  "new ores discovered + 4-10 iron one sector",
/* 35 */  "skilled diplomat obtains peace",
/* 36 */  "severe winter",
/* 37 */  "severe winter",
/* 38 */  "tidal wave -- abandon all coastlands ",
/* 39 */  "ninja destroy general staff - 1/2 armies paralyzed",
/* 40 */  "general found to be spy -- many armies paralyzed",
/* 41 */  "general prosperity +20% gold",
/* 42 */  "plague kills 20% of your soldiers",
/* 43 */  "poor conditions kill 20% of your soldiers"
};
#define MAXRANEVENT 43

/*finds unused nation and sets it up partially*/
int
findnew()
{
	int newntn=0,nationis;
	for ( nationis=MAXNTN ; nationis >= 1; nationis--)
		if (ntn[nationis].active == 0) newntn=nationis;
	if (newntn == 0) return (0);
	strcpy(ntn[newntn].leader,"rebel");
	strcpy(ntn[newntn].passwd,ntn[0].passwd);
	ntn[newntn].class=0;
	ntn[newntn].score=0L;
	ntn[newntn].tsctrs=0;
	ntn[newntn].active=2;
	return(newntn);
}

char
getnewmark()
{
	char tmpchr;
	int  done=TRUE,i;
	tmpchr='A'-1;
	while (done) {
		tmpchr++;
		done=FALSE;
		for (i=0;i<MAXNTN;i++)
			if (ntn[i].mark==tmpchr && ntn[i].active>0)
				done=TRUE;
		if (!done && !isupper(tmpchr))
			done=TRUE;
		if (tmpchr=='Z')
			done=FALSE;
	}
	return(tmpchr);
}

/* disolve a certain percent of a nation */
/*returns value of new nation */
int
disolve(percent, target)
int target;
int percent;
{
	int new;	/* new nation number */
	int split;	/* number of sectors split */
	int defaultx=(-1), defaulty=(-1), realx=(-1), realy=(-1), dist;
	int i,j,armynum,narmynum,posi,posj;

	split =  ntn[target].tsctrs * percent / 100;
	if (split==0) {
		strcpy(eventstr,"no sectors to split");
		return(0);
	}
	if (split<=7) {
		strcpy(eventstr,"nation is too small->no sectors will be split");
		return(0);
	}
	/* find starting town */
	for(posi=0; posi<MAPX; posi++) for(posj=0; posj<MAPY; posj++) {
		if((sct[posi][posj].designation == DCITY )
		&&(sct[posi][posj].owner == target)){
			if ( rand()%5 == 0 ) {
				realx = posi;
				realy = posj;
			} else {
				defaultx = posi;
				defaulty = posj;
			}
		}
	}
	if ((realx == (-1) ) && (realy == (-1))){
		realx = defaultx;
		realy = defaulty;
	}
	if ((realx == (-1) ) && (realy == (-1))) {
		strcpy(eventstr,"can't disolve nation->no cities available");
		return(0);
	}
	new=findnew();
	if(new == 0) {
		strcpy(eventstr,"no nations available");
		return(0);
	}

	if(getnewname(new) == 0) return(0);

#ifdef HIDELOC
	sprintf(eventstr,"new nation created");
#else
	sprintf(eventstr,"new nation created at %d,%d",realx,realy);
#endif HIDELOC
	wdisaster(target,realx,realy,0,"revolt");
	sct[realx][realy].owner=new;
	ntn[new].capx=realx;
	ntn[new].capy=realy;
	sct[realx][realy].designation=DCAPITOL;
	ntn[new].race= ntn[target].race;
	ntn[new].tgold= ntn[target].tgold* percent / 100;
	ntn[new].tfood= ntn[target].tfood* percent / 100;
	ntn[new].jewels= ntn[target].jewels* percent / 100;
	ntn[new].tiron= ntn[target].tiron* percent / 100;
	ntn[new].tciv= ntn[target].tciv* percent / 100;
	ntn[new].tmil= ntn[target].tmil* percent / 100;
	ntn[target].tgold -= ntn[new].tgold;
	ntn[target].tfood -= ntn[new].tfood;
	ntn[target].jewels -= ntn[new].jewels;
	ntn[target].tiron -= ntn[new].tiron;
	ntn[target].tciv -= ntn[new].tciv;
	ntn[target].tmil -= ntn[new].tmil;
	ntn[new].repro= ntn[target].repro;
	ntn[new].maxmove= ntn[target].maxmove;
	ntn[new].aplus= ntn[target].aplus;
	ntn[new].dplus= ntn[target].dplus;
	ntn[new].location= ntn[target].location;
	ntn[new].powers= ntn[target].powers;
	ntn[new].tships= 0;
	ntn[new].tsctrs = split;

	/* make the rebellion's mark some unused uppercase letter */
	ntn[new].mark = getnewmark();

	for ( dist=2 ; dist < 10; dist++) if (split > 0)
	for (i=defaultx-dist; i<defaultx+dist; i++)
		for (j=defaulty-dist; j<defaulty+dist; j++){
			if (i>=0 && j>=0 && i<MAPX && j<MAPY
			&&(split>0)
			&&( sct[i][j].designation != DCAPITOL )
			&&( sct[i][j].owner == target)){
					split--;
					sct[i][j].owner=new;
			}
		}

	narmynum=1;
	for (armynum=0;armynum<MAXNAVY;armynum++) {
		ntn[new].nvy[armynum].warships = 0;
		ntn[new].nvy[armynum].merchant = 0;
	}
	for (armynum=0;armynum<MAXARM;armynum++)
		if((ntn[target].arm[armynum].sold>0)
		&&(sct[ntn[target].arm[armynum].xloc][ntn[target].arm[armynum].yloc].owner==new)){
			ntn[new].arm[narmynum].sold
				=ntn[target].arm[armynum].sold;
			ntn[new].arm[narmynum].unittyp
				=ntn[target].arm[armynum].unittyp;
			ntn[new].arm[narmynum].xloc
				=ntn[target].arm[armynum].xloc;
			ntn[new].arm[narmynum].yloc
				=ntn[target].arm[armynum].yloc;
			ntn[new].arm[narmynum].stat
				=ntn[target].arm[armynum].stat;
			ntn[new].arm[narmynum].smove
				=ntn[target].arm[armynum].smove;
			ntn[target].arm[armynum].sold = 0;
			narmynum++;
		}
	for (armynum=narmynum; armynum<MAXARM; armynum++)
		ntn[new].arm[armynum].sold = 0;
	armynum=0;
	ntn[new].arm[0].sold = 300;
	ntn[new].arm[0].unittyp = A_INFANTRY;
	ntn[new].arm[0].xloc = realx;
	ntn[new].arm[0].yloc = realy;
	ntn[new].arm[0].stat = GARRISON;
	ntn[new].arm[0].smove = 0;
	for (dist=0;dist<MAXNTN;dist++) if(dist!=new) {
		/* create realistic diplomatic status */
		if (ntn[target].dstatus[dist]==UNMET) {
			ntn[new].dstatus[dist]=UNMET;
			ntn[dist].dstatus[new]=UNMET;
		}
		else if (ntn[target].dstatus[dist]<NEUTRAL)
		{
			ntn[new].dstatus[dist]=HOSTILE;
			ntn[dist].dstatus[new]=HOSTILE;
		}
		else {
			ntn[new].dstatus[dist]=NEUTRAL;
			ntn[dist].dstatus[new]=NEUTRAL;
		}
	}
	for (dist=MAXNTN;dist<NTOTAL;dist++) {
		ntn[new].dstatus[dist]=WAR;
		ntn[dist].dstatus[new]=WAR;
	}
	ntn[new].dstatus[target]=WAR;
	ntn[target].dstatus[new]=WAR;
	return(new);
}

getnewname(new)
int	new;
{
	int done,count,i=0;

	while( strlen(*(names+i)) > 1 ){
		done = TRUE;
		for (count=0;count<NTOTAL;count++)
			if(strcmp(ntn[count].name, *(names+i))==0) 
				done = FALSE;
		if(done==TRUE) break;
		i++;
	}

	strcpy(ntn[new].name,*(names+i));
	if(done ==TRUE) return(1);
	strcpy(eventstr,"no nations available");
	return(0);
}

int
randomevent()
{
	unsigned char	wierd;		/*because its weird I need to use this*/
	long totalscore=0;
	int count, event, newnation, i, j, armynum,x,y;
	int done, holdval;	/*if 1 then event happened */
	long newpower;
	struct s_sector *sptr;

	printf("RANDOM HAPPENINGS ARE NOW BEING CHECKED\n");

#ifdef VULCANIZE
/* have a volcano erupt on the map based on percent chance of PVULCAN */
	if(rand()%100<PVULCAN) erupt();
#endif

	for(country=0;country<MAXNTN;country++)
		if( ntn[country].active != 0 )
			totalscore+= ntn[country].score;

	/* decide what nations get random event */
	for(country=0;country<MAXNTN;country++)
	if (( ntn[country].active != 0 )
	&&(ntn[country].score > 20L)
	&&((rand()%totalscore) < NORANDEVENTS * ntn[country].score)){
		event = rand()%(MAXRANEVENT+1);
		printf("event %2d",event);
		done=TRUE;
		/* do the event */
		switch(event) {
		case 0:
			/*general/province defects*/
			if(disolve(10, country)==0) done=FALSE;;
			break;
		case 1:
			/*evil wizard sets up -- disolve 10%*/
			if(disolve(10, country)==0) done=FALSE;;
			break;
		case 2:
			/*tax revolt -- disolve 20%*/
			if(disolve(20, country)==0) done=FALSE;;
			break;
		case 3:
			/*rebelion  -- disolve 30%*/
			if(rand()%2==0) { if(disolve(30, country)==0) done=FALSE; }
			else done=FALSE;
			break;
		case 4:
			/*general takes over province -- disolve 30%*/
			if(rand()%2==0) { if(disolve(30, country)==0) done=FALSE; }
			else done=FALSE;
			break;
		case 5:
			/*religious schism -- disolve 30%*/
			if(rand()%2==0) { if(disolve(30, country)==0) done=FALSE; }
			else done=FALSE;
			break;
		case 6:
		case 7:
		case 8:
			/*peasant revolt */
			if((newnation=disolve(10, country)) == 0 ) done=FALSE;
			if(done==TRUE)
			for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
				if(( sct[i][j].owner == country)
				&&( solds_in_sector(i,j,country)==0))
				if(rand()%10<=3) {
					sct[i][j].owner = newnation;
					for(armynum=0;armynum<MAXARM;armynum++)
					if(ASOLD == 0) {
						ASOLD = sct[i][j].people/4;
						ASTAT = A_MILITIA;
						AXLOC = i;
						AYLOC = j;
						sct[i][j].people -= ASOLD;
						break;
					}
				}
			break;
		case 9:
			/*dragon raid -- lose 30% of food*/
				ntn[country].tfood *= 7L;
				ntn[country].tfood /= 10L;
			break;
		case 10:
			/*famine -- food=0 10% starve*/
			ntn[country].tfood /= 4L;
			break;
		case 11:
			/*hurricane*/
			sptr = rand_sector();

			wdisaster(country,xpos,ypos,20,"hurricane");
			sprintf (eventstr,"centered on sector %d, %d", xpos, ypos);
			/* one hex radius */
			for (x=xpos-1;x<=xpos+1;x++)
			for (y=ypos-1;y<=ypos+1;y++) if(ONMAP) {
				sptr = &sct[x][y];
				if ((sptr->designation != DCAPITOL) &&
				(sptr->designation != DCITY))
					sptr->designation = DNODESIG;
				else if (sptr->fortress != 0)
					sptr->fortress--;
				reduce(x, y, 20);
			}
			break;
		case 12:
			/*tornado*/
			sptr = rand_sector();
			sprintf (eventstr, "in sector %d, %d", xpos, ypos);
			if ((sptr->designation != DCAPITOL)
			&& (sptr->designation != DCITY))
				sptr->designation = DNODESIG;
			else {
				if (sptr->fortress < 2) sptr->fortress = 0;
				else sptr->fortress -= 2;
			}
			wdisaster(country,xpos,ypos,25,"tornado");
			reduce(xpos, ypos, 25);
			break;
		case 13:
			/*volcano -- all flee around one mountain -- 30% die*/
			holdval=0;		/* holdval is # of mountains */
			for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
			if(sct[i][j].owner==country && sct[i][j].altitude==MOUNTAIN)
				holdval++;

			if (holdval > 0) count = (rand()%holdval) + 1;
			else count = done = FALSE;
			if( count == FALSE ) break;

			for (i=0; count && (i<MAPX); i++)
			for (j=0;count && (j<MAPY); j++)
			if (( sct[i][j].owner == country)
			&& (sct[i][j].altitude == MOUNTAIN)) {
				count--;
				if (count == 0) {
				blowup(i,j);
				}
			}
			break;
		case 14:
			/*royal wedding (absorb neighbor nation)*/
			/*	takeover ( 100, 0 ); */  
			/* something not right.... */
			done=FALSE;
			break;
		case 15:
			/*new alloy +10% combat (WARRIOR...)*/
			if(magic(country,WARRIOR)!=1){
				ntn[country].powers|=WARRIOR;
				exenewmgk(WARRIOR);
			}
			else if(magic(country,CAPTAIN)!=1){
				ntn[country].powers|=CAPTAIN;
				exenewmgk(CAPTAIN);
			}
			else if(magic(country,WARLORD)!=1){
				ntn[country].powers|=WARLORD;
				exenewmgk(WARLORD);
			}
			else {	/* have all three powers... oh well */
				done=FALSE;
			}
			break;
		case 16:
			/*royal advisor is spy -- lose power*/
			done=FALSE;
			break;
		case 17:
		case 18:
			/*gold strike one sector +4-10 gold*/
			done=FALSE;
			for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
				if(( sct[i][j].owner == country)
				&& (done == FALSE)
				&& (is_habitable(i,j))
				&& (rand()%3 == 0)) {
					wierd = (char) rand()%7;
					wierd+=4;
					sct[i][j].gold += wierd;
					done=TRUE;
					break;
				}
			break;
		case 19:
		case 20:
			/*gold vein runs out one goldmine sector >5 gold */
			for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
				if(( sct[i][j].owner == country)
				&& (sct[i][j].gold >= 5)
				&& (done == FALSE)
				&& (sct[i][j].designation == DGOLDMINE)){
					sct[i][j].gold =0;
					sct[i][j].designation = DFARM;
					done=TRUE;
				}
			break;
		case 21:
			/*flood*/
			done=FALSE;
			break;
		case 22:	/*earthquake*/
			/* get epicenter */
			sptr = rand_sector();
			sprintf (eventstr, "quake in sector %d, %d (owner %s)", xpos, ypos,ntn[country].name);
			/* 10% damage in 3 sectors, 25 in 1, 50 in */
			for (x=xpos-3;x<=xpos+3;x++)
			for (y=ypos-3;y<=ypos+3;y++) if(ONMAP) {
				reduce(x, y, 10);
			}

			for (x=xpos-1;x<=xpos+1;x++)
			for (y=ypos-1;y<=ypos+1;y++) if(ONMAP) {
				reduce(x,y,15);	/* ADDITIONAL 15% */
				if(((rand()%2) == 0)
				&&(sct[x][y].designation!= DCAPITOL)
				&&(sct[x][y].designation!= DCITY))
					sct[x][y].designation=DNODESIG;
			}

			if(is_habitable(xpos,ypos))
				sptr->designation = DDEVASTATED;
			if (sptr->fortress < 2) sptr->fortress = 0;
			else sptr->fortress -= 2;
			reduce(xpos, ypos, 25);	/* ADDITIONAL 25% */
			wdisaster(country,xpos,ypos,50,"earthquake");
			break;
		case 23:
			/*frost -- crops ruined*/
			done=FALSE;
			break;
		case 24:
			/*dragon killed + 50000 jewels*/
			ntn[country].jewels+=50000;
			break;
		case 25:
			/*nomad raid -- put large nomad army in area*/
			done=FALSE;
			for(count=0; count < 100; count++) if(done <= 3){
			i=(rand()%(MAPX-8))+4;
			j=(rand()%(MAPY-8))+4;
			/* get army number */
			armynum = -1;
			for(newpower=0; newpower<MAXARM; newpower++)
				if (ntn[NNOMAD].arm[newpower].sold == 0)
					armynum=newpower;
			if(armynum == -1) done=4;
			else if((is_habitable(i,j))
			&& ( sct[i][j].owner == country)) {
				ntn[NNOMAD].arm[armynum].xloc =i;
				ntn[NNOMAD].arm[armynum].yloc =j;
				if(ntn[country].tmil > 10000)	/* 800-4800 */
				ntn[NNOMAD].arm[armynum].sold =800+50*(rand()%80);
				else if(ntn[country].tmil > 5000) /* 500-2500 */
				ntn[NNOMAD].arm[armynum].sold =500+50*(rand()%40);
				else if(ntn[country].tmil > 1000) /* 400-1400 */
				ntn[NNOMAD].arm[armynum].sold =400+20*(rand()%50);
				else	/* 200-600 */
				ntn[NNOMAD].arm[armynum].sold =200+20*(rand()%20);
				ntn[NNOMAD].arm[armynum].unittyp = A_LT_CAV;
				ntn[NNOMAD].arm[armynum].stat =ATTACK;
				done++;
			}
			}
			done=TRUE;
			break;
		case 26:
			/*town burns -- reduce fort and redesignate*/
			done=FALSE;
			for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
				if(( sct[i][j].owner == country)
				&& ( done == FALSE )
				&& ( sct[i][j].designation == DCITY)){
					sct[i][j].designation = DNODESIG;
					wdisaster(country,i,j,50,"city burned down");
					reduce(i,j,50);
					done=TRUE;
				}
			break;
		case 27:
			/*plague -- 40% of populace dies*/
				for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
					if( sct[i][j].owner == country){
					sct[i][j].people *= 6;
					sct[i][j].people /= 10;
					}
				for (armynum=0; armynum<MAXARM; armynum++)
					if((ASOLD > 0)&&(ATYPE<MINMONSTER)){
						ASOLD *= (6);
						ASOLD /= (10);
					}
			break;
		case 28: /*pirate raid on harbor*/
			done=FALSE;
			break;
		case 29: /*barbarian raid*/
			done=FALSE;
			break;
		case 30: /*new magician + RANDOM POWER*/
			/*buy new powers and/or new weapons*/
			if((newpower=getmagic(M_CIV))!=0L){
				for(i=S_CIV;i<=E_CIV;i++) if(powers[i]==newpower){
				printf("\tnation %s gets power %s\n",ntn[country].name,pwrname[newpower]);
				fprintf(fnews,"1. \tevent in %s->gets power %s\n", ntn[country].name,pwrname[newpower]);
				}
				exenewmgk(newpower);
			}
			else done=FALSE;
			break;
		case 31: /*new magic item + RANDOM POWER*/
			/*buy new powers and/or new weapons*/
			if((newpower=getmagic(M_MIL))!=0){
				for(i=S_MIL;i<=E_MIL;i++) if(powers[i]==newpower){
				printf("\tnation %s gets power %s\n",ntn[country].name,pwrname[newpower]);
				fprintf(fnews,"1. \tevent in %s->gets power %s\n", ntn[country].name,pwrname[newpower]);
				}
				exenewmgk(newpower);
			}
			else done=FALSE;
			break;
		case 32:
			/* ores run out */
			done=FALSE;
			for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
			if(( sct[i][j].owner == country)
				&& (sct[i][j].iron >= 5)
				&& (done == FALSE)
				&& (sct[i][j].designation == DMINE)){
					sct[i][j].iron =0;
					sct[i][j].designation = DNODESIG;
					done=TRUE;
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
			done=FALSE;
			for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
				if(( sct[i][j].owner == country)
				&& (done == FALSE)
				&& (is_habitable(i,j))
				&& (sct[i][j].iron != 0)){
					sct[i][j].iron += 4;
					wierd = (char) rand()%7;
					sct[i][j].iron += wierd;
					done=TRUE;
				}
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
			done=FALSE;
			break;
		case 38:
			/*tidal wave -- abandon all coastlands */
			done=FALSE;
			break;
		case 39: /*ninja attack paralyzes half your armys AMOVE=0*/
		case 40: /*general found to be spy AMOVE=0*/
			for(armynum=0; armynum<MAXARM; armynum++) if(rand()%2==0)
				AMOVE = 0;
			break;
		case 41:
			/*general prosperity +20% gold*/
			if (ntn[country].tgold > 0l) {
				ntn[country].tgold *= 12l;
				ntn[country].tgold /= 10l;
			}
			else done=FALSE;
			break;
		case 42:
		case 43:
			/*kill 20% of armies*/
			for (armynum=0; armynum<MAXARM; armynum++)
				if((ASOLD > 0) && (ATYPE<MINMONSTER)){
					ASOLD *= (8);
					ASOLD /= (10);
				}
			break;
		default:
			printf("error condition -- illegal random event\n");
			break;
		}
		if(done==TRUE) fprintf(fnews,"1. \tevent in %s - %s\n"
			,ntn[country].name,randevents[event]);
		if(done==TRUE) printf("\t%s -- %s\n"
			,ntn[country].name,randevents[event]);
		else printf("\t%s -- (unimplemented) %s\n"
			,ntn[country].name,randevents[event]);
		if(strlen(eventstr)>5) {
			printf("\t\t->%s\n",eventstr);
			if(done==TRUE) fprintf(fnews,"1. \tevent in %s -->%s\n"
				,ntn[country].name,eventstr);
		}
		strcpy(eventstr,"");
	}
}

#ifdef VULCANIZE
/*volcano erupts --- causes devastation in surrounding sectors */
void
erupt()
{
	int i, j, nvolcanos=0, volhold;

	printf("checking for volcanic eruptions\n");
	/* count all of the volcanos */
	for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
		if(sct[i][j].vegetation == VOLCANO) nvolcanos++;

	if(nvolcanos==0) {
		printf("no volcano's found\n");
		return;
	}
	/* choose a random one to erupt */
	volhold = rand()%nvolcanos;
	for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
	if ( sct[i][j].vegetation == VOLCANO ) {
		volhold--;
		if (volhold == 0) blowup(i,j);
	}
}
#endif VULCANIZE

/* blowup a volcano in sector i,j */
void
blowup(i,j)
register int i,j;
{
	register int x,y;
	wdisaster(sct[i][j].owner,i,j,100,"volcano erupted");
	printf("\tvolcano at %d, %d erupts; devastates surounding area\n",i,j);
	fprintf(fnews,"1. \tevent in sector %d, %d->volcanic eruption causes devastation\n",i,j);
	sct[i][j].vegetation = VOLCANO;
	sct[i][j].gold = 0;
	sct[i][j].iron = 0;
	reduce(i,j,100);
	if(is_habitable(i,j))
		sct[i][j].designation = DDEVASTATED;
	sct[i][j].fortress = 0;
	/* decrease neighboring population and armies 30% */
	for(x=i-1; x<=i+1; x++) for(y=j-1; y<=j+1; y++)
	if((ONMAP)&&(sct[x][y].altitude != WATER)) {
		reduce(x,y,30);
		if(is_habitable(x,y))
			sct[x][y].designation = DDEVASTATED;
		sct[x][y].fortress = 0;
	}
}

/** reduce will drop armies & and civilians in sector by percent **/
void
reduce(x,y,percent)
int x,y,percent;
{
	long temp;	/* used to avoid overflow problems */
	int armynum,ctry;

	percent = 100 - percent;	/* invert percent so math works */

	/* work on people */
	temp = sct[x][y].people;
	temp *= percent;
	temp /= 100;
	sct[x][y].people = temp;

	/* work on armies */
	for(ctry=1;ctry<MAXNTN;ctry++) {
		for(armynum=0;armynum<MAXARM;armynum++)
		if((ntn[ctry].arm[armynum].xloc==x)
		&&(ntn[ctry].arm[armynum].unittyp<MINMONSTER)
		&&(ntn[ctry].arm[armynum].yloc==y)) {
			temp = ntn[ctry].arm[armynum].sold;
			temp *= percent;
			temp /= 100;
			ntn[ctry].arm[armynum].sold = temp;
		}
	}
}

/* returns pointer to random sector in country */
struct s_sector
*rand_sector()
{
	int count=0;
	for(xpos=0;xpos<MAPX;xpos++) for(ypos=0;ypos<MAPX;ypos++)
		if(sct[xpos][ypos].owner == country) count++;
	count = rand()%count;
	for(xpos=0;xpos<MAPX;xpos++) for(ypos=0;ypos<MAPX;ypos++){
		if(sct[xpos][ypos].owner == country) count--;
		if(count==0) return(&sct[xpos][ypos]);
	}
	abrt();
}

/*global disaster report --- tell 'em where it hit */
wdisaster(cntry,xloc,yloc,prcnt,event)
int cntry,xloc,yloc,prcnt;
char *event;
{
	char line[100];
	FILE *fp, *fopen();
	char realname[12];

	/*send a message to the country if it is a PC*/
	if (ntn[country].active!=1) {
		return;
	}
	strcpy(realname,ntn[cntry].name);
	sprintf(line,"%s%d",msgfile,cntry);
	if((fp=fopen(line,"a+"))==NULL) {
		printf("\nError in writing disaster report.");
		abrt();
	}
	fprintf(fp,"%s GLOBAL DISASTER REPORT\n",realname);
	fprintf(fp,"%s \n",realname);
	fprintf(fp,"%s\tDisaster occurs within nation %s:\n",realname,realname);
	fprintf(fp,"%s\tA %s struck during the day centered around location %d %d.\n",realname,event,xloc,yloc);
	if(prcnt>0) {
		fprintf(fp,"%s\tDamage was estimated at about %d%% in severity.\n",realname,prcnt);
	}
	fputs("END\n",fp);
	fclose(fp);
}
#endif ADMIN
#endif RANEVENT
