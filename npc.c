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

extern FILE *fnews;

extern short country;
extern int attr[MAPX][MAPY];     /*sector attactiveness*/
extern short occ[MAPX][MAPY];
extern short movecost[MAPX][MAPY];
int peace;   	/*is 8 if at peace, 12 if at war*/

#ifdef NPC
nationrun()
{
	int loop,armynum;
	int x,y,i,z;

	prep();

	/* is there an error*/
	if((sct[ntn[country].capx][ntn[country].capy].owner==country)&&(sct[ntn[country].capx][ntn[country].capy].designation!=DCAPITOL)){
		sct[ntn[country].capx][ntn[country].capy].designation=DCAPITOL;
	}

	/*go to war*/
	for(x=1;x<MAXNTN;x++) {
		/*if they at war with you go to war with them*/
		if(ntn[x].dstatus[country]>=WAR) {
			ntn[country].dstatus[x]=WAR;
		}
		/*else adjust diplomacy*/
		else if((ntn[country].dstatus[x]!=UNMET)&&(ntn[country].dstatus[x]!=JIHAD)&&(ntn[country].dstatus[x]!=CONFEDERACY)){
			if((ntn[x].tmil>4*ntn[country].tmil)&&(ntn[x].score>4*ntn[country].score)){
				if(rand()%3==0)  ntn[country].dstatus[x]=WAR;
				else if(rand()%8==0)  
					ntn[country].dstatus[x]=JIHAD;
				else ntn[country].dstatus[x]++;
			}
			/*if 2* mil and 2* score then not like them*/
			else if((ntn[x].tmil>2*ntn[country].tmil)&&(ntn[x].score>2*ntn[country].score)){
				if(ntn[country].dstatus[x]!=WAR)
					if(ntn[x].race==ntn[country].race){
						if(rand()%10==0) ntn[country].dstatus[x]++;
					}
					else if(rand()%3==0) ntn[country].dstatus[x]++;
			}
			/*adjust based on your status with them*/
			if(ntn[country].dstatus[x]!=WAR){
				if(ntn[x].dstatus[country]>ntn[country].dstatus[x]){
					if(rand()%3==0) 
						ntn[country].dstatus[x]++;
				}
				else 
					if(ntn[x].dstatus[country]<ntn[country].dstatus[x]){
						if(rand()%3==0) 
						ntn[country].dstatus[x]--;
					}
			}
			if(rand()%4==0) ntn[country].dstatus[x]++;
			else if(rand()%3==0) ntn[country].dstatus[x]--;
		}
		else if((ntn[country].dstatus[x]==CONFEDERACY)&&(ntn[x].dstatus[country]==CONFEDERACY)&&(ntn[country].race==ntn[x].race)){
			if(ntn[x].active>=2) takeover(1,x);
		}
	}

	/*move units */
	/*are they at war with any normal countries*/
	peace=0;
	for(i=1;i<MAXNTN;i++) if(ntn[country].dstatus[i]>peace)
		peace=ntn[country].dstatus[i];

	if(peace<WAR){
		peace=8;
		pceattr();
		for(armynum=1;armynum<MAXARM;armynum++)
			if((ASOLD!=0)&&(ASTAT!=GARRISON)) armymove(armynum);
	}
	/*if war then attack &/or expand */
	else {
		peace=12;
		/*are they attacking or defending */
		for(x=0;x<MAXNTN;x++) if(ntn[country].dstatus[x]>HOSTILE){
			if(100*(ntn[country].tmil*(ntn[country].aplus+100))/((ntn[country].tmil*(ntn[country].aplus+100))+(ntn[x].tmil*(ntn[x].dplus+100)))>rand()%100){
				/*attacker*/
				for(armynum=1;armynum<MAXARM;armynum++)
					if((ASOLD>0)&&(ASTAT!=GARRISON)) ASTAT=ATTACK;
				atkattr();
			}
			/*defender*/
			else {
				for(armynum=1;armynum<MAXARM;armynum++)
					if((ASOLD>0)&&(ASTAT!=GARRISON)){
						if(ASOLD<350) ASTAT=DEFEND;
						else ASTAT=ATTACK;
					}
				defattr();
			}
		}

		for(armynum=1;armynum<MAXARM;armynum++)
			if((ASOLD!=0)&&(ASTAT!=GARRISON)) armymove(armynum);
	}

	/*redesignate sectors*/
	if(ntn[country].active!=1) for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++)
		if((sct[x][y].owner==country)&&(todigit(sct[x][y].vegitation)!=0)) npcredes(x,y);

	/*build forts in any cities*/
	if(ntn[country].active!=1) for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++)
		if(((sct[x][y].designation==DCITY)||(sct[x][y].designation==DCAPITOL))&&(sct[x][y].fortress<(sct[x][y].people%1000))&&(sct[x][y].fortress<5))
			sct[x][y].fortress++;

	/*redo mil*/
	/*rate position -- any cities with no garison get one*/
	if(ntn[country].active!=1) redomil();

	/*buy new powers and/or new weapons*/
	if(ntn[country].jewels > 2* JWL_MGK) {
		loop=0;
		for(armynum==0;armynum<=MAXPOWER;armynum++){
			if(magic(country,armynum)==1) i++;
		}
		/*maximum of nine powers for NPC nations*/
		if(i>NPCPOWERS) loop=1;
		while(loop==0){
			if((z=getmagic())!=1){
				fprintf(fnews,"2.\tnation %s gets magic power number %d\n",ntn[country].name,z);
				exenewmgk(z);
				ntn[country].jewels-=JWL_MGK;
				loop=1;
			}
		}

	}
	if(ntn[country].tiron >  4 * IRONORE * ntn[country].tmil){
		ntn[country].aplus+=1;
		ntn[country].dplus+=1;
		ntn[country].tgold-=2*IRONORE * ntn[country].tmil;
	}
}

