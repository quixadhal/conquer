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

getmagic()
{
int newpower;
  	newpower=1<<rand()%MAXPOWER;
	switch(newpower){
	case WARRIOR:
	case CAPTAIN:
	case WARLORD:
		if(magic(country,WARRIOR)!=1){
			ntn[country].powers|=WARRIOR;
			return(WARRIOR);
		}
		else if(magic(country,CAPTAIN)!=1){
			ntn[country].powers|=CAPTAIN;
			return(CAPTAIN);
		}
		else if(magic(country,WARLORD)!=1){
			ntn[country].powers|=WARLORD;
			return(WARLORD);
		}
		break;
	case MI_MONST:
	case AV_MONST:
	case MA_MONST:
		if(ntn[country].race!=ORC) return(0);
		if(magic(country,MI_MONST)!=1){
			ntn[country].powers|=MI_MONST;
			return(MI_MONST);
		}
		else if(magic(country,AV_MONST)!=1){
			ntn[country].powers|=AV_MONST;
			return(AV_MONST);
		}
		else if(magic(country,MA_MONST)==1){
			ntn[country].powers|=MA_MONST;
			return(MA_MONST);
		}
		break;
	case SPY:
	case KNOWALL:
	case DERVISH:
	case HIDDEN:
	case ARCHITECT:
	case URBAN:
	case THE_VOID:
	case HEALER:
	case ARCHER:
	case CAVALRY:
		if(magic(country,newpower)==1) return(0);
		ntn[country].powers|=newpower;
		return(newpower);
	case DESTROYER:
		if((ntn[country].race!=ELF)&&(magic(country,DESTROYER)!=1)){
			ntn[country].powers|=DESTROYER;
			return(DESTROYER);
		}
		break;
	case VAMPIRE:
		if((ntn[country].race!=ELF)&&(magic(country,VAMPIRE)!=1)){
			ntn[country].powers|=VAMPIRE;
			return(VAMPIRE);
		}
		break;
	case MINER:
		if((ntn[country].race!=ELF)&&(ntn[country].race!=DWARF)&&(magic(country,MINER)!=1)){
			ntn[country].powers|=MINER;
			return(MINER);
		}
		break;
	case STEEL:
		if(magic(country,STEEL)==1) return(0);
		if(magic(country,MINER)!=1) return(0);
		ntn[country].powers|=STEEL;
		return(STEEL);
	case BREEDER:
		if(magic(country,BREEDER)==1) return(0);
		if(ntn[country].race!=ORC) return(0);
		ntn[country].powers|=BREEDER;
		return(BREEDER);
	}
	return(0);
}

/*form to interactively get a magic power*/
domagic()
{
  	int x, count, done=0, loop=0, i, chance;
	long price;
	short isgod=0;
	if(country==0) {
		isgod=1;
		clear();
		mvaddstr(0,0,"WHAT NATION NUMBER:");
		refresh();
		country = get_number();
	}

	while(done==0){
		done=1;
		clear();
		count=3;
		redraw=TRUE;
		standout();
		mvprintw(0,(COLS/2)-15,"MAGIC POWERS FOR %s",ntn[country].name);
		standend();
		/*print the powers that you have*/
		if(magic(country,WARRIOR)==1)
			mvaddstr(count++,0,"you have WARRIOR power (Fighter Level 1)");
		if(magic(country,CAPTAIN)==1)
			mvaddstr(count++,0,"you have CAPTAIN power (Fighter Level 2)");
		if(magic(country,WARLORD)==1)
			mvaddstr(count++,0,"you have WARLORD power (Fighter Level 3)");
		if(magic(country,MI_MONST)==1)
			mvaddstr(count++,0,"you have MINOR MONSTER power");
		if(magic(country,AV_MONST)==1)
			mvaddstr(count++,0,"you have AVERAGE MONSTER power");
		if(magic(country,MA_MONST)==1) 
			mvaddstr(count++,0,"you have MAJOR MONSTER power");
		if(magic(country,SPY)==1) 
			mvaddstr(count++,0,"you have SPY power");
		if(magic(country,KNOWALL)==1)
			mvaddstr(count++,0,"you have KNOWALL power");
		if(magic(country,DERVISH)==1) 
			mvaddstr(count++,0,"you have DERVISH power");
		if(magic(country,DESTROYER)==1)
			mvaddstr(count++,0,"you have DESTROYER power");
		if(magic(country,HIDDEN)==1) 
			mvaddstr(count++,0,"you have HIDDEN power");
		if(magic(country,THE_VOID)==1)
			mvaddstr(count++,0,"you have THE_VOID power");
		if(magic(country,ARCHITECT)==1)
			mvaddstr(count++,0,"you have ARCHITECT power");
		if(magic(country,VAMPIRE)==1)
			mvaddstr(count++,0,"you have VAMPIRE power");
		if(magic(country,HEALER)==1)
			mvaddstr(count++,0,"you have HEALER power");
		if(magic(country,MINER)==1) 
			mvaddstr(count++,0,"you have MINER power");
		if(magic(country,URBAN)==1)
			mvaddstr(count++,0,"you have URBAN power");
		if(magic(country,STEEL)==1)
			mvaddstr(count++,0,"you have STEEL power");
		if(magic(country,ARCHER)==1)
			mvaddstr(count++,0,"you have ARCHER power");
		if(magic(country,CAVALRY)==1)
			mvaddstr(count++,0,"you have CAVALRY power");
		if(magic(country,BREEDER)==1)
			mvaddstr(count++,0,"you have BREEDER power");

		standout();
			
		if (count-3 >= MAXPOWER) {
			mvaddstr(count++,0,"You have all the available powers");
			refresh();
			getch();
		}
		price = JWL_MGK;
		for (i=1; i<count-3; i++) {
			price <<= 1;
			if (price > 500000000)
				break;
		}

		if((ntn[country].race==ORC)&& ntn[country].jewels>=100000L){
			if(magic(country,MA_MONST)==1) {
			mvaddstr(count++,0,"  You have a 10 percent chance for 100K Jewels to take over other orcs");
			chance=10;
			} else if(magic(country,AV_MONST)==1) {
			mvaddstr(count++,0,"  You have a 6 percent chance for 100K Jewels to take over other orcs");
			chance=6;
			} else if(magic(country,MI_MONST)==1){
			mvaddstr(count++,0,"  You have a 3 percent chance for 100K Jewels to take over other orcs");
			chance=3;
			}
			mvprintw(count++,0,"DO YOU WISH TO TAKE OVER AN ORC NPC NATION (enter y or n):");
			refresh();
			if(getch()=='y'){
				done=0;
				mvaddstr(count++,0,"  What orc nation:");
				refresh();
				i=get_number();
				if(ntn[i].race==ORC){
					ntn[country].jewels-=100000L;
					if(i=takeover(chance,i)==1)
		 			mvprintw(count++,0," Successful: %d",i);
				}
		 		else mvaddstr(count++,0,"  Wrong Race");
			}
		}

		count++;
		if(ntn[country].jewels>=price) {
			mvprintw(count++,0,"YOU CURRENTLY HAVE %ld JEWELS IN YOUR TREASURY",ntn[country].jewels);
			mvprintw(count++,0,"DO YOU WISH TO BUY A RANDOM NEW POWER FOR %ld JEWELS (enter y or n):",price);
			standend();
			refresh();
			if(getch()=='y'){
				done=0;
				loop = 0;
				while(loop++ < 500) if((x=getmagic())!=0){
					ntn[country].jewels -= price;
					CHGMGK;
					exenewmgk(x);
					refresh();
					break;
				}
				if (loop >= 500)
					mvaddstr(count++,0,"You have too many powers!");
			}
		} else {
			mvaddstr(count++,0,"NOT ENOUGH JEWELS TO PURCHASE NEW MAGIC -- hit any key");
			refresh();
			getch();
		}
	}
	if(isgod==1) country=0;
}


