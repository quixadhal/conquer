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
#include <ctype.h>
#include "newlogin.h"
#include "header.h"
#include "data.h"

extern FILE *fexe, *fopen();
extern short country;

void
newlogin()
{
	/* use points to create empire, add if late starter*/
	int points;
	char tempc[10];
	int n;
	int valid=TRUE;  /*valid==FALSE means continue loop*/
	int temp;
	int more=TRUE;	/*0 if add another player*/
	int loop;
	long x;
	char tempo[8];
	char passwd[8];
	register i;

	printf("\nPreparing to add player\n");
	printf("break at any time to abort\n");

	while(more==TRUE) {
		points=MAXPTS;
		/*find valid nation number type*/
		for(i=1;i<MAXNTN;i++)
			if(ntn[i].active==0) {
				country=i;
				break;
			}
		printf("first valid nation id is %d\n",country);

		if(i==MAXNTN) {
			beep();
			printf("error, cant add new nation\n");
			return;
		}

		/* open output for future printing*/
		sprintf(tempc,"%s%d",exefile,i);
		if ((fexe=fopen(tempc,"w"))==NULL) {
			beep();
			printf("error opening %s\n",tempc);
			exit(FAIL);
		}

		ntn[country].active=1;

		valid=FALSE;
		/*get name*/
		while(valid==FALSE) {
			valid=TRUE;
			printf("\nwhat name would you like your nation to have:");
			scanf("%s",ntn[country].name);

			if((strlen(ntn[country].name)<=1)
			 ||(strlen(ntn[country].name)>NAMELTH)){
				printf("\ninvalid name");
				valid=FALSE;
			}

			/*check if already used*/
			if((strcmp(ntn[country].name,"god")==0)
			||(strcmp(ntn[country].name,"unowned")==0)){
				printf("\nname already used");
				valid=FALSE;
			}

			for(i=1;i<MAXNTN;i++)
				if((i!=country)&&(strcmp(ntn[i].name,ntn[country].name)==0)) {
					printf("\nname already used");
					valid=FALSE;
				}
		}

		/*get password*/
		valid=FALSE;
		while(valid==FALSE) {
			printf("\nwhat is your nations password:");
			scanf("%7s",passwd);
			if((strlen(passwd)>PASSLTH)||(strlen(passwd)<2)) {
				beep();
				printf("\ninvalid password (too short or long)");
			}
			else {
				printf("\nreenter your nations password:");
				scanf("%7s",ntn[country].passwd);
			}
			if(strcmp(passwd,ntn[country].passwd)==0) valid=TRUE;
		}
		strncpy(ntn[country].passwd,crypt(passwd,SALT),PASSLTH);

		/*get your name*/
		valid=FALSE;
		while(valid==FALSE) {
			valid=TRUE;
			printf("\nenter either YOUR name (j_smith) or the name of your nations leader (gandalf...)");
			printf("\n\t(maximum 10 characters, no spaces):");
			scanf("%s",tempc);
			if((strlen(tempc)>=10)||(strlen(tempc)<2)) {
				beep();
				printf("\ninvalid name (too short or long)");
				valid=FALSE;
			}
			else strcpy(ntn[country].leader,tempc);
		}

		valid=FALSE;
		while(valid==FALSE) {
			valid=TRUE;
			printf("\nwhat race would you like to be:");
			printf("\n\tchoose (d)warf,(e)lf,(o)rc,(h)uman:");
			scanf("%1s",tempo);
			switch(tempo[0]) {
			case 'd':
				printf("\ndwarf chosen\n");
				/*MINER POWER INATE TO DWARVES*/
				printf("you have magical MINING skills\n");
				ntn[country].powers=MINER;
				x=MINER;
				CHGMGK;
				points -= getclass(DWARF);
				ntn[country].race=DWARF;
				ntn[country].tgold=NLDGOLD;	/*1   pts*/
				ntn[country].tfood=NLDFOOD;     /*0   pts*/
				ntn[country].jewels=NLDJEWEL;     /*0   pts*/
				ntn[country].tiron=NLDIRON;     /*0   pts*/
				ntn[country].tciv= NLDCIVIL;	/*6   pts*/
				ntn[country].tmil= NLDMILIT;	/*1.5 pts*/
				ntn[country].repro= NLDREPRO;	/*4   pts*/
				ntn[country].maxmove= NLDMMOVE;	/*1.5 pts*/
				ntn[country].aplus= NLDAPLUS;	/*2   pts*/
				ntn[country].dplus= NLDDPLUS;	/*2   pts*/
				ntn[country].location=RANDOM;	/*0+  pts*/
				points-=NLPTDW;
				break;
			case 'e':
				printf("\nelf chosen\n");
				printf("you have magical cloaking skills\n");
				ntn[country].powers=THE_VOID;
				x=THE_VOID;
				CHGMGK;
				points -= getclass(ELF);
				ntn[country].race=ELF;
				ntn[country].tgold=NLEGOLD;	/*1  pts*/
				ntn[country].tfood=NLEFOOD;
				ntn[country].jewels=NLEJEWEL;
				ntn[country].tiron=NLEIRON;
				ntn[country].tciv=NLECIVIL;	/*7    pts*/
				ntn[country].tmil=NLEMILIT;	/*0   pts*/
				ntn[country].repro=NLEREPRO;	/*3   pts*/
				ntn[country].maxmove=NLEMMOVE;	/*2   pts*/
				ntn[country].aplus=NLEAPLUS;	/*1   pts*/
				ntn[country].dplus=NLEDPLUS;	/*4   pts*/
				ntn[country].location=FAIR;	/*1   pts*/
				points-=NLPTEL;	/* VOID COSTS ADDITIONAL PT*/
				break;
			case 'o':
				printf("\norc chosen\n");
				/*MINOR MONSTER POWER INATE TO ORCS*/
				printf("your leader is a minor monster (army 0 is very potent)\n");
				ntn[country].powers=MI_MONST;
				x=MI_MONST;
				CHGMGK;
				points -= getclass(ORC);
				ntn[country].race=ORC;
				ntn[country].tgold=NLOGOLD;	/*0   pts*/
				ntn[country].tfood=NLOFOOD;
				ntn[country].jewels=NLOJEWEL;
				ntn[country].tiron=NLOIRON;
				ntn[country].tciv=NLOCIVIL;	/*4   pts*/
				ntn[country].tmil=NLOMILIT;	/*2.5 pts*/
				ntn[country].repro=NLOREPRO;	/*10  pts*/
				ntn[country].maxmove=NLOMMOVE;	/*1.5 pts*/
				ntn[country].aplus=NLOAPLUS;	/*0   pts*/
				ntn[country].dplus=NLODPLUS;	/*0   pts*/
				ntn[country].location=RANDOM;	/*0   pts*/
				points-=NLPTOR;
				break;
			case 'h':
				printf("\nhuman chosen\n");
				ntn[country].race=HUMAN;
				/*WARRIOR POWER INATE TO HUMANS*/
				printf("you have magical WARRIOR skills\n");
				ntn[country].powers=WARRIOR;
				x=WARRIOR;
				CHGMGK;
				points -= getclass(HUMAN);
				ntn[country].tgold=NLHGOLD;	/*0  pts*/
				ntn[country].tfood=NLHFOOD;	/*0   pts*/
				ntn[country].jewels=NLHJEWEL;	/*0   pts*/
				ntn[country].tiron=NLHIRON;	/*0   pts*/
				ntn[country].tciv=NLHCIVIL;	/*6   pts*/
				ntn[country].tmil=NLHMILIT;	/*1   pts*/
				ntn[country].repro=NLHREPRO;	/*5   pts*/
				ntn[country].maxmove=NLHMMOVE;	/*2.5 pts*/
				ntn[country].aplus=NLHAPLUS;	/*1   pts*/
				ntn[country].dplus=NLHDPLUS;	/*1   pts*/
				ntn[country].location=RANDOM;	/*0   pts*/
				points-=NLPTHU;
				break;
			default:
				printf("\ninvalid race\n ");
				valid=0;
			}
		}

		ntn[country].mark = toupper(*ntn[country].name);
		printf("\ntesting first letter of name (%c) for nation mark...",ntn[country].mark);
		valid=TRUE;
		while(valid==TRUE) {
			valid=FALSE;
			if((isprint(ntn[country].mark)==0)
			||(isspace(ntn[country].mark)!=0)) {
				printf("\n%c is not a graphical character",ntn[country].mark);
				valid=TRUE;
			}
			if(valid==FALSE) for(i=0;i<=3;i++)
			if(ntn[country].mark==(*(ele+i))) {
				printf("\n%c is an altitude character",ntn[country].mark);
				valid=TRUE;
			}
			if(valid==FALSE) for(i=0;i<=11;i++)
			if(ntn[country].mark==(*(veg+i))) {
				printf("\n%c is a vegetation character", ntn[country].mark );
				valid=TRUE;
			}
			if(valid==FALSE) for(i=1;i<country;i++)
			if(ntn[i].mark==ntn[country].mark) {
				printf("\n%c is an already used character",ntn[country].mark);
				valid=TRUE;
			}
			if(valid==FALSE) if(ntn[country].mark=='*') {
				printf("\n'*' is invalid character");
				valid=TRUE;
			}
			if(valid==TRUE){
				printf("\nplease reenter new national mark for maps:");
				printf("\n (this can be any uppercase character)");
				scanf("%1s",tempc);
				ntn[country].mark=(*tempc);
				/* if lower case letter, make it upper */
				if( islower(ntn[country].mark)!=0)
					ntn[country].mark=
						toupper(ntn[country].mark);
			}
			else printf("\nvalid...");
		}

		printf("mark currently is %c\n",ntn[country].mark);

		while(points>0) {
			printf("\n\nwhat would you like to buy with your remaining %d points\n\n",points);
			printf("\t1. population (%d/pt):\t\tnow have %ld civilians\n",NLPOP,ntn[country].tciv);
			printf("\t2. more gold ($%d/pt):\t\tnow have %ld gold pieces\n",NLGOLD,ntn[country].tgold);
			printf("\t3. better location (%d pt):\t\tlocation is now is %c\n",NLLOCCOST,ntn[country].location);
			printf("\t4. more soldiers (%d/pt):\t\tnow have %ld soldiers\n",NLSOLD,ntn[country].tmil);
			printf("\t5. better attack (%d%%/pt):\t\tnow is +%d\n ",NLATTACK,ntn[country].aplus);
			printf("\t6. better defence (%d%%/pt):\t\tnow is +%d\n",NLDEFENCE,ntn[country].dplus);
			printf("\t7. higher reproduction (+%d%%/%d pt):\trate is now %d%%\n",NLREPRO,NLREPCOST,ntn[country].repro);
			printf("\t8. higher movement (%d/pt): \t\tnow move %d sectors\n",NLMOVE,ntn[country].maxmove);
			printf("\t9. double raw recourses (%d pts): \tfood now %ld\n",NLDBLCOST*ntn[country].tfood/NLHFOOD,ntn[country].tfood);
			printf("\t                                \tjewels now %ld\n",ntn[country].jewels);
			printf("\t                                \tiron now %ld\n",ntn[country].tiron);
			printf("\t10. additional random magic power (%d pts)\n",NLMAGIC);


			printf("\nWhat option to buy:");
			if(scanf("%d",&n)==1) switch(n) {

			case 1:
				printf("additional population costs 1 pt per %d\n",NLPOP);
				printf("how many points to spend on population:");
				scanf("%d",&temp);
				putchar('\n');
				if(points >= temp) {
					points -= temp;
					ntn[country].tciv+=temp*NLPOP;
				}
				else printf("You dont have enough points left");
				break;
			case 2:
				printf("you now have $%ld\n",ntn[country].tgold);
				printf("and can buy gold at $%d per point\n",NLGOLD);
				printf("how many points to spend on added gold:");
				scanf("%d",&temp);
				putchar('\n');
				if(points>=temp)
				{
					points-=temp;
					ntn[country].tgold+=temp*NLGOLD;
				}
				else printf("You dont have enough points left");
				break;
			case 3:
				printf("you now are in %c location\n",ntn[country].location);
				printf("  R=random, F=fair, G=good\n");
				if(ntn[country].location==RANDOM){
					printf("%dpts for (g)ood location or 1pts for (f)air\n",2*NLLOCCOST);
				}
				if(ntn[country].location==FAIR){
					printf("%dpts for (g)ood location\n",NLLOCCOST);
				}
				if(ntn[country].location==GREAT) return;

				printf("what type of location do you wish:");
				scanf("%1s",tempo);
				switch(tempo[0]) {
				case 'g':
				case GREAT:
					if(ntn[country].location==FAIR){
						printf("%dpts for (g)ood location\n",NLLOCCOST);
						if(points>=NLLOCCOST) {
							points -=NLLOCCOST;
							ntn[country].location=GREAT;
						}
						else printf("You dont have enough points left");
					}
					else {
						if(points>=2*NLLOCCOST) {
							points -= 2*NLLOCCOST;
							ntn[country].location=GREAT;
						}
						else printf("You dont have enough points left");
					}
					break;
				case 'f':
				case FAIR:
					if(ntn[country].location==RANDOM){
						if(points>=NLLOCCOST) {
							points -=NLLOCCOST;
							ntn[country].location=FAIR;
						}
						else printf("You dont have enough points left");
					}
					else printf("\nlocation unchanged\n");
					break;
				default:
					printf("\nlocation unchanged\n");
				}
				putchar('\n');
				break;
			case 4:
				printf("you start with %ld soldiers\n",ntn[country].tmil);
				printf("additional military costs 1 / %d\n",NLSOLD);
				printf("how many points to spend?");
				scanf("%d",&temp);
				putchar('\n');
				if(points >= temp) {
					points -= temp;
					ntn[country].tmil+=temp*NLSOLD;
				}
				else printf("You dont have enough points left");
				break;
			case 5:
				if(magic(country,VAMPIRE)==1) {
				printf("you have vampire power and cant add to combat bonus\n");
				break;
				}
				printf("now have %d percent attack bonus\n",ntn[country].aplus);
				printf("an additional %d percent per point\n",NLATTACK);
				printf("how many points do you wish to spend?");
				scanf("%d",&temp);
				putchar('\n');
				if(points >= temp) {
					points -= temp;
					ntn[country].aplus+=temp*NLATTACK;
				}
				else printf("You dont have enough points left");
				break;
			case 6:
				if(magic(country,VAMPIRE)==1) {
				printf("you have vampire power and cant add to combat bonus\n");
				break;
				}
				printf("now have %d percent defence bonus\n",ntn[country].dplus);
				printf("an additional %d percent per point\n",NLDEFENCE);
				printf("how many points do you wish to spend?");
				scanf("%d",&temp);
				putchar('\n');
				if(points >= temp) {
					points -= temp;
					ntn[country].dplus+=temp*NLDEFENCE;
				}
				else printf("You dont have enough points left");
				break;
			case 7:
				printf("repro rate costs %d points per %d percent\n",NLREPCOST,NLREPRO);
				printf("you now have %d percent\n",ntn[country].repro);
				if((ntn[country].race!=ORC)
				&&(ntn[country].repro>=10)){
					printf("you have the maximum rate");
					break;
				}
				else if(ntn[country].repro>=15){
					printf("you have the maximum rate");
					break;
				}
				printf("how many percentage points to add?:");
				scanf("%d",&temp);
				putchar('\n');
				if(points >= (temp*NLREPCOST)) {
					if((ntn[country].race!=ORC)
					&&(ntn[country].repro+NLREPRO*temp>10)){
					printf("that exceeds the 10%% limit");
					}
					else if((ntn[country].race==ORC)
					&&(ntn[country].repro>15)){
					printf("that exceeds the 15%% limit");
					}
					else {
					points -= temp*NLREPCOST;
					ntn[country].repro+=NLREPRO*temp;
					}
				}
				else printf("You dont have enough points left");
				break;
			case 8:
				printf("additional movement costs 1 per +%d sct/turn\n",NLMOVE);
				printf("you start with a rate of %d\n",ntn[country].maxmove);
				printf("you now have a rate of %d\n",ntn[country].maxmove+NLMOVE);
				putchar('\n');
				if(points >= 1) {
					points -= 1;
					ntn[country].maxmove+=NLMOVE;
				}
				else printf("You dont have enough points left");
				break;
			case 9:
				printf("doubling raw materials\n");
				if((ntn[country].tfood<800000)
				&&(points >=NLDBLCOST*ntn[country].tfood/NLHFOOD)) {
					points-=NLDBLCOST*ntn[country].tfood/NLHFOOD;
					ntn[country].tfood*=2;
					ntn[country].jewels*=2;
					ntn[country].tiron*=2;
				}
				else printf("sorry\n");
				break;
			case 10:
				printf("choosing basic magic at %d point cost\n",NLMAGIC);
				printf("log in and read the magic screen to be informed of your powers\n");
				if(points >0) {
					points-=NLMAGIC;
					loop=0;
					while(loop==0) if((x=getmagic(rand()%M_MGK+M_MIL))!=0){
						CHGMGK;
						loop=1;
					}
				}
				else printf("sorry not enough points\n");
				break;
			default:
				printf("invalid option - hit return");
				scanf("%*s");
			}
		}

		ntn[country].powers=0;;
		printnat();
		printf("\nhit 'y' if OK?");
		getchar();
		if(getchar()!='y'){
			ntn[country].active=0;
			getchar();
			printf("\n OK, nation deleted\n");
			printf("\nhit return to add another nation");
			printf("\nhit any other key to continue?");
			if(getchar()=='\n') more=TRUE;
			else more=FALSE;
			putchar('\n');
		}
		else {
			place();
			getchar();
			printf("\nNation is now added to world");
			printf("\nhit return to add another nation");
			printf("\nhit any other key to continue?");
			if(getchar()=='\n') more=TRUE;
			else more=FALSE;
			putchar('\n');
		}
		fclose(fexe);
	}
	writedata();
}

