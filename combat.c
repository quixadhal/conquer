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

#define	MGKNUM	32		/* number of units possible in battle */
#define	ATKR	2
#define	DFND	1

extern	FILE	*fnews;
extern	short	country;

short	retreatside;	/* ATKR, DFND, or none (0) */
short	retreatx;	/* retreat x square */
short	retreaty;	/* retreat y square */

int	unit[MGKNUM];		/*armynum*/
int	owner[MGKNUM];		/*owner*/
int	side[MGKNUM];		/*see definitions->1=units 2=unit*/
int	troops[MGKNUM];		/*starting troops in army */
int	anation;		/*nation attacking in this fight*/
int	dnation;		/*one nation defending in this fight*/

/*RESOLVE COMBAT RESULTS */
/*attacker wants a high roll and defender wants low roll on both dice*/
/*       0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9*/

int Cbt6_1[]={
20,20,20,20,20,15,10,10,10,10,5,5,5,5,5,0,0,0,0,0,0
};
int Cbt5_1[]={
40,30,20,20,20,20,20,20,10,10,10,10,5,5,5,0,0,0,0,0,0
};
int Cbt4_1[]={
50,40,30,30,30,20,20,20,20,10,10,10,10,5,5,5,0,0,0,0,0
};
int Cbt3_1[]={
60,50,40,30,30,30,20,20,20,20,10,10,10,10,5,5,5,0,0,0,0
};
int Cbt2_1[]={
70,50,40,40,40,40,30,30,20,20,20,20,10,10,10,10,10,5,5,5,0
};
int Cbt3_2[]={
80,60,40,40,40,40,30,30,30,30,20,20,20,20,20,20,20,10,10,10,0
};
int Cbt5_4[]={
90,60,50,50,50,50,40,40,30,30,30,30,30,30,30,30,30,20,10,10,10
};
int Cbt1_1[]={
100,90,60,60,60,60,50,50,40,40,40,40,30,30,30,30,20,20,20,10,10
};
int Cbt4_5[]={
100,90,80,70,70,70,60,60,50,50,50,50,40,40,40,40,40,30,20,20,10
};
int Cbt2_3[]={
110,90,80,80,70,70,60,60,50,50,50,50,40,40,40,40,40,40,30,20,10
};
int Cbt1_2[]={
110,100,100,90,80,80,80,80,70,60,60,60,50,50,50,50,50,40,30,30,10
};
int Cbt1_3[]={
120,110,110,100,90,90,90,90,80,80,70,70,70,70,60,50,50,40,30,30,20
};
int Cbt1_4[]={
120,110,100,100,90,90,90,90,80,80,70,70,70,70,60,50,50,40,30,30,20
};
int Cbt1_5[]={
130,110,110,100,100,90,90,90,80,80,70,70,70,70,60,50,50,40,40,30,20
};
int Cbt1_6[]={
130,120,110,110,110,100,100,90,90,90,80,80,80,70,70,50,50,40,40,40,40
};


/*run all combat on map
 *  procedure is to find each sector with armies in attack mode and
 *  then search around them
 */