/*calculate attractiveness when at peace*/
pceattr()
{
	int x,y,temp;
	/*add around capital*/
	for(x=ntn[country].capx-2;x<=ntn[country].capx+2;x++)
		for(y=ntn[country].capy-2;y<=ntn[country].capy+2;y++)
			if(sct[x][y].owner==0) attr[x][y]+=80;
	/*add to attractiveness for unowned sectors*/
	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) if(sct[x][y].owner==0) {
		/*temp stands for the distance from the capital*/
		temp=0;
		if(x>ntn[country].capx) temp+=x-ntn[country].capx;
		else temp+=ntn[country].capx-x;
		if(y>ntn[country].capy) temp+=y-ntn[country].capy;
		else temp+=ntn[country].capy-y;
		if(temp<3) attr[x][y]+=300-(50*temp);
		else if(temp<20) attr[x][y]+=100;
	}
	else if((sct[x][y].owner==NNOMAD)||(sct[x][y].owner==NBARBARIAN)||(sct[x][y].owner==NPIRATE)) attr[x][y]+=100;
	if(sct[ntn[country].capy][ntn[country].capy].owner!=country){
		attr[ntn[country].capy][ntn[country].capy]=1000;
	}
}

/*calculate attractiveness of attacking sectors*/
atkattr()
{
	int nation,armie,x,y,x1,x2,Y1,y2;

	for(x=ntn[country].capx-2;x<=ntn[country].capx+2;x++)
		for(y=ntn[country].capy-2;y<=ntn[country].capy+2;y++)
			if(sct[x][y].owner==0) attr[x][y]+=80;

	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) if((sct[x][y].owner==0)||(sct[x][y].owner==NNOMAD)||(sct[x][y].owner==NBARBARIAN)||(sct[x][y].owner==NPIRATE)) attr[x][y]+=100;

	/*adjust for each nation that you are at war with*/
	for(nation=1;nation<MAXNTN;nation++) if((ntn[country].dstatus[nation]>=WAR)&&(ntn[country].active!=0)){

		/*plus 1/2 men if in sector with their army*/
		/*defend your capital if occupied, +50 more if with their army*/
		for(armie=1;armie<MAXARM;armie++){
			if(ntn[nation].arm[armie].sold > 0) {

				attr[ntn[nation].arm[armie].xloc][ntn[nation].arm[armie].yloc]+=ntn[nation].arm[armie].sold%10;

				if((ntn[nation].arm[armie].xloc<=ntn[country].capx+2)&&(ntn[nation].arm[armie].yloc<=ntn[country].capy+2)&&(ntn[nation].arm[armie].xloc>=ntn[country].capx-2)&&(ntn[nation].arm[armie].yloc>=ntn[country].capy-2)){
				attr[ntn[nation].arm[armie].xloc][ntn[nation].arm[armie].yloc]+=ntn[nation].arm[armie].sold%5;
				if((rand()%3==0)&&(ntn[country].dstatus[nation]<WAR))
					ntn[country].dstatus[nation]++;
				if((ntn[nation].arm[armie].xloc==ntn[country].capx)&&(ntn[nation].arm[armie].yloc==ntn[country].capy)){
					if(ntn[country].dstatus[nation]==WAR)
					ntn[country].dstatus[nation]=JIHAD;
					if(ntn[nation].arm[armie].sold>2*ntn[country].arm[0].sold){
						attr[ntn[nation].arm[armie].xloc][ntn[nation].arm[armie].yloc]+=ntn[nation].arm[armie].sold%2;
					}
					else attr[ntn[nation].arm[armie].xloc][ntn[nation].arm[armie].yloc]+=ntn[nation].arm[armie].sold%5;
				}
				}
			}
		}

		/*plus 40 if next to their capital */
		for(x=ntn[nation].capx-1;x<=ntn[nation].capy+1;x++){
			for(y=ntn[nation].capy-1;y<=ntn[nation].capy+1;y++){
				if((x>0)&&(x<MAPX)&&(y>0)&&(y<MAPY)) attr[x][y]+=40;
			}
		}

		/*plus 100 if on their capital*/
		attr[ntn[nation].capx][ntn[nation].capy]+=100;

		/*+60 if between the two capitals*/
		if (ntn[nation].capx < ntn[country].capx){
			x1=ntn[nation].capx;
			x2=ntn[country].capx;
		}
		else {
			x1=ntn[country].capx;
			x2=ntn[nation].capx;
		}
		if (ntn[nation].capy < ntn[country].capy){
			Y1=ntn[nation].capy;
			y2=ntn[country].capy;
		}
		else {
			Y1=ntn[country].capy;
			y2=ntn[nation].capy;
		}
		for(x=x1;x<=x2;x++) for(y=Y1;y<=y2;y++) {
			if((x>0)&&(x<MAPX)&&(y>0)&&(y<MAPY)) attr[x][y]+=60;
		}

		/*for each sector +60 if their sector*/
		for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++)
			if((sct[x][y].owner==nation)&&(occ[x][y]==0))
				attr[x][y]+=100;
			else if(sct[x][y].owner==nation) 
				attr[x][y]+=60;
	}
	if(sct[ntn[country].capy][ntn[country].capy].owner!=country){
		attr[ntn[country].capy][ntn[country].capy]=1000;
	}
}