void
printnat()
{
	int i;
	i=country;
	printf("about to print stats for nation %d\n\n",i);
	printf("name is .........%s\n",ntn[i].name);
	printf("leader is .......%s\n",ntn[i].leader);
	printf("total sctrs .....%d\n",ntn[i].tsctrs);
	printf("class is ........%s\n",*(Class+ntn[i].class));
	printf("mark is .........%c\n",ntn[i].mark);
	printf("race is .........%c\n",ntn[i].race);
	printf("attack plus is ..+%d\n",ntn[i].aplus);
	printf("defence plus is .+%d\n",ntn[i].dplus);
	printf("gold is .........%ld\n",ntn[i].tgold);
	printf("maxmove is ......%d sctrs\n",ntn[i].maxmove);
	printf("jewels is .......%ld\n",ntn[i].jewels);
	printf("# military ......%ld\n",ntn[i].tmil);
	printf("# civilians .....%ld\n",ntn[i].tciv);
	printf("repro is ........%d percent\n",ntn[i].repro);
	printf("total iron ......%ld\n",ntn[i].tiron);
	printf("total food ......%ld\n",ntn[i].tfood);
	printf("total ships .....%d\n",ntn[i].tships);
}

/*PLACE NATION*/
void
place()
{
	int placed=0;
	int t;
	int temp;
	int n; /*count vbl for inf loop*/
	short armynum=0;
	register i,j,x,y;

	n=0;
	switch(ntn[country].location) {
	case OOPS:
		while ((placed == 0)&&(n++<2000)){
			if(ntn[country].active==1){
				x = (rand()%(MAPX-8))+4;
				y = (rand()%(MAPY-8))+4;
			}
			else {
				x = (rand()%(MAPX-2))+1;
				y = (rand()%(MAPY-2))+1;
			}
			if(is_habitable(x,y)) placed=1;

			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[i][j].owner!=0) placed=0;
			temp=0;
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[i][j].altitude==WATER) temp++;
			if(temp>=7) placed=0;
			if(tofood(sct[x][y].vegetation,country)<DESFOOD) placed=0;
		}
		break;
	case RANDOM:
		while ((placed == 0)&&(n++<2000)){
			if(ntn[country].active==1){
#if (MAPX>12)
				x = rand()%(MAPX-12)+6;
				y = rand()%(MAPY-12)+6;
#else
				x = rand()%(MAPX-8)+4;
				y = rand()%(MAPY-8)+4;
#endif
				if(is_habitable(x,y)) placed=1;
				/*important that no countries near*/
				for(i=x-4;i<=x+4;i++) for(j=y-4;j<=y+4;j++)
					if((sct[i][j].owner<MAXNTN)
					&&(sct[i][j].owner!=0)) placed=0;
			}
			else {
				x = (rand()%(MAPX-6))+3;
				y = (rand()%(MAPY-6))+3;
				if(is_habitable(x,y)) placed=1;
				/*important that no countries near*/
				for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++)
					if((sct[i][j].owner<MAXNTN)
					&&(sct[i][j].owner!=0)) placed=0;
			}
			temp=0;
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[i][j].altitude==WATER) temp++;
			if(temp>=7) placed=0;
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[i][j].owner!=0) placed=0;
			if(tofood(sct[x][y].vegetation,country)<DESFOOD) placed=0;
		}
		if (placed==1) for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
			if(sct[i][j].altitude!=WATER) sct[i][j].vegetation=GOOD;
		break;
	case FAIR:
		while ((placed == 0)&&(n++<2000)) {
			if(ntn[country].active==1){
#if (MAPX>24)
				x = rand()%(MAPX-24)+12;
				y = rand()%(MAPY-24)+12;
#else
				x = rand()%(MAPX-14)+7;
				y = rand()%(MAPY-14)+7;
#endif
			}
			else {
				x = rand()%(MAPX-10)+5;
				y = rand()%(MAPY-10)+5;
			}

			if(is_habitable(x,y)) placed=1;

			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[i][j].owner!=0) placed=0;

			if(tofood(sct[x][y].vegetation,country)>=DESFOOD){
#if(PWATER>50)
				temp=0;
				for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
					if(sct[i][j].altitude==WATER) temp++;
				if(temp>=7) placed=0;

				/*important that no countries near*/
				for(i=x-3;i<=x+3;i++) for(j=y-3;j<=y+3;j++){
				if((sct[i][j].owner<MAXNTN)
					&&(sct[i][j].owner!=0)) placed=0;
				}
#else
				temp=0;
				for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
					if(sct[i][j].altitude==WATER) temp++;
				if(temp>=5) placed=0;

				/*important that no countries near*/
				for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++){
				if((sct[i][j].owner<MAXNTN)
					&&(sct[i][j].owner!=0)) placed=0;
				}