void
combat()
{
	register int i,j;
	/*if fought is 1 then do not fight a second battle in sector*/
	short fought[MAPX][MAPY];
	int temp;
	short armynum,nvynum;
	int valid;
	int count=0;

	printf("RUN COMBAT SUBROUTINES");
	fprintf(fnews,"4\tBATTLE SUMMARY STATISTICS\n");
	/*for each nation, if in attack mode run a check*/

	for(i=0;i<MAPX;i++) for(j=0;j<MAPY;j++) fought[i][j]=0;

	for(anation=0;anation<NTOTAL;anation++) if(ntn[anation].active>0) {

		/*army combat*/
		for(j=0;j<MAXARM;j++) if((ntn[anation].arm[j].stat==ATTACK)
			&&(ntn[anation].arm[j].sold>0)
			&&(fought[ntn[anation].arm[j].xloc][ntn[anation].arm[j].yloc]==0)){

			fought[ntn[anation].arm[j].xloc][ntn[anation].arm[j].yloc]=1;
			/*initialize matrix*/
			for(temp=0;temp<MGKNUM;temp++){
				unit[temp]=(-1);
				owner[temp]=(-1);
				side[temp]=(-1);
				troops[temp]=0;
			}

			/*check all armies in area and add to matrix*/
			count=0;
			valid=FALSE;
			/*is valid,set matrix*/
			for(country=0;country<NTOTAL;country++)
			if(ntn[country].active!=0)
			for(armynum=0;armynum<MAXARM;armynum++)
			if((ASOLD>0)
			&&(ASTAT!=SCOUT)
			&&(AXLOC==ntn[anation].arm[j].xloc)
			&&(AYLOC==ntn[anation].arm[j].yloc)
			&&(count<MGKNUM)) {

				if((country!=anation)
				&&(ntn[anation].dstatus[country]>HOSTILE)) {
					valid=TRUE;
					dnation=country;
				}
				unit[count]=armynum;
				owner[count]=country;
				count++;
			}

			if(valid==TRUE) fight();
		}

		/*navy combat*/
		for(j=0;j<MAXNAVY;j++)
		if((ntn[anation].nvy[j].warships>0)
&&(fought[ntn[anation].nvy[j].xloc][ntn[anation].nvy[j].yloc]==0)
&&(sct[ntn[anation].nvy[j].xloc][ntn[anation].nvy[j].yloc].altitude==WATER)){

		 fought[ntn[anation].nvy[j].xloc][ntn[anation].nvy[j].yloc]=1;

			/*initialize matrix*/
			for(temp=0;temp<MGKNUM;temp++){
				unit[temp]=(-1);
				owner[temp]=(-1);
				side[temp]=(-1);
				troops[temp]=0;
			}

			/*check all fleets in 1 sector range and add to matrix*/
			count=0;
			valid=FALSE;
			/*is valid,set matrix*/
			for(country=0;country<NTOTAL;country++)
			if(ntn[country].active!=0)
			for(nvynum=0;nvynum<MAXNAVY;nvynum++)
			if((NWAR+NMER>0)
			&&(abs(NXLOC-ntn[anation].nvy[j].xloc)<=1)
			&&(abs(NYLOC-ntn[anation].nvy[j].yloc)<=1)
			&&(count<MGKNUM)) {
				fought[NXLOC][NYLOC]=1;
				if((country!=anation)
				&&(ntn[anation].dstatus[country]>HOSTILE)){
					valid=TRUE;
					dnation=country;
				}
				unit[count]=nvynum;
				owner[count]=country;
				count++;
			}
			if(valid==TRUE) navalcbt();
		}
	}
	printf("\nall army and navy attacks completed");
}