defattr()
{
	int nation,armie,x,y,x1,x2,y1,y2;

	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) if((sct[x][y].owner==0)||(sct[x][y].owner==NNOMAD)||(sct[x][y].owner==NBARBARIAN)||(sct[x][y].owner==NPIRATE)) attr[x][y]+=100;

	for(x=ntn[country].capx-2;x<=ntn[country].capx+2;x++)
		for(y=ntn[country].capy-2;y<=ntn[country].capy+2;y++)
			if(sct[x][y].owner==0) attr[x][y]+=80;

	for(nation=1;nation<MAXNTN;nation++) if((ntn[nation].active!=0)&&(ntn[country].dstatus[nation]>=WAR)) {

		/*plus if near enemy army*/
		/*plus 30 if next to their army*/
		/*plus 60 if with their army*/
		for(armie=1;armie<MAXARM;armie++) if(ntn[nation].arm[armie].sold > 0) {

			attr[ntn[nation].arm[armie].xloc][ntn[nation].arm[armie].yloc]+=ntn[nation].arm[armie].sold%10;

			if((ntn[nation].arm[armie].xloc<=ntn[country].capx+2)&&(ntn[nation].arm[armie].yloc<=ntn[country].capy+2)&&(ntn[nation].arm[armie].xloc>=ntn[country].capx-2)&&(ntn[nation].arm[armie].yloc>=ntn[country].capy-2)){
				attr[ntn[nation].arm[armie].xloc][ntn[nation].arm[armie].yloc]+=ntn[nation].arm[armie].sold%5;
				if((rand()%3==0)&&(ntn[country].dstatus[nation]<WAR))
					ntn[country].dstatus[nation]++;
				if((ntn[nation].arm[armie].xloc==ntn[country].capx)&&(ntn[nation].arm[armie].yloc==ntn[country].capy)){
					if(ntn[country].dstatus[nation]==WAR)
					ntn[country].dstatus[nation]=JIHAD;
					if(ntn[nation].arm[armie].sold>2*ntn[country].arm[0].sold){
						attr[ntn[nation].arm[armie].xloc][ntn[nation].arm[armie].yloc]+=ntn[nation].arm[armie].sold%2;
					}
					else attr[ntn[nation].arm[armie].xloc][ntn[nation].arm[armie].yloc]+=ntn[nation].arm[armie].sold%5;
				}
			}
		}


		/*plus if strategic blocking sector*/
		/*+60 if between the two capitals*/
		if (ntn[nation].capx < ntn[country].capx){
			x1=ntn[nation].capx;
			x2=ntn[country].capx;
		}
		else {
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
			if((x>0)&&(x<MAPX)&&(y>0)&&(y<MAPY)) attr[x][y]+=60;
		}

		/*plus 80 if near your capital */
		for(x=ntn[country].capx-1;x<=ntn[country].capy+1;x++){
			for(y=ntn[country].capy-1;y<=ntn[country].capy+1;y++){
				if((x>0)&&(x<MAPX)&&(y>0)&&(y<MAPY)) attr[x][y]+=80;
			}
		}

		/*plus based on defensive value*/
		for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) {
			if(movecost[x][y]==1) attr[x][y] += 50;
			else if(movecost[x][y]<=3) attr[x][y] += 20;
			else if(movecost[x][y]<=5) attr[x][y] += 10;

			if((sct[x][y].designation==DCITY)&&((sct[x][y].owner==country)||(sct[x][y].owner==nation))){
				attr[x][y] += 50;
			}

			/* plus 60 if they own and unoccupied*/
			if((sct[x][y].owner==nation)&&(occ[x][y]==0)) attr[x][y]+=100;
			else if(sct[x][y].owner==nation) attr[x][y]+=60;
		}
	}

	if(sct[ntn[country].capy][ntn[country].capy].owner!=country){
		attr[ntn[country].capy][ntn[country].capy]=1000;
	}
}
#endif