#endif
			}
			else placed=0;
		}
		if (placed==1) for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
			if(sct[i][j].altitude!=WATER) sct[i][j].vegetation=GOOD;
		break;
	case GREAT:
		placed = 0;
		while ((placed == 0) && (n++<2000)){
			if(ntn[country].active==1){
#if (MAPX>40)
				x = rand()%(MAPX-40)+20;
				y = rand()%(MAPY-40)+20;
#else
				x = rand()%(MAPX-18)+9;
				y = rand()%(MAPY-18)+9;
#endif

				if(is_habitable(x,y)) placed=1;
				/*important that no countries near*/
				for(i=x-4;i<=x+4;i++) for(j=y-4;j<=y+4;j++){
				if((sct[i][j].owner<MAXNTN)
					&&(sct[i][j].owner!=0)) placed=0;
				}
			}
			else {
#if (MAPX>24)
				x = rand()%(MAPX-24)+12;
				y = rand()%(MAPY-24)+12;
#else
				x = rand()%(MAPX-12)+6;
				y = rand()%(MAPY-12)+6;
#endif
				if(is_habitable(x,y)) placed=1;
				/*important that no countries near*/
				for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++){
				if((sct[i][j].owner<MAXNTN)
					&&(sct[i][j].owner!=0)) placed=0;
				}
			}

			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[i][j].owner!=0) placed=0;

			if(tofood(sct[x][y].vegetation,country)>=DESFOOD) {
				temp=0;
				/*if any water within 2 sectors placed = 0*/
				for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++)
					if(tofood(sct[x][y].vegetation,country)<=0)
						temp++;