/*taking the three matrices, run a combat*/
void
fight()
{
	FILE *fpmsg, *fopen();
	char tempstr[15];
	int droll,aroll;
	int odds;		/*odds total times 100*/
	int done;
	int i,j,k;
	int asold=0,dsold=0;    /*a's and d's total soldiers*/
	int Aloss=0,Dloss=0;    /*a's and d's total losses*/
	int PAloss,PDloss;	/*percent a and d loss*/
	int loss,divisor;
	int abonus=0,dbonus=0;		/*bonus aggregate*/
	short vampire=0;		/*# non vamps deaded */
	short nvamps=0;			/*number of vampire armies*/

	/* determine who is attacker & who is on defenders side?*/
	for(j=0;j<MGKNUM;j++) if(owner[j]!=(-1)){
		if(owner[j]==anation) side[j]=ATKR;
		else if(owner[j]==dnation) side[j]=DFND;
		else if(ntn[anation].dstatus[owner[j]]==JIHAD) side[j]=DFND;
		else if(ntn[owner[j]].dstatus[anation]==JIHAD) side[j]=DFND;
		else if(ntn[anation].dstatus[owner[j]]==WAR)   side[j]=DFND;
		else if(ntn[owner[j]].dstatus[anation]==WAR)   side[j]=DFND;
		else if((ntn[owner[j]].dstatus[anation]==CONFEDERACY)&&(ntn[owner[j]].dstatus[dnation]>HOSTILE)) side[j]=ATKR;
		else if((ntn[owner[j]].dstatus[anation]==ALLIED)&&(ntn[owner[j]].dstatus[dnation]>HOSTILE)) side[j]=ATKR;
	}

	/*calculate number of troops */
	asold=0;
	dsold=0;
	for(i=0;i<MGKNUM;i++) if(owner[i]!=(-1)) {
		if(side[i]==ATKR){
			troops[i] = ntn[owner[i]].arm[unit[i]].sold;
			asold += ntn[owner[i]].arm[unit[i]].sold;
		} else if(side[i]==DFND){
			troops[i] = ntn[owner[i]].arm[unit[i]].sold;
			dsold += ntn[owner[i]].arm[unit[i]].sold;
		}
		if(magic(owner[i],VAMPIRE)==1) nvamps++;
	}

	if((dsold<=0)||(asold<=0)) {
		printf("\nERROR CONDITION -- ABORTING THIS COMBAT!!!!!!\n");
		return;
	}
	odds = (asold*100)/dsold;

	/* CALCULATE WEIGHTED AVERAGE BONUS*/
	abonus=0;
	dbonus=0;
	for(i=0;i<MGKNUM;i++) if(owner[i]!=(-1)) {
		if(side[i]==ATKR)
			abonus += cbonus(i)*troops[i];
		else if(side[i]==DFND)
			dbonus += cbonus(i)*troops[i];
	}

	/*archer bonus if not in fort vs kinghts/cavalry*/
	j=0;
	k=0;
	for(i=0;i<MGKNUM;i++) if(owner[i]!=(-1))

	if(sct[ntn[owner[i]].arm[unit[i]].xloc][ntn[owner[i]].arm[unit[i]].yloc].fortress==0){
		if((ntn[owner[i]].arm[unit[i]].unittyp == A_CAVALRY)
		||(ntn[owner[i]].arm[unit[i]].unittyp == A_KNIGHT))
		if(side[i]==ATKR) j+=troops[i];
		else if(side[i]==DFND) k+=troops[i];
	}

	for(i=0;i<MGKNUM;i++) if(owner[i]!=(-1)) {
	if(j>0) abonus += (15 * j * troops[i]) / asold;
	if(k>0) dbonus += (15 * k * troops[i]) / dsold;
	}

	abonus/=asold;
	dbonus/=dsold;

/*CALCULATED BONUSES TO WHOLE COMBAT*/
	for(i=0;i<MGKNUM;i++) if(owner[i]!=(-1)) {
		if(sct[ntn[owner[i]].arm[unit[i]].xloc][ntn[owner[i]].arm[unit[i]].yloc].fortress != 0){
		/*Catapults add +1%/20 men defending castle (max +10%)*/
		if((ntn[owner[i]].arm[unit[i]].unittyp == A_CATAPULT)
			&&(side[i]==DFND))
			abonus += max((troops[i]/20),10);
		/*Seige_engines add +1%/20 men when attacking fortress*/
		else if((ntn[owner[i]].arm[unit[i]].unittyp == A_SEIGE)
			&&(side[i]==ATKR))
			abonus += max((troops[i]/20),30);
		}
		else {
		/*Catapults add +1%/40 men normal combat (max +10%)*/
		if(ntn[owner[i]].arm[unit[i]].unittyp == A_CATAPULT)
			abonus+=max((troops[i]/40),10);
		}
	}

	/*RUN COMBAT */
	/*DICE RESULTS MAY BE FROM 0 TO 200*/
	/*attacker wants a high roll and defender wants low roll on both dice*/
	aroll =   abs(rand()%100);
	aroll +=  50 + abonus - dbonus;
	if(aroll<0) aroll=0;
	if(aroll>199) aroll=199;

	droll =   abs(rand()%100);
	droll +=  50 + abonus - dbonus;
	if(droll<0) droll=0;
	if(droll>199) droll=199;

	/*odds bonus*/
	if(odds>1500) {
		PAloss = (Cbt6_1[aroll/10])/4;
		PDloss = Cbt1_6[20-droll/10]+30;
	} else if(odds>600) {
		PAloss = Cbt6_1[aroll/10];
		PDloss = Cbt1_6[20-droll/10];
	} else if(odds>500){
		PAloss = Cbt5_1[aroll/10];
		PDloss = Cbt1_5[20-droll/10];
	} else if(odds>400){
		PAloss = Cbt4_1[aroll/10];
		PDloss = Cbt1_4[20-droll/10];
	} else if(odds>300) {
		PAloss = Cbt3_1[aroll/10];
		PDloss = Cbt1_3[20-droll/10];
	} else if(odds>200) {
		PAloss = Cbt2_1[aroll/10];
		PDloss = Cbt1_2[20-droll/10];
	} else if(odds>150) {
		PAloss = Cbt3_2[aroll/10];
		PDloss = Cbt2_3[20-droll/10];
	} else if(odds>125) {
		PAloss = Cbt5_4[aroll/10];
		PDloss = Cbt4_5[20-droll/10];
	} else if(odds>100){
		PAloss = Cbt1_1[aroll/10];
		PDloss = Cbt1_1[20-droll/10];
	} else if(odds>75) {
		PAloss = Cbt4_5[aroll/10];
		PDloss = Cbt5_4[20-droll/10];
	} else if(odds>50) {
		PAloss = Cbt1_2[aroll/10];
		PDloss = Cbt2_1[20-droll/10];
	} else if(odds>33) {
		PAloss = Cbt1_3[aroll/10];
		PDloss = Cbt3_1[20-droll/10];
	} else if(odds>15) {
		PAloss = Cbt1_6[aroll/10];
		PDloss = Cbt6_1[20-droll/10];
	} else {
		PAloss = 120;
		PDloss = 0;
	}

	retreatside = 0;

	if((PDloss > 2* PAloss)
	&&(PDloss>=50)
	&&(odds>150)
	&&(rand()%4==0))	retreatside=DFND;

	if((PAloss > 2* PDloss)
	&&(PAloss>=50)
	&&(odds<150)
	&&(rand()%2==0))	retreatside=ATKR;

	if(retreatside!=0) {
		fdxyretreat();
		/* no legal retreat route */
		if((retreatx== ntn[owner[0]].arm[unit[0]].xloc)
		&& (retreaty== ntn[owner[0]].arm[unit[0]].yloc)){
			if(retreatside==ATKR) PAloss+=10;
			else PDloss+=10;
#ifdef DEBUG
	printf("side %d (%d %d) cant retreat...+10% loss\n",retreatside,retreatx,retreaty);
#endif DEBUG
		}
#ifdef DEBUG
	else
	printf("retreat side %d to %d %d\n",retreatside,retreatx,retreaty);
#endif DEBUG
	}

	/*divisor is the smallest combat bonus*/
	divisor = 100;
	for(i=0;i<MGKNUM;i++) if(owner[i]!=(-1)){
		if(side[i]==ATKR)
		divisor= min (divisor,ntn[owner[i]].aplus);
		else
		divisor= min (divisor,ntn[owner[i]].dplus);
	}
	divisor+=100;

	/*NOTE LOSSES ARE ADJUSTED BY CBONUS*/
	for(i=0;i<MGKNUM;i++) if(owner[i]!=(-1)){
		if(side[i]==ATKR){
			if( ntn[owner[i]].arm[unit[i]].unittyp >= MINMONSTER) {
				if((rand()%100) < PAloss){ /* kill it */
					Aloss += troops[i];
					ntn[owner[i]].arm[unit[i]].sold=0;
				}
			} else {
			loss=(troops[i]*PAloss*divisor) /(100*(100+ntn[owner[i]].aplus));
			if(loss>troops[i] )
				loss=troops[i];
			/*army can't have less than 25 men in it*/
			if(troops[i]-loss<25)
				loss=troops[i];
			Aloss+=loss;
			ntn[owner[i]].arm[unit[i]].sold-=loss;
			}
		} else if(side[i]==DFND){
			if( ntn[owner[i]].arm[unit[i]].unittyp >= MINMONSTER) {
				if((rand()%100) < PDloss){ /* kill it */
					Dloss +=troops[i];
					ntn[owner[i]].arm[unit[i]].sold=0;
				}
			} else {
			loss=(troops[i]*PDloss*divisor)/(100*(100+ntn[owner[i]].dplus));
			if(loss>troops[i] )
				loss=troops[i];
			/*destroy army if < 25 men*/
			if(troops[i]-loss<25)
				loss=troops[i];
			Dloss+=loss;
			ntn[owner[i]].arm[unit[i]].sold-=loss;
			}
		}
		if((nvamps>0)&&(magic(owner[i],VAMPIRE)==1))
			vampire+= loss / 3;
	}
#ifdef HIDELOC
	fprintf(fnews,"4.\tBattle occurs in %s", ntn[sct[ntn[owner[0]].arm[unit[0]].xloc][ntn[owner[0]].arm[unit[0]].yloc].owner].name);
#else
	fprintf(fnews,"4.\tBattle in %d,%d",ntn[owner[0]].arm[unit[0]].xloc, ntn[owner[0]].arm[unit[0]].yloc);
#endif
	for(j=0;j<MGKNUM;j++) if(owner[j]!=(-1)){
		done=FALSE;
		for(i=0;i<j;i++) if(owner[j]==owner[i]) done=TRUE;
		if(done==FALSE) {
		if(side[i]==ATKR)
		fprintf(fnews,", attacker %s",ntn[owner[j]].name);
		else if(side[i]==DFND)
		fprintf(fnews,", defender %s",ntn[owner[j]].name);
		}
	}
	fprintf(fnews,"\n");
	if(nvamps>0){
		for(i=0;i<MGKNUM;i++) if(owner[i]!=(-1)){
			if(magic(owner[i],VAMPIRE)==1)
			ntn[owner[i]].arm[unit[i]].sold+=vampire/nvamps;
		}
	}

	/*who is in the battle*/
	for(j=0;j<MGKNUM;j++) if(owner[j]!=(-1)){
		done=FALSE;

		/*first time your nation appears done=FALSE*/
		for(i=0;i<j;i++) if(owner[j]==owner[i]) done=TRUE;

		if((done==FALSE)&&(ntn[owner[j]].active==1)) {
			sprintf(tempstr,"%s%d",msgfile,owner[j]);
			if ((fpmsg=fopen(tempstr,"a+"))==NULL) {
				printf("\n\tERROR OPENING %s",tempstr);
				exit(FAIL);
			}

			fprintf(fpmsg,"%s\tBATTLE SUMMARY for sector %d, %d\n",ntn[owner[j]].name,ntn[owner[0]].arm[unit[0]].xloc, ntn[owner[0]].arm[unit[0]].yloc);

			if(side[j]==ATKR) fprintf(fpmsg,"%s\tYou are on the Attacking Side\n",ntn[owner[j]].name);
			else fprintf(fpmsg,"%s\tYou are on the Defending Side\n",ntn[owner[j]].name);

			/*detail all participants in battle*/
			for(k=0;k<MGKNUM;k++) if(owner[k]!=(-1)){
				if(side[k]==DFND)
				fprintf(fpmsg,"%s\t%s is defending with army %d (type %s, men %d, bonus=%d)\n",
				ntn[owner[j]].name,
				ntn[owner[k]].name,
				unit[k],
				unittype[ntn[owner[k]].arm[unit[k]].unittyp%100],
				troops[k],
				cbonus(k));
				else if(side[k]==ATKR)
				fprintf(fpmsg,"%s\t%s is attacking with army %d (type %s, men %d, bonus=%d)\n",
				ntn[owner[j]].name,
				ntn[owner[k]].name,
				unit[k],
				unittype[ntn[owner[k]].arm[unit[k]].unittyp%100],
				troops[k],
				cbonus(k));
				else
				fprintf(fpmsg,"%s\t%s is neutral with army %d (type %s, men %d, bonus=%d)\n",
				ntn[owner[j]].name,
				ntn[owner[k]].name,
				unit[k],
				unittype[ntn[owner[k]].arm[unit[k]].unittyp%100],
				ntn[owner[k]].arm[unit[k]].sold,
				cbonus(k));
			}

			fprintf(fpmsg,"%s\tattacking soldiers=%d\tmodified roll=%d-> attackers percent loss %d%%\n",ntn[owner[j]].name,asold,aroll,PAloss);
			fprintf(fpmsg,"%s\tdefending soldiers=%d\tmodified roll=%d-> defenders percent loss %d%%\n",ntn[owner[j]].name,dsold,droll,PDloss);
			fprintf(fpmsg,"%s\tODDS are %d to 100\n",ntn[owner[j]].name,odds);
			fprintf(fpmsg,"%s\tRESULT: Attackers lose %d men, Defenders lose %d men\n",ntn[owner[j]].name,Aloss, Dloss);
			if(retreatside==ATKR){
			fprintf(fpmsg,"%s\t\tAdditionally, All attackers retreat to %d %d\n",ntn[owner[j]].name,retreatx,retreaty);
#ifdef DEBUG
			printf("%s\t\tAdditionally, All attackers retreat to %d %d\n",ntn[owner[j]].name,retreatx,retreaty);
#endif DEBUG
			} else if(retreatside==DFND){
			fprintf(fpmsg,"%s\t\tAdditionally, All defenders retreat to %d %d\n",ntn[owner[j]].name,retreatx,retreaty);
#ifdef DEBUG
			printf("%s\t\tAdditionally, All defenders retreat to %d %d\n",ntn[owner[j]].name,retreatx,retreaty);
#endif DEBUG
			}
			fprintf(fpmsg,"%s\n","END");
			fclose(fpmsg);
		}
	}
	retreat();
}