/*ntn 1 is nation you are updating*/
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

/*destroy nation--special case if capital not owned by other nation*/
destroy()
{
	short armynum, nvynum;
	int i, x, y;
	fprintf(fnews,"1.\tDESTROY NATION %s",ntn[country].name);
	if(country!=sct[ntn[country].capx][ntn[country].capy].owner){
		fprintf(fnews,"\t(their capitol is owned by %s)\n",ntn[sct[ntn[country].capx][ntn[country].capy].owner].name);
		/*get +5% to combat skill*/
		ntn[sct[ntn[country].capx][ntn[country].capy].owner].aplus+=5;
	}
	else fprintf(fnews,"\t(they own their capitol)\n");

	ntn[country].active=0;
	for(armynum=0;armynum<MAXARM;armynum++) if(ASOLD>0) {
		if(ntn[sct[AXLOC][AYLOC].owner].race==ntn[country].race)
			sct[AXLOC][AYLOC].people+=ASOLD;
		ASOLD=0;
	}
	for(nvynum=0;nvynum<MAXNAVY;nvynum++) {
		NMER=0;
		NWAR=0;
	}
	for(i=0;i<MAXNTN;i++) {
		ntn[i].dstatus[country]=UNMET;
		ntn[country].dstatus[i]=UNMET;
	}

	/*if take them you get their gold*/
	if(country!=sct[ntn[country].capx][ntn[country].capy].owner){
		if(ntn[country].tgold>0) ntn[sct[ntn[country].capx][ntn[country].capy].owner].tgold+=ntn[country].tgold;
		if(ntn[country].jewels>0) ntn[sct[ntn[country].capx][ntn[country].capy].owner].jewels+=ntn[country].jewels;
		if(ntn[country].tiron>0) ntn[sct[ntn[country].capx][ntn[country].capy].owner].tiron+=ntn[country].tiron;
		if(ntn[country].tfood>0) ntn[sct[ntn[country].capx][ntn[country].capy].owner].tfood+=ntn[country].tfood;
	}

	/*if god destroys then kill all population*/
	if(country==sct[ntn[country].capx][ntn[country].capy].owner){
		for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) if(sct[x][y].owner==country) {
			sct[x][y].people=0;
			sct[x][y].owner=0;
			if(isdigit(sct[x][y].vegitation)!=0)
				sct[x][y].designation=DNODESIG;
		}
	}
	/*slowly take over and all people flee*/
	else if(ntn[sct[ntn[country].capx][ntn[country].capy].owner].race!=ntn[country].race){
		for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) if(sct[x][y].owner==country) {
			/*all kinds of refugees to neighboring countries*/
			flee(x,y,1);
			sct[x][y].people=0;
			sct[x][y].owner=0;
			if(isdigit(sct[x][y].vegitation)!=0)
				sct[x][y].designation=DNODESIG;
		}
	}
	/*else same race, so give all land to conqueror*/
	else {
		for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) if(sct[x][y].owner==country){
			sct[x][y].owner=sct[ntn[country].capx][ntn[country].capy].owner;
			if(isdigit(sct[x][y].vegitation)!=0)
				sct[x][y].designation=DNODESIG;
		}
	}
	return;
}