/*do magic for both npcs and pcs in update*/
/*if target is 0 then it is update and target will be picked randomly*/
int
takeover(percent,target)
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
			&&(ntn[country].active>=2)) loop=1;
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
		else DESTROY;
		destroy();
		y=country;
		country=save;
		return(y);
	}
	country=save;
	return(0);
}

/*execute new magic*/
exenewmgk(newpower)
{
	short x,y;
	switch(newpower){
		case WARRIOR:
			ntn[country].aplus+=10;
			ntn[country].dplus+=10;
			break;
		case CAPTAIN:
			ntn[country].aplus+=10;
			ntn[country].dplus+=10;
			break;
		case WARLORD:
			ntn[country].aplus+=10;
			ntn[country].dplus+=10;
			break;
		case HEALER:
			if(ntn[country].race==ORC) {
				if(ntn[country].repro<=11)
					ntn[country].repro+=2;
				else	ntn[country].repro=13;
			}
			else if(ntn[country].repro<=8){
				ntn[country].repro=10;
			}
			else if(ntn[country].repro==9){
				ntn[country].repro=10;
				ntn[country].dplus+=5;
			}
			else if(ntn[country].repro>=10){
				ntn[country].dplus+=10;
			}
			break;
		case DESTROYER:
			for(x=ntn[country].capx-3;x<=ntn[country].capx+3;x++) {
				for(y=ntn[country].capy-3;y<=ntn[country].capy+3;y++){
					if((ONMAP)
					&&(sct[x][y].altitude!=WATER)
					&&(isdigit(sct[x][y].vegetation)==0)){
						sct[x][y].vegetation=DESERT;
						sct[x][y].designation=DESERT;
					}
				}
			}
  			updmove(ntn[country].race);
			break;
		case DERVISH:
  			updmove(ntn[country].race);
  			break;
		case MI_MONST:
		case AV_MONST:
		case MA_MONST:
		case SPY:
		case KNOWALL:
		case HIDDEN:
		case THE_VOID:
		case ARCHITECT:
		case MINER:
			break;
		case VAMPIRE:
			ntn[country].aplus-=35;
			ntn[country].dplus-=35;
			ntn[country].maxmove-=2;
			break;
		case URBAN:
			if(ntn[country].race==ORC) {
				ntn[country].repro+=3;
				if(ntn[country].repro>=13){
				ntn[country].maxmove+=2*(ntn[country].repro-13);
				ntn[country].repro=13;
				}
			}
			else if(ntn[country].repro<=9){
				ntn[country].repro+=3;
			}
			else {
				ntn[country].maxmove+=2*(ntn[country].repro-9);
				ntn[country].repro=12;
			}
			break;
		case STEEL:
			break;
		case ARCHER:
			ntn[country].dplus+=10;
			break;
		case CAVALRY:
			ntn[country].aplus+=10;
			ntn[country].maxmove+=6;
			break;
		case BREEDER:
			ntn[country].repro+=3;
			ntn[country].dplus-=10;
			ntn[country].dplus-=10;
			break;
	}
}