/*Subroutine to determine combat bonuses for unit i in matrix*/
/*	returns int - bonus */
int
cbonus(num)
{
	short armynum;
	int armbonus;

	armbonus=0;
	armynum=unit[num];
	country=owner[num];

	/*Racial combat bonus due to terrain (the faster you move the better)*/
	armbonus+=5*(9-movecost[AXLOC][AYLOC]);

	if((magic(country,MI_MONST)==1)&&(unit[num]==0)) armbonus+=20;
	if((magic(country,AV_MONST)==1)&&(unit[num]==0)) armbonus+=20;
	if((magic(country,MA_MONST)==1)&&(unit[num]==0)) armbonus+=20;

	if(((magic(country,DESTROYER)==1)
	||(magic(country,DERVISH)==1))
	&&((sct[AXLOC][AYLOC].vegetation==ICE)
	||(sct[AXLOC][AYLOC].vegetation==DESERT)))
		armbonus+=30;

	if(side[num]==DFND){

		if(sct[AXLOC][AYLOC].altitude==MOUNTAIN) armbonus+=20;
		else if(sct[AXLOC][AYLOC].altitude==HILL) armbonus+=10;

		if(sct[AXLOC][AYLOC].vegetation==JUNGLE) armbonus+=20;
		else if(sct[AXLOC][AYLOC].vegetation==FOREST) armbonus+=15;
		else if(sct[AXLOC][AYLOC].vegetation==WOOD) armbonus+=10;

		armbonus += ntn[owner[num]].dplus;

		if(sct[AXLOC][AYLOC].designation==DCASTLE)
			armbonus+=5*sct[AXLOC][AYLOC].fortress;
		else if((ASTAT==GARRISON)
		&&(sct[AXLOC][AYLOC].designation==DCITY)){
			if(magic(country,ARCHITECT)==1){
				armbonus+=10+16*sct[AXLOC][AYLOC].fortress;
			}
			else armbonus+=10+8*sct[AXLOC][AYLOC].fortress;
		}
		else if((ASTAT==GARRISON)
		&&(sct[AXLOC][AYLOC].designation==DCAPITOL)){
			if(magic(country,ARCHITECT)==1){
				armbonus+=20+20*sct[AXLOC][AYLOC].fortress;
			}
			else armbonus+=20+10*sct[AXLOC][AYLOC].fortress;
		}

	}
	else if(side[num]==ATKR) armbonus += ntn[owner[num]].aplus;

	/*army status is important*/
	if(ASTAT==MARCH) armbonus-=40;

	/*if a fortress*/
	if(sct[AXLOC][AYLOC].fortress!=0){
		/*Cavalry and Knights get -20%*/
		if((ATYPE == A_CAVALRY) ||(ATYPE == A_KNIGHT)) armbonus -= 20;
		/*Archers gain pluses*/
		else if((ATYPE == A_ARCHER)&&(side[num]==DFND)) armbonus += 15;
		else if((ATYPE == A_ARCHER)&&(side[num]==ATKR)) armbonus += 5;
	}

	/*Phalanx and Legionaires need certain numbers of troops*/
	if(((ATYPE!=A_PHALANX) || (ASOLD>500))
	&&((ATYPE!=A_LEGION) || (ASOLD>1000))){
		if(side[num]==ATKR) armbonus+= *(unitattack+(ATYPE%100));
		else armbonus+= *(unitdefend+(ATYPE%100));
	}

	return(armbonus);
}