monster()
{
	short nvynum,armynum;
	int x, y, done, j;

	/*move nomads randomly until within 2 of city then attack*/
	country=NNOMAD;
	for(armynum=0;armynum<MAXARM;armynum++) if(ASOLD>0){
		x=AXLOC+rand()%4-2;
		y=AYLOC+rand()%4-2;
		if((x>0)&&(x<MAPX)&&(y>0)&&(y<MAPY)&&(sct[x][y].altitude!=PEAK)&&(sct[x][y].altitude!=WATER)) {
			AXLOC=x;
			AYLOC=y;
			done=0;
			/*if owned & unoccupied you take & people flee*/
			if(sct[x][y].owner!=0) for(j=0;j<MAXARM;j++) if((ntn[sct[x][y].owner].arm[j].xloc==x)&&(ntn[sct[x][y].owner].arm[j].yloc==y)&&(ntn[sct[x][y].owner].arm[j].sold>0)) done=1;

			/*nomads take sector if done=0*/
			if(done==0){
				if(sct[x][y].owner==0)
					fprintf(fnews,"3.\tnomads take sector %d,%d\n",x,y);
				else fprintf(fnews,"3.\tnomads capture sector %d,%d\n",x,y);
				if(sct[x][y].owner!=0) flee(x,y,1);
				sct[x][y].owner=NNOMAD;
				sct[x][y].designation=DDEVASTATED;
			}
		}
	}
	else {
		/*place a new Nomad army*/
		x=(rand()%MAPX-8)+4;
		y=(rand()%MAPY-8)+4;
		if((rand()%4==0)&&(sct[x][y].altitude!=PEAK)&&(sct[x][y].altitude!=WATER)) {
			AXLOC=x;
			AYLOC=y;
			ASOLD=100+100*rand()%10;
			ASTAT=ATTACK;
		}
	}
	fprintf(fnews,"2.\t%s are updated\n",ntn[NPIRATE].name);
	/*if pirate fleet within 3 attack if outnumber any fleets */
	for(nvynum=0;nvynum<MAXNAVY;nvynum++){
	for(x=1;x<MAXNTN;x++) for(y=0;y<MAXNAVY;y++)
	if((ntn[x].nvy[y].xloc-NXLOC<2)&&(ntn[x].nvy[y].xloc-NXLOC>(-2))&&
	(ntn[x].nvy[y].yloc-NYLOC<2)&&(ntn[x].nvy[y].yloc-NYLOC>(-2))&&
	(sct[ntn[x].nvy[y].xloc][ntn[x].nvy[y].yloc].altitude==WATER)){
		NXLOC= ntn[x].nvy[y].xloc;
		NYLOC= ntn[x].nvy[y].yloc;
	}
	/*add one warship to random pirate fleet*/
	if((NWAR>0)&&(rand()%3==0)) NWAR++;
	}
}

