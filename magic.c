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

/*create a new login for a new player*/
#include "header.h"
#include "data.h"
#include <ctype.h>

extern short country;
extern short redraw;
extern FILE *fexe;
extern FILE *fnews;

/*give player one new magic power in current magic (powers)*/
/*do nothing if that player has that power or it is not permitted*/
/*getmagic() returns the value of the power gained, and stores it in power*/

long
getmagic(type)
int type;
{
	long newpower;
	int start,end;
	switch(type){
		case M_MGK:
			start=S_MGK;
			end=E_MGK;
			break;
		case M_CIV:
			start=S_CIV;
			end=E_CIV;
			break;
		case M_MIL:
			start=S_MIL;
			end=E_MIL;
			break;
		default:
			printf("fatal error in num_powers");
			abrt();
	}
	newpower=powers[start+(rand()%end)];
	if(newpower==0) {
		printf("ILLEGAL POWER");
		abrt();
	}

	if((newpower==WARRIOR)||(newpower==CAPTAIN)||(newpower==WARLORD)){
		if(magic(country,WARRIOR)!=TRUE){
			ntn[country].powers|=WARRIOR;
			return(WARRIOR);
		}
		else if(magic(country,CAPTAIN)!=TRUE){
			ntn[country].powers|=CAPTAIN;
			return(CAPTAIN);
		}
		else if(magic(country,WARLORD)!=TRUE){
			ntn[country].powers|=WARLORD;
			return(WARLORD);
		}
		else return(0L);
	}else if((newpower==MI_MONST) ||(newpower==AV_MONST) ||(newpower==MA_MONST)){
		if(ntn[country].race!=ORC) return(0L);
		if(magic(country,MI_MONST)!=TRUE){
			ntn[country].powers|=MI_MONST;
			return(MI_MONST);
		}
		else if(magic(country,AV_MONST)!=TRUE){
			ntn[country].powers|=AV_MONST;
			return(AV_MONST);
		}
		else if(magic(country,MA_MONST)==TRUE){
			ntn[country].powers|=MA_MONST;
			return(MA_MONST);
		}
		else return(0L);
	}else if(newpower==CAVALRY){
		if(ntn[country].active >= 2) return(0L);	/*npc nation*/
		if(magic(country,newpower)==TRUE) return(0L);
		ntn[country].powers|=newpower;
		return(newpower);
	}else if(newpower==URBAN){
		if(magic(country,BREEDER)==TRUE) return(0L);
		if(magic(country,newpower)==TRUE) return(0L);
		ntn[country].powers|=newpower;
		return(newpower);
	}else if(newpower==HEALER){
		if(ntn[country].race==ORC) return(0L);
		if(magic(country,newpower)==TRUE) return(0L);
		ntn[country].powers|=newpower;
		return(newpower);
	}else if(newpower==KNOWALL){
#ifdef OGOD
	     /* only god should have KNOWALL if sectors hidden */
	     if(country!=0) return(0L);
#endif
	     if(magic(country,KNOWALL)==TRUE) return(0L);
	     ntn[country].powers|=KNOWALL;
	     return(KNOWALL);
	}else if((newpower==SLAVER)
	||(newpower==DERVISH)
	||(newpower==HIDDEN)
	||(newpower==ARCHITECT)
	||(newpower==THE_VOID)
	||(newpower==ARCHER)){
		if(magic(country,newpower)==TRUE) return(0L);
		ntn[country].powers|=newpower;
		return(newpower);
	}else if(newpower==DESTROYER){
		if((ntn[country].race!=ELF)&&(magic(country,DESTROYER)!=TRUE)){
			ntn[country].powers|=DESTROYER;
			return(DESTROYER);
		}
		return(0L);
	}else if(newpower==VAMPIRE){
		if((ntn[country].race!=ELF)&&(magic(country,VAMPIRE)!=TRUE)){
			ntn[country].powers|=VAMPIRE;
			return(VAMPIRE);
		}
		return(0L);
	}else if(newpower==MINER){
		if((ntn[country].race!=ELF)&&(ntn[country].race!=DWARF)&&(magic(country,MINER)!=TRUE)){
			ntn[country].powers|=MINER;
			return(MINER);
		}
		return(0L);
	}else if(newpower==STEEL){
		if(magic(country,STEEL)==TRUE) return(0L);
		if(magic(country,MINER)!=TRUE) return(0L);
		ntn[country].powers|=STEEL;
		return(STEEL);
	}else if(newpower==BREEDER){
		if(magic(country,URBAN)==TRUE) return(0L);
		if(magic(country,BREEDER)==TRUE) return(0L);
		if(ntn[country].race!=ORC) return(0L);
		ntn[country].powers|=BREEDER;
		return(BREEDER);
	}
	else if(ntn[country].active >= 2) {
		return(0L);	/* remaining powers only for pc's */
	} else if((newpower==NINJA)
	||(newpower==SLAVER)
	||(newpower==SAILOR)
	||(newpower==DEMOCRACY)
	||(newpower==ROADS)
	||(newpower==SAPPER)
	||(newpower==ARMOR)
	||(newpower==AVIAN)){	
		if(magic(country,newpower)==TRUE) return(0L);
		ntn[country].powers|=newpower;
		return(newpower);
	}
	else if((newpower==SUMMON)||(newpower==WYZARD)||(newpower==SORCERER)){
		if(magic(country,SUMMON)!=TRUE) {
			ntn[country].powers|=SUMMON;
			return(SUMMON);
		} else if(magic(country,WYZARD)!=TRUE) {
			ntn[country].powers|=WYZARD;
			return(WYZARD);
		} else if(magic(country,SORCERER)!=TRUE) {
			ntn[country].powers|=SORCERER;
			return(SORCERER);
		} else return(0L);
	} else return(0L);
}
#ifdef CONQUER
/*form to interactively get a magic power*/
void
domagic()
{
	int count, done=FALSE, loop=0, i,type;
	long price,x;
	short isgod=0;
	if(country==0) {
		isgod=1;
		clear();
		mvaddstr(0,0,"WHAT NATION NUMBER:");
		refresh();
		country = get_number();
	}

	while(done==FALSE){
		done=TRUE;
		clear();
		count=3;
		redraw=TRUE;
		standout();
		mvprintw(0,(COLS/2)-15,"MAGIC POWERS FOR %s",ntn[country].name);
		mvprintw(count++,30,"1) %d military powers: %ld jewels",
			num_powers(country,M_MIL) ,getmgkcost(M_MIL,country));
		mvprintw(count++,30,"2) %d civilian powers: %ld jewels",
			num_powers(country,M_CIV) ,getmgkcost(M_CIV,country));
		mvprintw(count++,30,"3) %d magic powers:    %ld jewels",
			num_powers(country,M_MGK),getmgkcost(M_MGK,country));

		price =  getmgkcost(M_MIL,country);
		if(price > getmgkcost(M_CIV,country))
			price = getmgkcost(M_CIV,country);
		if(price > getmgkcost(M_MGK,country))
			price = getmgkcost(M_MGK,country);

		standend();
		count=3;
		/*print the powers that you have*/
		i=0;
		while( powers[i] != 0 ){
			if(magic(country,powers[i])==TRUE)
			mvprintw(count++,0,"you have power %s",*(pwrname+i));
			i++;
		}

		if(count<=7) count=8;
		else count++;
		standout();
		mvprintw(count++,0,"YOU HAVE %ld JEWELS IN YOUR TREASURY",ntn[country].jewels);
		if(price < ntn[country].jewels){
		mvaddstr(count++,0,"DO YOU WISH TO BUY A RANDOM NEW POWER (enter y or n):");
		standend();
		refresh();
		count++;
		if(getch()=='y'){
			done=FALSE;
			mvprintw(count++,0,"ENTER SELECTION (1,2,3):");
			refresh();
			type = get_number();
			if(type==M_MIL || type==M_CIV || type==M_MGK){
			price=getmgkcost(type,country);
#ifdef OGOD
			if (isgod==TRUE) price=0;
#endif OGOD
			if(ntn[country].jewels>=price) {
				loop = 0;
				while(loop++ < 500) if((x=getmagic(type))!=0){
					ntn[country].jewels -= price;
					CHGMGK;
					exenewmgk(x);
					refresh();
					if (isgod==TRUE) country=0;
					return;
				}
				if (loop >= 500)
					mvaddstr(count++,0,"You have too many powers! -- hit any key");
					refresh();
					getch();
			} else {
			mvaddstr(count++,0,"NOT ENOUGH JEWELS TO PURCHASE NEW MAGIC -- hit any key");
			refresh();
			getch();
			}
			} else {
			mvaddstr(count++,0,"INVALID SELECTION -- hit any key");
			refresh();
			getch();
			}
		}
		} else {
			mvaddstr(count++,0,"CAN NOT BUY NEW POWER - hit any key");
			standend();
			refresh();
			getch();
		}
		if(magic(country,SUMMON)==TRUE)
			done=dosummon(&count);
#ifdef ORCTAKE
		if((ntn[country].race==ORC)&&(ntn[country].jewels>=TAKEPRICE))
			done=orctake(&count);
#endif ORCTAKE
#ifdef OGOD
		if (isgod==TRUE) {
			mvaddstr(count++,0,"GOD: REMOVE A MAGIC POWER? (y or n)");
			refresh();
			if (getch()=='y') killmagk();
		}
#endif OGOD
	}
	if(isgod==1) country=0;
}
#endif CONQUER
/*do magic for both npcs and pcs in update*/
/*if target is 0 then it is update and target will be picked randomly*/
int
takeover(percent,target)
int percent,target;
{
	int loop=1,y,save,isupdate=0;
	save=country;
	if(target==country) return(0);
	if(target==0) isupdate=1;
	country=target;
	if(rand()%100<percent){
		loop=0;
		y=0;
		if (target==0) while(loop==0){
			y++;
			country=rand()%MAXNTN;
			if((ntn[country].race==ntn[save].race)
			&&(ntn[country].active>=2)
			&&(country!=save)) loop=1;
			else if(y>=500) {
				country=save;
				return(0);
			}
		}
		sct[ntn[country].capx][ntn[country].capy].owner=save;
		if(isupdate==1){
		printf("nation %s taken over by %s\n",ntn[country].name,ntn[save].name);
		fprintf(fnews,"1.\tnation %s taken over by %s\n",ntn[country].name,ntn[save].name);
		}
		else {
			DESTROY;
			if ((fnews=fopen("/dev/null","w"))==NULL){
				printf("error opening null file\n");
				exit(FAIL);
			}
		}
		destroy(country);
		if(isupdate!=1) fclose(fnews);
		y=country;
		country=save;
		return(y);
	}
	country=save;
	return(0);
}

