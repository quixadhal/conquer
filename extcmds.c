/*
* additions made in tut.fi (winter 1987 and spring 1988)
* for more information ask from mta@tut.fi or esc@tut.fi
*/

/* THESE COMMANDS APPEAR USEFUL and (of course) I have heavily
	modified them.  I basically decided to use these subroutines in
	my code too. - The Ed */

/* ext_cmd()			run extended commands	*/
/* clear_bottom(i)		clear bottom min(i,4) lines of the map */
/* errormsg(str)		print error message to bottom line */
/* splitarmy(army)		split ? men from army */
/* reducearmy(army,men)		subtract men from army */
/* change_status(army,stat)	change army stat to integer status */
/* combinearmies(army1,army2)	add army2 to army1 */

#include <stdio.h>
#include "header.h"
#include "data.h"

extern FILE *fexe;
extern short selector;
extern short country;
extern short xcurs, ycurs, xoffset, yoffset;

ext_cmd()
{
	int stat=0,armynum,army2,men;

	clear_bottom(0);

	mvprintw(LINES-3, 0,
	" (+) combine,(-) split n men,(/) divide by 2,([1-5]) chg status");
	mvprintw(LINES-4, 0, "Extended command:");
	refresh();

	switch(getch()) {
	case '-':		/* split army */
		armynum = getselunit();
		splitarmy(armynum);
		break;
	case '/':		/* divide army */
		armynum = getselunit();
		men = ASOLD/2;
		reducearmy(armynum,men);
		break;
	case '+':		/* combine armies */
		armynum = getselunit();
		selector +=2;
		army2 = getselunit();
		selector -=2;
		combinearmies(armynum,army2);
		break;
	case '5':		/* Army status */
		stat++;
	case '4':
		stat++;
	case '3':
		stat++;
	case '2':
		stat++;
	case '1':
		stat++;
		armynum = getselunit();
		change_status(armynum,stat);
		break;
	default:		/* unimplemented? */
		errormsg("Unimplemented extended command");
	}
	makebottom();
	refresh();
}

combinearmies(armynum,army2)
int armynum, army2;
{
	if (armynum < 0 || armynum >= MAXARM ||
		army2 < 0 || army2 >= MAXARM ||
		armynum == army2 ||
		ASTAT == SCOUT ||
		ntn[country].arm[army2].stat == SCOUT ||
		ATYPE >= MINMONSTER ||
		ATYPE!=ntn[country].arm[army2].unittyp) {
			errormsg("Selected armies not legal");
			return;
	}
	if((ntn[country].arm[army2].xloc!=AXLOC)
	&&(ntn[country].arm[army2].yloc!=AYLOC)) {
		errormsg("Selected armies not together");
		return;
	}
	ASOLD += ntn[country].arm[army2].sold;
	ntn[country].arm[army2].sold = 0;
	AMOVE=min(AMOVE,ntn[country].arm[army2].smove);
	AADJMEN;
	AADJMOV;
	armynum = army2;
	AADJMEN;
}

change_status(armynum,new_stat)
int armynum,new_stat;
{
	if (armynum < 0 || armynum >= MAXARM || ASTAT==SCOUT) {
		errormsg("Selected army not legal");
		return;
	}
	if (new_stat == SCOUT && ASOLD > 25) {
		errormsg("Need less than 26 men to scout");
		return;
	}
	ASTAT = new_stat;
	AADJSTAT;
}

reducearmy(armynum,men)
int armynum,men;
{
	int army2;
	int oldx, oldy, army;

	if(ATYPE>=MINMONSTER){
		errormsg("sorry -- army is monster");
		return;
	}

	if(men<0 || armynum < 0 || armynum >= MAXARM || ASOLD < men+25 ) {
		errormsg("Selected army too small or illegal");
		return;
	}

	if(ASOLD < men) {
		errormsg("Too many men to split from army");
		return;
	}

	oldx=AXLOC;
	oldy=AYLOC;
	army2=armynum;

	armynum=(-1);
	for(army=0;army<MAXARM;army++)
		if(ntn[country].arm[army].sold==0){
			if(armynum==(-1)) armynum=army;
		}
	/*overflow*/
	if(armynum==(-1)) {
		errormsg("TOO MANY ARMIES");
		return;
	} else {
		AMOVE=ntn[country].arm[army2].smove;
		ATYPE=ntn[country].arm[army2].unittyp;
		ASTAT=ntn[country].arm[army2].stat;
		AXLOC=oldx;
		AYLOC=oldy;
		ASOLD=men;
		ntn[country].arm[army2].sold-=ASOLD;
		AADJSTAT;
		AADJMEN;
		AADJLOC;
		AADJMOV;
		armynum=army2;
		AADJMEN;
	}
}

splitarmy(armynum)
int armynum;
{
	int men;
	clear_bottom(2);
	mvaddstr(LINES-2, 0, "How many men to split? ");
	refresh();
	men = get_number();
	reducearmy(armynum,men);
}

errormsg(str)
char *str;
{
	mvprintw(LINES-1, 0, str);
	mvprintw(LINES-1, 60, "PRESS ANY KEY");
	beep();
	refresh();
	getch();
}

clear_bottom(i)
int i;
{
	if(i==0) i=4;
	for (; i>0 ; i--) {
		move( LINES-i, 0);
		clrtoeol();
	}
}