npcredes(x,y)
{
	int food;
	/*large enough for a city now?*/
	if(((sct[x][y].people*(CITYLIMIT+(ntn[sct[x][y].owner].tsctrs/3))>ntn[sct[x][y].owner].tciv)||((ntn[sct[x][y].owner].tciv<30000)&&(sct[x][y].people>1000)))&&(ntn[sct[x][y].owner].tfood>ntn[sct[x][y].owner].tciv*2)){
		if((rand()%2==0)&&((sct[x][y].designation==DFARM)||(sct[x][y].designation==DGOLDMINE)||(sct[x][y].designation==DMINE)))
			sct[x][y].designation=DCITY;
	}
	/*not large enough for city and not enough food*/
	else if((sct[x][y].designation==DCITY)&&(ntn[sct[x][y].owner].tfood<ntn[sct[x][y].owner].tciv*2)&&(todigit(sct[x][y].vegitation)>5)){
		sct[x][y].designation=DFARM;
		food=todigit(sct[x][y].vegitation)*sct[x][y].people;
		ntn[sct[x][y].owner].tfood += food;
		ntn[sct[x][y].owner].tgold += food*TAXFOOD/100-(sct[x][y].people * TAXCITY/100);
	}
	/*not large enough for a city but enough food*/
	else if((sct[x][y].designation==DCITY)&&(rand()%5==0)){
		sct[x][y].designation=DFARM;
		food=todigit(sct[x][y].vegitation)*sct[x][y].people;
		ntn[sct[x][y].owner].tfood += food;
		ntn[sct[x][y].owner].tgold += food*TAXFOOD/100-(sct[x][y].people * TAXCITY/100);
	}

	/*what if it is not a city*/
	if((sct[x][y].designation!=DCITY)&&(sct[x][y].designation!=DCAPITOL)) {
		sct[x][y].designation=DFARM;
		/*crisis situation -- need more food producers*/
		if(ntn[sct[x][y].owner].tfood<=2*ntn[sct[x][y].owner].tciv){
			if(sct[x][y].iron>5)
				sct[x][y].designation=DMINE;
			if((sct[x][y].gold>5)&&(sct[x][y].gold>sct[x][y].iron-2))
				sct[x][y].designation=DGOLDMINE;
		}
		/*non crisis situation -- need more food producers*/
		else {
			if(sct[x][y].iron>2)
				sct[x][y].designation=DMINE;
			if((sct[x][y].gold>2)&&(sct[x][y].gold>sct[x][y].iron-2))
				sct[x][y].designation=DGOLDMINE;
		}
	}
}