/*execute new magic*/
long
exenewmgk(newpower)
long newpower;
{
	short x,y;
	if(newpower==WARRIOR) {
		ntn[country].aplus+=10;
		ntn[country].dplus+=10;
		return(0L);
	}
	if(newpower==CAPTAIN) {
		ntn[country].aplus+=10;
		ntn[country].dplus+=10;
		return(0L);
	}
	if(newpower==WARLORD) {
		ntn[country].aplus+=10;
		ntn[country].dplus+=10;
		return(0L);
	}
	if(newpower==HEALER) {
		if(ntn[country].race==ORC) {
			printf("ORCS CANT HAVE HEALER POWER\n");
			abrt();
		} else if(ntn[country].repro<=8){
			ntn[country].repro+=2;
		} else if(ntn[country].repro==9){
			ntn[country].repro=10;
			ntn[country].dplus+=5;
		} else if(ntn[country].repro>=10){
			ntn[country].dplus+=10;
		}
		return(0L);
	}
	if(newpower==DESTROYER) {
		for(x=ntn[country].capx-3;x<=ntn[country].capx+3;x++) {
			for(y=ntn[country].capy-3;y<=ntn[country].capy+3;y++){
				if((ONMAP)
				&&(sct[x][y].altitude!=WATER)
#ifdef DERVDESG
				&&((rand()%2)==0)
#else
				&&(tofood(sct[x][y].vegetation,0)<DESFOOD)
#endif DERVDESG
				&&((x!=ntn[country].capx)
					||(y!=ntn[country].capy))){
					sct[x][y].vegetation=DESERT;
					sct[x][y].designation=DNODESIG;
				}
			}
		}
		updmove(ntn[country].race,country);
		return(0L);
	}
	if(newpower==DERVISH) {
		updmove(ntn[country].race,country);
		return(0L);
	}
	if((newpower==MI_MONST)
	||(newpower==AV_MONST)
	||(newpower==MA_MONST)
	||(newpower==KNOWALL)
	||(newpower==HIDDEN)
	||(newpower==THE_VOID)
	||(newpower==ARCHITECT)
	||(newpower==MINER))
		return(0L);
	if(newpower==VAMPIRE) {
		ntn[country].aplus-=35;
		ntn[country].dplus-=35;
		ntn[country].maxmove-=2;
		return(0L);
	}
	if(newpower==URBAN) {
		if(ntn[country].race==ORC) {
			x=ntn[country].repro;
			if(ntn[country].repro>=13){
				ntn[country].maxmove+=4;
			}
			else if(ntn[country].repro>10){
				ntn[country].maxmove+=2*(x-10);
				ntn[country].repro=13;
			}
			else ntn[country].repro+=3;
		}
		else if(ntn[country].repro<=9){
			ntn[country].repro+=3;
		}
		else {
			ntn[country].maxmove+=2*(ntn[country].repro-9);
			ntn[country].repro=12;
		}
		return(0L);
	}
	if(newpower==BREEDER) {
		ntn[country].repro+=3;
		ntn[country].dplus-=10;
		ntn[country].aplus-=10;
		return(0L);
	}
	if(newpower==DEMOCRACY){
		ntn[country].repro+=1;
		ntn[country].dplus+=10;
		ntn[country].aplus+=10;
		return(0L);
	}
	if(newpower==ROADS){
		ntn[country].maxmove+=4;
		return(0L);
	}
	if(newpower==ARMOR){
		ntn[country].maxmove-=3;
		if( ntn[country].maxmove<4) ntn[country].maxmove=4;
		ntn[country].dplus+=20;
	}
	if((newpower==NINJA)
	||(newpower==STEEL)
	||(newpower==ARCHER)
	||(newpower==CAVALRY)
	||(newpower==SAILOR)
	||(newpower==SUMMON)
	||(newpower==WYZARD)
	||(newpower==SORCERER)
	||(newpower==SAPPER)
	||(newpower==AVIAN)){	/* these powers are only for pc's */
		return(1L);
	}
	return(0L);
}
#ifdef CONQUER
/* returns 0 if summon occurred, 1 else */
int
dosummon(count)
int *count;
{
	int done=TRUE,x,i,armynum;
	long e_cost;
	int newtype,s_cost;
	if((*count)>20) {
		(*count)=2;
		clear();
	}
	mvaddstr((*count)++,0,"YOU HAVE SUMMON POWER");
	mvaddstr((*count)++,0,"DO YOU WISH SUMMON A MONSTER (enter y or n):");
	refresh();
	if(getch()=='y'){
		done=FALSE;
		x=0;
		mvaddstr((*count)++,x,"options:");
		x+=9;
		for(i=MINMONSTER;i<=MAXMONSTER;i++){
			if(unitvalid(i)==TRUE) {
				mvprintw((*count),x+2,"%s",*(shunittype+(i%200)));
				mvprintw((*count),x,"(%c)",*(shunittype+(i%200))[0]);
				x+=7;
				if(x>COLS-20){
					x=0;
					(*count)++;
				}
			}
		}
		(*count)++;
		mvaddstr((*count)++,0,"what type of unit do you want to raise:");
		refresh();

		newtype='0';
		switch(getch()){
		case 's':
			newtype=SPIRIT;
			break;
		case 'A':
			newtype=ASSASSIN;
			break;
		case 'e':
			newtype=DJINNI;
			break;
		case 'G':
			newtype=GARGOYLE;
			break;
		case 'W':
			newtype=WRAITH;
			break;
		case 'H':
			newtype=HERO;
			break;
		case 'C':
			newtype=CENTAUR;
			break;
		case 'g':
			newtype=GIANT;
			break;
		case 'S':
			newtype=SUPERHERO;
			break;
		case 'M':
			newtype=MUMMY;
			break;
		case 'E':
			newtype=ELEMENTAL;
			break;
		case 'm':
			newtype=MINOTAUR;
			break;
		case 'd':
			newtype=DEMON;
			break;
		case 'B':
			newtype=BALROG;
			break;
		case 'D':
			newtype=DRAGON;
			break;
		default:
			break;
		}

		if(unitvalid(newtype)==FALSE) {
			beep();
			mvprintw((*count)++,0,"%d INVALID TYPE",newtype);
			refresh();
			sleep(2);
			return(done);
		}

		e_cost= (long) *(u_encost+(newtype%200)) * *(unitminsth+(newtype%200));
		s_cost= *(u_encost+(newtype%200));
		/*check to see if enough spell points*/
		if(s_cost > ntn[country].spellpts) {
			mvprintw((*count)++,0,"you dont have %d spell points",s_cost);
			refresh();
			sleep(1);
			return(done);
		}

		/*check to see if enough gold*/
		if(e_cost >  ntn[country].tgold) {
			mvprintw((*count)++,0,"you dont have %ld gold in treasury",e_cost);
			refresh();
			sleep(1);
			return(done);
		}

		armynum=0;
		while(armynum<MAXARM) {
			if(ASOLD<=0) {
				ASOLD= *(unitminsth+(newtype%200));
				ATYPE=newtype;
				ASTAT=DEFEND; /* set new armies to DEFEND */
				AXLOC=ntn[country].capx;
				AYLOC=ntn[country].capy;
				AMOVE=0;
				AADJLOC;
				AADJSTAT;
				AADJMEN;
				AADJMOV;
				armynum=MAXARM;
			} else if(armynum==MAXARM-1) {
				mvaddstr((*count)++,0,"NO FREE ARMIES");
				refresh();
				sleep(2);
				return(done);
			} else armynum++;
		}
		ntn[country].tgold -= e_cost;
		ntn[country].spellpts -= s_cost;
		EDECSPL;
	}
	return(done);
}
#endif CONQUER
#ifdef CONQUER
#ifdef ORCTAKE
/* orc takeover routine... returns 0 if run, 1 if not */
orctake(count)
int *count;
{
	int chance=0,done=TRUE,i;
	if((*count)>20) {
		(*count)=2;
		clear();
	}
	if(magic(country,MA_MONST)==TRUE) {
	mvprintw((*count)++,0,"  You have a 10 percent chance for %ld Jewels take over other orcs",TAKEPRICE);
	chance=10;
	} else if(magic(country,AV_MONST)==TRUE) {
	mvprintw((*count)++,0,"  You have a 6 percent chance for %ld Jewels take over other orcs",TAKEPRICE);
	chance=6;
	} else if(magic(country,MI_MONST)==TRUE){
	mvprintw((*count)++,0,"  You have a 3 percent chance for %ld Jewels to take over other orcs",TAKEPRICE);
	chance=3;
	}
	if(chance==0) return(TRUE);

	mvaddstr((*count)++,0,"DO YOU WISH TO TAKE OVER AN ORC NPC NATION (enter y or n):");
	refresh();
	if(getch()=='y'){
		done=FALSE;
		mvaddstr((*count)++,0,"  What orc nation (number):");
		refresh();
		i=get_number();
		if(ntn[i].race==ORC){
			ntn[country].jewels-=TAKEPRICE;
			if((i=takeover(chance,i))==1)
			mvprintw((*count)++,0," Successful: %d",i);
		}
		else mvaddstr((*count)++,0,"  Wrong Race");
	}
	return(done);
}
#endif ORCTAKE
#endif CONQUER
#ifdef CONQUER
/* unitvalid tells if nation has powers needed to draft unit */
int
unitvalid(type)
int type;
{
	int valid=FALSE;
	switch(type){
		case GARGOYLE:
		case A_GOBLIN:
		case A_ORC:	if(magic(country,MI_MONST)==TRUE) valid=TRUE;
				break;
		case A_MARINES: if(magic(country,SAILOR)==TRUE) valid=TRUE;
				break;
		case A_ARCHER:	if(magic(country,ARCHER)==TRUE) valid=TRUE;
				break;
		case A_URUK:	if(magic(country,AV_MONST)==TRUE) valid=TRUE;
				break;
		case A_NINJA:	if(magic(country,NINJA)==TRUE) valid=TRUE;
				break;
		case A_PHALANX:	if(magic(country,CAPTAIN)==TRUE) valid=TRUE;
				break;
		case A_OLOG:	if((magic(country,BREEDER)==TRUE)
				&&(magic(country,AV_MONST)==TRUE)) valid=TRUE;
				break;
		case A_ELEPHANT:if(magic(country,DERVISH)==TRUE)  valid=TRUE;
				break;
		case SUPERHERO:
		case A_LEGION:	if(magic(country,WARLORD)==TRUE) valid=TRUE;
				break;
		case A_DRAGOON:	if(magic(country,CAVALRY)==TRUE) valid=TRUE;
				break;
		case A_TROLL:	if(magic(country,MA_MONST)==TRUE) valid=TRUE;
				break;
		case A_ELITE:	if(magic(country,STEEL)==TRUE) valid=TRUE;
				break;
		case A_LT_CAV:
		case A_CAVALRY:	if(magic(country,CAVALRY)==TRUE) valid=TRUE;
				break;
		case A_KNIGHT:	if((magic(country,ARMOR)==TRUE)
				&&(magic(country,CAVALRY)==TRUE)) valid=TRUE;
				break;
		case A_ROC:
		case A_GRIFFON: if(magic(country,AVIAN)==TRUE) valid=TRUE;
				break;
		case ASSASSIN:	if(magic(country,NINJA)==TRUE) valid=TRUE;
				break;
		case DJINNI:	if(magic(country,DERVISH)==TRUE) valid=TRUE;
				break;
		case HERO:	if(magic(country,WARRIOR)==TRUE) valid=TRUE;
				break;
		case ELEMENTAL:	if(magic(country,SORCERER)==TRUE) valid=TRUE;
				break;
		case WRAITH:
		case MUMMY:	if(magic(country,VAMPIRE)==TRUE) valid=TRUE;
				break;
		case MINOTAUR:
		case DEMON:	if(magic(country,DESTROYER)==TRUE) valid=TRUE;
				break;
		case BALROG:	if((magic(country,WYZARD)==TRUE)
				&&(magic(country,VAMPIRE)==TRUE)) valid=TRUE;
				break;
		case DRAGON:	if((magic(country,MA_MONST)==TRUE)
				&&(magic(country,WYZARD)==TRUE)) valid=TRUE;
				break;
		default:	valid=TRUE;	/* for all unrestricted types */
	}
	return(valid);
}
#endif CONQUER