fdxyretreat()	/* finds retreat location */
{
	int x,y,nation=(-1);
	int xsctr= ntn[owner[0]].arm[unit[0]].xloc;
	int ysctr= ntn[owner[0]].arm[unit[0]].yloc;

	if((sct[xsctr][ysctr].designation==DCITY)
	||(sct[xsctr][ysctr].designation==DCAPITOL)){
		retreatside=0;
		return;
	}
	retreatx=xsctr;
	retreaty=ysctr;

	if(retreatside == ATKR) nation=anation;
	else nation=dnation;

	for(x= xsctr-1; x<=xsctr+1; x++)
	for(y= ysctr-1; y<=ysctr+1; y++) if(ONMAP){
		if(tofood(sct[x][y].vegetation,0)==0) continue;
		if(((sct[x][y].owner == nation)
		   ||(ntn[sct[x][y].owner].dstatus[nation] < NEUTRAL))
		||(solds_in_sector( x, y, sct[x][y].owner) == 0)){
			retreatx=x;
			retreaty=y;
#ifdef DEBUG
			printf("%d %d RETREAT TO %d %d\n",xsctr,ysctr,x,y);
#endif DEBUG
			return;
		}
	}
}

retreat()
{
	int cnum;

	if(retreatside == 0) return;

	for(cnum=0;cnum<MGKNUM;cnum++) if(owner[cnum]!=(-1)){
		if(owner[cnum] > MAXNTN ) continue;	/*monsts dont retreat */
		if((side[cnum]==ATKR)&&(retreatside==ATKR)){
			ntn[owner[cnum]].arm[unit[cnum]].xloc = retreatx;
			ntn[owner[cnum]].arm[unit[cnum]].yloc = retreaty;
		}
		else if((side[cnum]==DFND)&&(retreatside==DFND)){
			ntn[owner[cnum]].arm[unit[cnum]].xloc = retreatx;
			ntn[owner[cnum]].arm[unit[cnum]].yloc = retreaty;
		}
	}
}