redomil()
{
	short x,y,armynum;
	int diff, i, free, done;

	/*make sure enough men in army 0 -- garrison duty in capital*/
	armynum=0;
	ASTAT=GARRISON;
	AXLOC=ntn[country].capx;
	AYLOC=ntn[country].capy;

	/*Ideally ASOLD(0)*MILINCAP=tmil*peace/10*/
	/*MILRATIO ratio mil:civ for non player countries*/
	/*MILINCAP ratio (mil in cap):mil for NPCs*/
	if(ASOLD*MILINCAP<.9*ntn[country].tmil*peace/10){
		/*too few soldiers on garrison*/
		/*diff is number to change mil in cap (>0)*/
		diff=(ntn[country].tmil*peace/(10*MILINCAP))-ASOLD;

		if(diff<ntn[country].tiron*10) diff=ntn[country].tiron*10;
		if(ntn[country].tgold<0L) diff=0;

		if(diff>sct[ntn[country].capx][ntn[country].capy].people)
			diff=sct[ntn[country].capx][ntn[country].capy].people/2;

		sct[ntn[country].capx][ntn[country].capy].people-=diff;
		ASOLD+=diff;
		ntn[country].tciv-=diff;
		ntn[country].tmil+=diff;
		ntn[country].tgold-=diff*ENLISTCOST;
		ntn[country].tiron-=diff*10;
	}
	/*else split garrison army if 1.25* needed number*/
	else if(ASOLD*MILINCAP>1.1*ntn[country].tmil*peace/10){
		diff=ASOLD-(1.25*ntn[country].tmil*peace/(10*MILINCAP));
		free=0;
		ASOLD-=diff;
		ntn[country].tciv-=diff;
		ntn[country].tmil+=diff;
		sct[ntn[country].capx][ntn[country].capy].people+=diff;
	}

	/*build ships and/or armies*/
	done=0;
	/*if tmil*MILRATIO<tciv build a single new army in the capital if possible*/
	if((ntn[country].tmil*MILRATIO<.8*ntn[country].tciv*peace/10)&&(ntn[country].tgold>0L)) for(armynum=1;armynum<MAXARM;armynum++) if((done==0)&&(ASOLD==0)) {
		done=1;
		ASOLD=(ntn[country].tciv*peace/(10*MILRATIO))-ntn[country].tmil;
		if(ASOLD>ntn[country].tiron*10) ASOLD= ntn[country].tiron/10;
		if(2*ASOLD>sct[ntn[country].capx][ntn[country].capy].people)
			ASOLD=sct[ntn[country].capx][ntn[country].capy].people/2;
		ntn[country].tiron-=ASOLD*10;
		AXLOC= ntn[country].capx;
		AYLOC= ntn[country].capy;
		ntn[country].tmil += ASOLD;
		ntn[country].tciv -= ASOLD;
		ntn[country].tgold-=ASOLD*ENLISTCOST;
		sct[AXLOC][AYLOC].people-=ASOLD;
		ASTAT= DEFEND;
		AMOVE=0;
	}

	/*disband ships and/or armies*/
	if(ntn[country].tmil*MILRATIO>1.2*ntn[country].tciv*peace/10){
		/*disband a pseudo-random army*/
		done=0;
		diff=ntn[country].tmil-(1.2*ntn[country].tciv*peace/(10*MILRATIO));
		for(armynum=1;armynum<MAXARM;armynum++)
			if((done==0)&&(sct[AXLOC][AYLOC].owner==country)&&((sct[AXLOC][AYLOC].gold>5)||(sct[AXLOC][AYLOC].iron>5))&&(rand()%5==0)&&(ASOLD<diff)){
				sct[AXLOC][AYLOC].people+=ASOLD;
				ntn[country].tmil -= ASOLD;
				ntn[country].tciv += ASOLD;
				ASOLD=0;
				if(ntn[country].tmil*MILRATIO>1.2*ntn[country].tciv*peace/10) done=1;
			}
	}

	/*resize armies */
	/*maximum npc army is 200 or tmil/20, minimum is 50*/
	for(armynum=1;armynum<MAXARM;armynum++){
		if((ASOLD>200)&&(ASOLD>ntn[country].tmil/20)) {
			free=0;
			for(i=1;i<MAXARM;i++){
				if((free==0)&&(ntn[country].arm[i].sold==0)){
					free=1;
					ASOLD/=2;
					ntn[country].arm[i].sold  = ASOLD;
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
				if((ntn[country].arm[i].sold>0)&&(ntn[country].arm[i].xloc==AXLOC)&&(ntn[country].arm[i].yloc==AYLOC)&&(i!=armynum)&&(free==0)){
					free=1;
					ntn[country].arm[i].sold += ASOLD;
					ASOLD=0;
				}
			}
		}
	}

	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) 
	if((sct[x][y].owner==country)&&(sct[x][y].designation==DCITY)){
		free=0;
		for(armynum=0;armynum<MAXARM;armynum++){
			if((AXLOC==x)&&(AYLOC==y)) free=1;
		}
		/*move army in*/
		if(free==0){
			for(armynum=1;armynum<MAXARM;armynum++)
				if((abs(AXLOC-x)<=1)&&(abs(AYLOC-y)<=1))
					free=armynum;
			if(free>0){
				armynum=free;
				AXLOC=x;
				AYLOC=y;
			}
		}
	}
}