#if(PWATER>50)
				if(temp>=18) placed=0;
#else
				if(temp>=15) placed=0;
#endif

				if (placed==1) switch(ntn[country].race) {
				case DWARF:
					sct[x][y].altitude=MOUNTAIN;
					for(i=x-1;i<=x+1;i++)
					for(j=y-1;j<=y+1;j++)
					if((i!=x)&&(j!=y)
					&&(sct[i][j].altitude!=WATER)){
						if (rand()%3==0) {
						sct[i][j].altitude=MOUNTAIN;
						sct[i][j].gold=rand()%6;
						sct[i][j].iron=rand()%6;
						}
						else {
						sct[i][j].altitude=HILL;
						sct[i][j].iron=rand()%4;
						sct[i][j].gold=rand()%4;
						}
					}
					break;
				case ELF:
					sct[x][y].vegetation = FOREST;
					for(i=x-1;i<=x+1;i++)
					for(j=y-1;j<=y+1;j++)
					if((i!=x)&&(j!=y)
					&&(sct[i][j].altitude!=WATER)) {
						if (rand()%3==0)
						sct[i][j].vegetation=FOREST;
						else sct[i][j].vegetation=WOOD;

						if (rand()%2==0)
							sct[i][j].gold=rand()%8;
						else sct[i][j].gold=rand()%4;
					}
					break;
				case ORC:
					sct[x][y].altitude=MOUNTAIN;
					for(i=x-1;i<=x+1;i++)
					for(j=y-1;j<=y+1;j++)
					if((i!=x)&&(j!=y)
					&&(sct[i][j].altitude!=WATER)) {
						if (rand()%3==0) {
						sct[i][j].altitude=MOUNTAIN;
						}
						else {
						sct[i][j].altitude=HILL;
						}
						if (rand()%2==0){
							sct[i][j].iron=rand()%8;
							sct[i][j].gold=rand()%8;
						}
						else {
							sct[i][j].iron=rand()%4;
							sct[i][j].gold=rand()%4;
						}
					}
					break;
				case HUMAN:
					sct[x][y].altitude = CLEAR;
					for(i=x-1;i<=x+1;i++)
					for(j=y-1;j<=y+1;j++)
					if((i!=x)&&(j!=y)
					&&(sct[i][j].altitude!=WATER)) {
						if (rand()%2==0)
						sct[x][y].altitude = CLEAR;

						if (rand()%2==0)
						sct[i][j].vegetation=WOOD;
						else
						sct[i][j].vegetation=GOOD;

						if (rand()%2==0)
						sct[i][j].gold=rand()%7+1;
						else
						sct[i][j].iron=rand()%7+1;
					}
					break;
				}
			}
			else placed=0;
		}
	}

	/*done with one try*/
	if(placed==1) {
		ntn[country].capx = x;
		ntn[country].capy = y;
		sct[x][y].designation=DCAPITOL;
		sct[x][y].owner=country;
		sct[x][y].people=ntn[country].tciv;
		sct[x][y].fortress=5;

		/* put all military into armies of 100 */
		armynum=0;
		temp= (int) ntn[country].tmil;
		ASOLD= (int) ntn[country].tmil/3;
		ATYPE=A_INFANTRY;
		temp-=ASOLD;
		ASTAT=GARRISON;
		AMOVE=0;
		AXLOC=ntn[country].capx;
		AYLOC=ntn[country].capy;
		armynum++;
		while ((armynum < MAXARM-1)&&(temp>0)) {
			if(temp>100){
				ASOLD=100;
				ATYPE=A_INFANTRY;
				temp-=100;
			}
			else {
				ASOLD=temp;
				ATYPE=A_INFANTRY;
				temp=0;
			}
			AXLOC=ntn[country].capx;
			AYLOC=ntn[country].capy;
			ASTAT=DEFEND;
			AMOVE=ntn[country].maxmove;
			armynum++;
		}
		if(temp>0) {
			armynum=0;
			ASOLD+=temp;
			ATYPE=A_INFANTRY;
		}

		/* give you some terain to start with: pc nations get more*/
		if ((ntn[country].active>=2)&&(ntn[country].location==GREAT))
			t=1;
		else if (ntn[country].active>=2) t=0;
		else if (ntn[country].location==OOPS) t=0;
		else if (ntn[country].location==RANDOM) t=0;
		else if (ntn[country].location==FAIR) t=1;
		else if (ntn[country].location==GREAT) t=2;
		else printf("error");

		for(i=x-t;i<=x+t;i++) for(j=y-t;j<=y+t;j++)
			if((tofood(sct[i][j].vegetation,country)>=DESFOOD)
			&&(sct[i][j].owner==0)
			&&(sct[i][j].people==0)) {
				sct[i][j].owner=country;
				sct[i][j].designation=DFARM;
			}
	}
	else {
		if(ntn[country].location==OOPS) printf("ERROR\n");
		else if(ntn[country].location==RANDOM) {
			printf("RANDOM PLACE FAILED, TRYING TO PLACE AGAIN\n");
			ntn[country].location=OOPS;
			place();
		}
		else if(ntn[country].location==FAIR) {
			printf("FAIR PLACE FAILED, TRYING AGAIN - adding %d people to nation\n",NLPOP);
			/*give back one point -> NLPOP people*/
			ntn[country].tciv+=NLPOP;
			ntn[country].location=RANDOM;
			place();
		}
		else if(ntn[country].location==GREAT) {
			printf("GOOD PLACE FAILED, TRYING AGAIN - adding %d people to nation\n",NLPOP);
			/*give back one point -> NLPOP people*/
			ntn[country].tciv+=NLPOP;
			ntn[country].location=FAIR;
			place();
		}
	}
}