#ifdef OGOD
/*remove a magic power*/
long
removemgk(oldpower)
long oldpower;
{
	short x,y;
	if((oldpower==WARRIOR)
	||(oldpower==CAPTAIN)
	||(oldpower==WARLORD)) {
		ntn[country].aplus-=10;
		ntn[country].dplus-=10;
		return(0L);
	}
	if(oldpower==HEALER) {
		if(ntn[country].race==ORC) {
			printf("ORCS CANT HAVE HEALER POWER\n");
			abrt();
		} else ntn[country].repro -= 2;
		return(0L);
	}
	if(oldpower==DESTROYER) {
		for(x=ntn[country].capx-3;x<=ntn[country].capx+3;x++) {
			for(y=ntn[country].capy-3;y<=ntn[country].capy+3;y++){
				if((ONMAP)
				&&(sct[x][y].altitude!=WATER)
				&&((x!=ntn[country].capx)
					||(y!=ntn[country].capy))){
					if (sct[x][y].vegetation==DESERT)
					{
						/* LT_VEG has medium value*/
						sct[x][y].vegetation=LT_VEG;
						sct[x][y].designation=DNODESIG;
					}
				}
			}
		}
		updmove(ntn[country].race,country);
		return(0L);
	}
	if(oldpower==DERVISH) {
		updmove(ntn[country].race,country);
		return(0L);
	}
	if((oldpower==MI_MONST)
	||(oldpower==AV_MONST)
	||(oldpower==MA_MONST)
	||(oldpower==KNOWALL)
	||(oldpower==HIDDEN)
	||(oldpower==THE_VOID)
	||(oldpower==ARCHITECT)
	||(oldpower==MINER))
		return(0L);
	if(oldpower==VAMPIRE) {
		ntn[country].aplus+=35;
		ntn[country].dplus+=35;
		ntn[country].maxmove+=2;
		return(0L);
	}
	if(oldpower==URBAN) {
		ntn[country].repro -= 3;
		return(0L);
	}
	if(oldpower==BREEDER) {
		ntn[country].repro-=3;
		ntn[country].dplus+=10;
		ntn[country].aplus+=10;
		return(0L);
	}
	if(oldpower==DEMOCRACY){
		ntn[country].repro-=1;
		ntn[country].dplus-=10;
		ntn[country].aplus-=10;
		return(0L);
	}
	if(oldpower==ROADS){
		ntn[country].maxmove-=4;
		return(0L);
	}
	if(oldpower==ARMOR){
		ntn[country].maxmove+=3;
		ntn[country].dplus-=20;
	}
	if((oldpower==NINJA)
	||(oldpower==STEEL)
	||(oldpower==ARCHER)
	||(oldpower==CAVALRY)
	||(oldpower==SAILOR)
	||(oldpower==SUMMON)
	||(oldpower==WYZARD)
	||(oldpower==SORCERER)
	||(oldpower==SAPPER)
	||(oldpower==AVIAN)){	/* these powers are only for pc's */
		return(1L);
	}
	return(0L);
}

#ifdef CONQUER
/* killmagk: this routine removes a magic power */
killmagk()
{
	int count,choice,i;
	long holdmagk;

	clear();
	count=3;
	standout();
	mvprintw(0,(COLS/2)-15,"MAGIC POWERS FOR %s",ntn[country].name);
	standend();
	i=0;
	while( powers[i] != 0 ){
		if(magic(country,powers[i])==TRUE)
		mvprintw(count,0,"%d: power %s",count-2,*(pwrname+i));
		count++;
		i++;
	}
	count++;
	standout();
	mvaddstr(count++,5," Which power to remove? ");
	standend();
	refresh();
	choice=get_number();
	if (choice>count-5) choice=0;
	mvprintw(count++,0," Remove magic #%d? (y or [n])",choice);
	refresh();
	if ((getch()=='y')&&(choice!=0)) {
		i=0;
		if (magic(country,holdmagk=powers[i])==TRUE) choice--;
		while(choice) {
			i++;
			if (magic(country,holdmagk=powers[i])==TRUE)
		choice--;
		}
		ntn[country].powers ^= holdmagk;
		removemgk(holdmagk);
	}
}
#endif CONQUER
#endif OGOD