/*SUBROUTINE TO RUN NAVAL COMBAT ON ALL SHIPS */
/*just like fight, this takes array of owner,side,unit and calculate*/
void
navalcbt()
{
	FILE *fpmsg, *fopen();
	int sailor;
	char tempmsg[15];
	int aship=0,dship=0;    /*a's and d's total war ships*/
	int asunk=0,dsunk=0;    /*a's and d's losses for the round*/
	int amsunk=0,dmsunk=0;  /*a's and d's msunks for the round*/
	register int done,i,j,k;

	/* determine who is attacker & who is on defenders side?*/
	for(j=0;j<MGKNUM;j++) if(owner[j]!=(-1)){
		if(owner[j]==anation) side[j]=ATKR;
		else if(ntn[anation].dstatus[owner[j]]==JIHAD) side[j]=DFND;
		else if(ntn[owner[j]].dstatus[anation]==JIHAD) side[j]=DFND;
		else if(ntn[anation].dstatus[owner[j]]==WAR)   side[j]=DFND;
		else if(ntn[owner[j]].dstatus[anation]==WAR)   side[j]=DFND;
		else if((ntn[owner[j]].dstatus[anation]==CONFEDERACY)
		&&(ntn[owner[j]].dstatus[dnation]>HOSTILE)) side[j]=ATKR;
		else if((ntn[owner[j]].dstatus[anation]==ALLIED)
		&&(ntn[owner[j]].dstatus[dnation]>HOSTILE)) side[j]=ATKR;
	}

	/*RUN COMBAT; loop until done*/
	/*determine relative strengths--does anybody try to flee*/
	/*calculate number of ships on a side*/
	for(j=0;j<MGKNUM;j++) if(owner[j]!=(-1)){
		if(side[j]==DFND) {
			dship+=ntn[owner[j]].nvy[unit[j]].warships;
		}
		else if(side[j]==ATKR) {
			aship+=ntn[owner[j]].nvy[unit[j]].warships;
		}
	}

	/*no bonus currently included in this combat*/

	/*each warship can do damage 40%; once all warships sunk then all*/
	/*sunk are captured merchant*/
	sailor=FALSE;
	for(j=0;j<MGKNUM;j++) if(owner[j]!=(-1)) if(side[j]==ATKR)
		if(magic(owner[j],SAILOR)==TRUE) sailor=TRUE;
	if(sailor==TRUE) for(i=0;i<aship;i++) if(rand()%10<=5) dsunk++;
	else for(i=0;i<aship;i++) if(rand()%10<=3) dsunk++;

	sailor=FALSE;
	for(j=0;j<MGKNUM;j++) if(owner[j]!=(-1)) if(side[j]==DFND)
		if(magic(owner[j],SAILOR)==TRUE) sailor=TRUE;
	if(sailor==TRUE) for(i=0;i<dship;i++) if(rand()%10<=4) asunk++;
	else for(i=0;i<dship;i++) if(rand()%10<=3) asunk++;

#ifdef HIDELOC
	fprintf(fnews,"4.\tNaval Battle occurs");
#else
	fprintf(fnews,"4.\t%d,%d: Naval Battle",ntn[owner[0]].nvy[unit[0]].xloc, ntn[owner[0]].nvy[unit[0]].yloc);
#endif
	for(j=0;j<MGKNUM;j++) if(owner[j]!=(-1)){
		k=0;
		for(i=0;i<j;i++) if(owner[j]==owner[i]) k=1;
		if(k==0) {
		if(side[i]==DFND)
			fprintf(fnews,", attacker %s",ntn[owner[j]].name);
		else if(side[i]==ATKR)
			fprintf(fnews,", defender %s",ntn[owner[j]].name);
		}
	}
	fprintf(fnews,"\n");


#ifdef HIDELOC
	fprintf(fnews,"4.\t\tNaval attackers lose %d of %d warships\n",
		min(asunk,aship),aship);
	fprintf(fnews,"4.\t\tNaval defenders lose %d of %d warships\n",
		min(dsunk,dship),dship);
	fprintf(fnews,"4.\t\tNaval attackers lose %d merchants\n",
		max(0,asunk-aship));
	fprintf(fnews,"4.\t\tNaval defenders lose %d merchants\n",
		max(0,dsunk-dship));
#else
	fprintf(fnews,"4.\t%d,%d: Naval attackers lose %d of %d warships\n",
	ntn[owner[0]].nvy[unit[0]].xloc,
	ntn[owner[0]].nvy[unit[0]].yloc,min(asunk,aship),aship);
	fprintf(fnews,"4.\t%d,%d: Naval defenders lose %d of %d warships\n",
	ntn[owner[0]].nvy[unit[0]].xloc,
	ntn[owner[0]].nvy[unit[0]].yloc,min(dsunk,dship),dship);
	fprintf(fnews,"4.\t%d,%d: Naval attackers lose %d merchants\n",
	ntn[owner[0]].nvy[unit[0]].xloc,
	ntn[owner[0]].nvy[unit[0]].yloc,max(0,asunk-aship));
	fprintf(fnews,"4.\t%d,%d: Naval defenders lose %d merchants\n",
	ntn[owner[0]].nvy[unit[0]].xloc,
	ntn[owner[0]].nvy[unit[0]].yloc,max(0,dsunk-dship));
#endif

	for(i=0;i<MGKNUM;i++) if(owner[i]!=(-1)){
		if((asunk>0)&&(side[i]==ATKR)){
			if(asunk > aship ) {
				ntn[owner[i]].nvy[unit[i]].warships=0;
				amsunk = asunk - aship;
			} else  {
				ntn[owner[i]].nvy[unit[i]].warships -= asunk * ntn[owner[i]].nvy[unit[i]].warships/aship;
			}
		}
		else if((dsunk>0)&&(side[i]==DFND)){
			if(dsunk > dship ) {
				ntn[owner[i]].nvy[unit[i]].warships=0;
				dmsunk = dsunk -dship;
			} else  {
				ntn[owner[i]].nvy[unit[i]].warships -= dsunk * ntn[owner[i]].nvy[unit[i]].warships / dship;
			}
		}
	}
	/* take care of merchants*/
	for(i=0;i<MGKNUM;i++) if(owner[i]!=(-1)){
		if((amsunk>0)&&(side[i]==ATKR)){
			if(amsunk >= ntn[owner[i]].nvy[unit[i]].merchant ) {
				amsunk -= ntn[owner[i]].nvy[unit[i]].merchant;
				ntn[owner[i]].nvy[unit[i]].merchant=0;
			} else  {
				ntn[owner[i]].nvy[unit[i]].merchant-=amsunk;
				amsunk=0;
			}
		}
		else if((dmsunk>0)&&(side[i]==DFND)){
			if(dmsunk >= ntn[owner[i]].nvy[unit[i]].merchant ) {
				dmsunk -= ntn[owner[i]].nvy[unit[i]].merchant;
				ntn[owner[i]].nvy[unit[i]].merchant=0;
			} else  {
				ntn[owner[i]].nvy[unit[i]].merchant-=dmsunk;
				dmsunk=0;
			}
		}
	}

	/*will round continue; does one side wish to withdraw*/

	/*mail results; who is in the battle*/
	for(j=0;j<MGKNUM;j++) if(owner[j]!=(-1)){
		done=FALSE;

		/*first time your nation appears done=FALSE*/
		for(i=0;i<j;i++) if(owner[j]==owner[i]) done=TRUE;

		if((done==FALSE)&&(ntn[owner[j]].active==1)) {
			sprintf(tempmsg,"%s%d",msgfile,owner[j]);
			if ((fpmsg=fopen(tempmsg,"a+"))==NULL) {
				fprintf(fnews,"4.\tERROR OPENING %s\n",tempmsg);
				exit(FAIL);
			}

			fprintf(fpmsg,"%s NAVAL BATTLE in sector %d %d\n"
			,ntn[owner[j]].name,ntn[owner[0]].nvy[unit[0]].xloc
			,ntn[owner[0]].nvy[unit[0]].yloc);

			if(side[j]==ATKR)
			fprintf(fpmsg,"%s You are on the Attacking Side\n"
				,ntn[owner[j]].name);
			else fprintf(fpmsg,"%s You are on the Defending Side\n"
				,ntn[owner[j]].name);

			/*detail all participants in battle*/
			for(k=0;k<MGKNUM;k++) if(owner[k]!=(-1)){
				if(side[k]==DFND)
				fprintf(fpmsg,"%s\t%s is defender with navy %d\n" ,ntn[owner[j]].name , ntn[owner[k]].name,unit[k]);
				else fprintf(fpmsg,"%s\t%s is attacker with navy %d\n",ntn[owner[j]].name,ntn[owner[k]].name, unit[k]);
			}

			fprintf(fpmsg,"%s RESULT: Attackers lose %d ships, Defenders lose %d ships\n",ntn[owner[j]].name, asunk,dsunk);
			fprintf(fpmsg,"%s         Attackers capture %d merchants, Defenders capture %d merchants\n",ntn[owner[j]].name, dmsunk,amsunk);
			fprintf(fpmsg,"END\n");
			fclose(fpmsg);
		}
	}
}