/*get class routine*/
/* return the number of points needed */
int
getclass(race)
{
	short check=FALSE,i;
	int x;
	short tmp;
	while(check==FALSE){
		printf("what type of nation would you like to be\n");
		if(race!=ORC){
			printf("1. king      (Humans, Dwarves, and Elves)\n");
			printf("2. emperor   (Humans, Dwarves, and Elves)\n");
			printf("3. wizard    (Humans, Dwarves, and Elves)........Cost = 2 Points\n");
			printf("\tA wizard will have the magical SUMMON power automatically. \n");
		}
		if(race==HUMAN){
			printf("4. theocracy (Humans Only).......................Cost = 2 Points\n");
			printf("\tA theocracy will have the magical HEALER power automatically. \n");
		}
		if((race==HUMAN)||(race==ORC)){
			printf("5. pirate    (Humans & Orcs Only)................Cost = 1 Point\n");
			printf("\tPirates will have the magical SAILOR power automatically\n");
		}
		if((race==ELF)||(race==DWARF)){
			printf("6. trader    (Elves & Dwarves Only)..............Cost = 2 Points\n");
			printf("\tTraders have URBAN power automatically\n");
		}
		if((race==HUMAN)||(race==ORC)){
			if(magic(country,WARRIOR)==TRUE)
			printf("7. warlord   (Humans & Orcs Only)................Cost = 2 Point\n");
			else printf("7. warlord   (Humans & Orcs Only)................Cost = 3 Point\n");
			printf("\tWarlords have magical WARLORD power automatically\n");
		}
		if(race==ORC){
			printf("8. demon     (Orcs Only).........................Cost = 2 Points\n");
			printf("\tA demon will have the magical DESTROYER power automatically\n");
			printf("9. dragon    (Orcs Only).........................Cost = 2 Points\n");
			printf("\tA dragon will have the magical MAJOR MONSTER power automatically\n");
			printf("10. shadow    (Orcs Only)........................Cost = 1 Point\n");
			printf("\tA shadow will have the magical VOID power automatically\n");
		}
		printf("\tinput:");
		scanf("%hd",&tmp);
		if((tmp>=1)&&(tmp<=10)) {
			if((race==HUMAN)&&((tmp<6)||(tmp==7))) check=TRUE;
			else if((race==DWARF)&&((tmp<=3)||(tmp==6))) check=TRUE;
			else if((race==ELF)&&((tmp==6)||(tmp<=3))) check=TRUE;
			else if((race==ORC)&&((tmp==5)||(tmp>6))) check=TRUE;
			else printf("bad input \n\n\n");
		} else {
			printf("\tinvalid input\n\n\n");
			getchar();
		}
	}
	ntn[country].class=tmp;
	switch(tmp){
	case 3:
		printf("\nwizards have the magical SUMMON power automatically");
		ntn[country].powers|=SUMMON;
		x=SUMMON;
		CHGMGK;
		return(2);
	case 4:
		printf("\ntheocracies have magical HEALER power automatically. ");
		ntn[country].powers|=HEALER;
		x=HEALER;
		CHGMGK;
		return(2);
	case 5:
		printf("\tPirates have the magical SAILOR power automatically\n");
		ntn[country].powers|=SAILOR;
		x=SAILOR;
		CHGMGK;
		return(1);
	case 6:
		printf("\tTraders have URBAN power automatically\n");
		ntn[country].powers|=URBAN;
		x=URBAN;
		CHGMGK;
		return(2);
	case 7:
		i=0;
		if(magic(country,WARRIOR)!=TRUE){
			ntn[country].powers|=WARRIOR;
			x=WARRIOR;
			CHGMGK;
			i++;
		}
		if(magic(country,CAPTAIN)!=TRUE){
			ntn[country].powers|=CAPTAIN;
			x=CAPTAIN;
			CHGMGK;
			i++;
		}
		if(magic(country,WARLORD)!=TRUE){
			ntn[country].powers|=WARLORD;
			x=WARLORD;
			CHGMGK;
			i++;
		}
		printf("\tWarlords have WARRIOR/CAPTAIN/WARLORD power automatically\n");
		return(i);
	case 8:
		printf("\ndemons have the magical DESTROYER power automatically");
		ntn[country].powers|=DESTROYER;
		x=DESTROYER;
		CHGMGK;
		return(2);
	case 9:
		printf("\ndragons have the magical MAJOR MONSTER power automatically");
		ntn[country].powers|=AV_MONST;
		x=AV_MONST;
		CHGMGK;
		ntn[country].powers|=MA_MONST;
		x=MA_MONST;
		CHGMGK;
		return(2);
	case 10:
		printf("\nshadows have the magical VOID power automatically");
		ntn[country].powers|=THE_VOID;
		x=THE_VOID;
		CHGMGK;
		return(1);
	default:
		return(0);
	}
}
