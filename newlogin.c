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

/*create a new login for a new player*/
#include <ctype.h>

#include "header.h"

extern short country;

newlogin()
{
	/* use points to create empire, add if late starter*/
	int points;
	char tempc[10];
	int n;
	int valid=1;  /*valid == 0 means continue loop*/
	int temp;
	int more=0; 	/*0 if add another player*/
	int loop;
	int x;
	char tempo[8];
	char passwd[8];
	register i;

	printf("\nPreparing to add player\n");
	printf("break at any time to abort\n");

	while(more==0) {
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

		ntn[country].active=1;

		valid=0;
		/*get name*/
		while(valid==0) {
			valid=1;
			printf("\nwhat name would you like your nation to have:");
			scanf("%s",ntn[country].name);

			if((strlen(ntn[country].name)<=1)||(strlen(ntn[country].name)<=1)){
				printf("\ninvalid name");
				valid=0;
			}

			/*check if already used*/
			if((strcmp(ntn[i].name,"god")==0)||(strcmp(ntn[i].name,"unowned")==0)){
				printf("\nname already used");
				valid=0;
			}

			for(i=1;i<(country-1);i++)
				if(strcmp(ntn[i].name,ntn[country].name)==0) {
					printf("\nname already used");
					valid=0;
				}
		}

		/*get password*/
		valid=0;
		while(valid==0) {
			printf("\nwhat is your nations password:");
			scanf("%s",passwd);
			if((strlen(passwd)>=8)||(strlen(passwd)<2)) {
				beep();
				printf("\ninvalid password (too short or long)");
			}
			else {
				printf("\nreenter your nations password:");
				scanf("%s",ntn[country].passwd);
			}
			if(strcmp(passwd,ntn[country].passwd)==0) valid=1;
		}
		strncpy(ntn[country].passwd,crypt(passwd,SALT),PASSLTH);

		/*get your name*/
		valid=0;
		while(valid==0) {
			valid=1;
			printf("\nenter either YOUR name (j_smith) or the name of your nations leader (gandalf...)");
			printf("\n\t(maximum 10 characters, no spaces):");
			scanf("%s",tempc);
			if((strlen(tempc)>=10)||(strlen(tempc)<2)) {
				beep();
				printf("\ninvalid name (too short or long)");
				valid=0;
			}
			else strcpy(ntn[country].leader,tempc);
		}

		valid=0;
		while(valid==0) {
			valid=1;
			printf("\nwhat race would you like to be:");
			printf("\n\tchoose (d)warf,(e)lf,(o)rc,(h)uman:");
			scanf("%s",tempo);
			switch(tempo[0]) {
			case 'd':
				printf("\ndwarf chosen\n");
				getclass(DWARF);
				ntn[country].race=DWARF;
				ntn[country].tgold=100000;	/*1   pts*/
				ntn[country].tfood=12000;     /*0   pts*/
				ntn[country].jewels=10000;     /*0   pts*/
				ntn[country].tiron=10000;     /*0   pts*/
				ntn[country].tciv= 6000;	/*6   pts*/
				ntn[country].tmil= 1500;	/*1.5 pts*/
				ntn[country].repro=   4;	/*4   pts*/
				ntn[country].maxmove= 6;	/*1.5 pts*/
				ntn[country].aplus=   20;	/*2   pts*/
				ntn[country].dplus=   20;	/*2   pts*/
				ntn[country].location=RANDOM;	/*0+  pts*/
				/*MINER POWER INATE TO DWARVES*/
				printf("you have magical MINING skills");
				ntn[country].powers=MINER;
				exenewmgk(MINER);
				points-=18;
				break;
			case 'e':
				printf("\nelf chosen\n");
				getclass(ELF);
				ntn[country].race=ELF;
				ntn[country].tgold=100000;	/*1  pts*/
				ntn[country].tfood=12000;
				ntn[country].jewels=10000;
				ntn[country].tiron=10000;
				ntn[country].tciv=7000;	/*7    pts*/
				ntn[country].tmil=200;		/*0   pts*/
				ntn[country].repro=3;		/*3   pts*/
				ntn[country].maxmove=8;		/*2   pts*/
				ntn[country].aplus=10; 		/*1   pts*/
				ntn[country].dplus=40;		/*4   pts*/
				ntn[country].location=FAIR;	/*1   pts*/
				printf("you have magical cloaking skills");
				ntn[country].powers=THE_VOID;
				exenewmgk(THE_VOID);
				points-=20;	/* VOID COSTS ADDITIONAL PT*/
				break;
			case 'o':
				printf("\norc chosen\n");
				getclass(ORC);
				ntn[country].race=ORC;
				ntn[country].tgold=1000;	/*0   pts*/
				ntn[country].tfood=12000;
				ntn[country].jewels=10000;
				ntn[country].tiron=10000;
				ntn[country].tciv=4000;	/*4   pts*/
				ntn[country].tmil=2500;	/*2.5 pts*/
				ntn[country].repro=10;	/*10  pts*/
				ntn[country].maxmove=6;	/*1.5 pts*/
				ntn[country].aplus=00;	/*0   pts*/
				ntn[country].dplus=00;	/*0   pts*/
				/*MINOR MONSTER POWER INATE TO ORCS*/
				printf("your leader is a minor monster (army 0 is very potent)");
				ntn[country].powers=MI_MONST;
				exenewmgk(MI_MONST);
				ntn[country].location=RANDOM;	/*0   pts*/
				points-=18;
				break;
			case 'h':
				printf("\nhuman chosen\n");
				ntn[country].race=HUMAN;
				getclass(HUMAN);
				ntn[country].tgold=1000;	/*0  pts*/
				ntn[country].tfood=12000;	/*0   pts*/
				ntn[country].jewels=10000;	/*0   pts*/
				ntn[country].tiron=10000;	/*0   pts*/
				ntn[country].tciv=6000;		/*6   pts*/
				ntn[country].tmil=1000;		/*1   pts*/
				ntn[country].repro=5;		/*5   pts*/
				ntn[country].maxmove=10;	/*2.5 pts*/
				ntn[country].aplus=10;	/*1   pts*/
				ntn[country].dplus=10;	/*1   pts*/
				ntn[country].location=RANDOM;	/*0   pts*/
				/*WARRIOR POWER INATE TO HUMANS*/
				printf("you have magical WARRIOR skills");
				ntn[country].powers=WARRIOR;
				exenewmgk(WARRIOR);
				points-=18;
				break;
			default:
				printf("\ninvalid race\n ");
				valid=0;
			}
		}

		ntn[country].mark=(*ntn[country].name);
		printf("\ntesting first letter of name (%c) for nation mark...",ntn[country].mark);
		valid=1;
		while(valid==1) {
			valid=0;
			if((isprint(ntn[country].mark)==0)||(isspace(ntn[country].mark)!=0)) {
				printf("\n%c is not a graphical character",ntn[country].mark);
				valid=1;
			}
			if(valid==0) for(i=0;i<=3;i++) if(ntn[country].mark==(*(ele+i))) {
				printf("\n%c is an altitude character",ntn[country].mark);
				valid=1;
			}
			if(valid==0) for(i=0;i<=11;i++) if(ntn[country].mark==(*(veg+i))) {
				printf("\n%c is a vegitation character,ntn[country].mark");
				valid=1;
			}
			if(valid==0) for(i=1;i<country;i++) if(ntn[i].mark==ntn[country].mark) {
				printf("\n%c is an already used character",ntn[country].mark);
				valid=1;
			}
			if(valid==0) if(ntn[i].mark=='*') {
				printf("\n'*' is invalid character");
				valid=1;
			}
			if(valid==1){
				printf("\nplease reenter new national mark for maps:");
				printf("\n (this can be any printing key like ! or O)");
				scanf("%s",tempc);
				ntn[country].mark=(*tempc);
			}
			else printf("\nvalid\n");
		}

		printf("\nmark currently is %c\n",ntn[country].mark);

		while(points>0) {
			printf("\n\nwhat would you like to buy with your remaining %d points\n\n",points);
			printf("\t1. population (1000/pt):\t\tnow have %d civilians\n",ntn[country].tciv);
			printf("\t2. more gold ($100000/pt):\t\tnow have %d gold pieces\n",ntn[country].tgold);
			printf("\t3. better location:\t\t\tlocation is now is %c\n",ntn[country].location);
			printf("\t4. more soldiers (1000/pt):\t\tnow have %d soldiers\n",ntn[country].tmil);
			printf("\t5. better attack (10%%/pt):\t\tnow is +%d\n ",ntn[country].aplus);
			printf("\t6. better defence (10%%/pt):\t\tnow is +%d\n",ntn[country].dplus);
			printf("\t7. higher reproduction (1%%/pt):\t\trate is now %d%%\n",ntn[country].repro);
			printf("\t8. higher movement (4/pt): \t\tnow move %d sectors\n",ntn[country].maxmove);
			printf("\t9. double raw recourses (1 pt): \tfood now %d\n",ntn[country].tfood);
			printf("\t                                \tgold dust now %d\n",ntn[country].jewels);
			printf("\t                                \tiron now %d\n",ntn[country].tiron);
			printf("\t10. additional random magic power (1 pt)\n");


			printf("\nWhat option to buy:");
			scanf("%d",&n);
			switch(n) {

			case 1:
				printf("additional population costs 1 pt per 1000\n");
				printf("how many points to spend on population:");
				scanf("%d",&temp);
				putchar('\n');
				if(points >= temp) {
					points -= temp;
					ntn[country].tciv+=temp*1000;
				}
				else printf("You dont have enough points left");
				break;
			case 2:
				printf("you now have $%d\n",ntn[country].tgold);
				printf("and can buy gold at $100000 per point\n");
				printf("how many points to spend on added gold:");
				scanf("%d",&temp);
				putchar('\n');
				if(points>=temp)
				{
					points-=temp;
					ntn[country].tgold+=temp*100000;
				}
				else printf("You dont have enough points left");
				break;
			case 3:
				printf("you now are in %c location\n",ntn[country].location);
				printf("  R=random, F=fair, G=good\n");
				if(ntn[country].location==RANDOM){
					printf("2pts for (g)ood location or 1pts for (f)air\n");
				}
				if(ntn[country].location==FAIR){
					printf("1pts for (g)ood location\n");
				}
				if(ntn[country].location==GREAT) return;

				printf("what type of location do you wish:");
				scanf("%s",tempo);
				switch(tempo[0]) {
				case 'g':
				case GREAT:
					if(ntn[country].location==FAIR){
						printf("1pts for (g)ood location\n");
						if(points>=1) {
							points -=1;
							ntn[country].location=GREAT;
						}
						else printf("You dont have enough points left");
					}
					else {
						if(points>=2) {
							points -=2;
							ntn[country].location=GREAT;
						}
						else printf("You dont have enough points left");
					}
					break;
				case 'f':
				case FAIR:
					if(ntn[country].location==RANDOM){
						if(points>=1) {
							points -=1;
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
				printf("you start with %d soldiers\n",ntn[country].tmil);
				printf("additional military costs 1 /  1000\n");
				printf("how many points you wish:");
				scanf("%d",&temp);
				putchar('\n');
				if(points >= temp) {
					points -= temp;
					ntn[country].tmil+=temp*1000;
				}
				else printf("You dont have enough points left");
				break;
			case 5:
				printf("now have %d percent attack bonus\n",ntn[country].aplus);
				printf("an additional 10 percent per point\n");
				printf("how many points you wish:");
				scanf("%d",&temp);
				putchar('\n');
				if(points >= temp) {
					points -= temp;
					ntn[country].aplus+=temp*10;
				}
				else printf("You dont have enough points left");
				break;
			case 6:
				printf("now have %d percent defence bonus\n",ntn[country].dplus);
				printf("an additional 10 percent per point\n");
				printf("how many points you wish:");
				scanf("%d",&temp);
				putchar('\n');
				if(points >= temp) {
					points -= temp;
					ntn[country].dplus+=temp*10;
				}
				else printf("You dont have enough points left");
				break;
			case 7:
				printf("repro rate costs 1 per percent\n");
				printf("you now have %d percent\n",ntn[country].repro);
				if((ntn[country].race!=ORC)&&(ntn[country].repro>=10)){
					printf("you have the maximum rate");
					break;
				}
				printf("how many points you wish:");
				scanf("%d",&temp);
				putchar('\n');
				if(points >= temp) {
					if((ntn[country].race!=ORC)&&(ntn[country].repro+temp>=10)){
					printf("that exceeds the 10% limit");
					}
					else {
					points -= temp;
					ntn[country].repro+=temp;
					}
				}
				else printf("You dont have enough points left");
				break;
			case 8:
				printf("additional movement costs 1 per +4 sct/turn\n");
				printf("you start with a rate of 6\n");
				printf("you now have a rate of 10\n");
				putchar('\n');
				if(points >= 1) {
					points -= 1;
					ntn[country].maxmove+=4;
				}
				else printf("You dont have enough points left");
				break;
			case 9:
				printf("doubling raw materials\n");
				if(points >0) {
					points--;
					ntn[country].tfood*=2;
					ntn[country].jewels*=2;
					ntn[country].tiron*=2;

				}
				break;
			case 10:
				printf("choosing basic magic at 1 point cost\n");
				if(points >0) {
					points--;
					loop=0;
					while(loop==0) if((x=getmagic())>1){
						exenewmgk(x);
						loop=1;
					}
				}
				else printf("sorry not enough points\n");
				break;
			default:
				printf("invalid option\n");
			}
		}

		printnat();
		printf("\nhit 'y' if OK?");
		getchar();
		if(getchar()!='y'){
			ntn[i].active=0;
			getchar();
			printf("\n OK, nation deleted\n");
			printf("\nhit return to add another nation");
			printf("\nhit any other key to continue?");
			if(getchar()=='\n') more=0;
			else more=1;
			putchar('\n');
		}
		else {
			place();
			getchar();
			printf("\nNation is now added to world");
			printf("\nhit return to add another nation");
			printf("\nhit any other key to continue?");
			if(getchar()=='\n') more=0;
			else more=1;
			putchar('\n');
		}
	}
	writedata();
}

printnat()
{
	int i;
	i=country;
	printf("am about to print stats for nation %d\n\n",i);
	printf("name is %8s\n",ntn[i].name);
	printf("leader is %8s\n",ntn[i].leader);
	printf("total sctrs :%2d\n",ntn[i].tsctrs);
	printf("class is %8s\n",*(Class+ntn[i].class));
	printf("mark is %c       \n",ntn[i].mark);
	printf("race is %c       \n",ntn[i].race);
	printf("aplus is +%2d\n",ntn[i].aplus);
	printf("dplus is +%2d\n",ntn[i].dplus);
	printf("total gold is %5d\n",ntn[i].tgold);
	printf("maxmove is %2d sctrs\n",ntn[i].maxmove);
	printf("jewels is %2d\n",ntn[i].jewels);
	printf("total military is %5d\n",ntn[i].tmil);
	printf("total civilians is %5d\n",ntn[i].tciv);
	printf("repro is %2d percent\n",ntn[i].repro);
	printf("total iron :%5d\n",ntn[i].tiron);
	printf("total food :%5d\n",ntn[i].tfood);
	printf("total ships :%2d\n",ntn[i].tships);
}

/*PLACE NATION*/
place()
{
	int placed=0;
	int t;
	int temp;
	int n; /*count vbl for inf loop*/
	short armynum=0;
	register i,j,x,y;

	n=0;
	updmove(ntn[country].race);
	switch(ntn[country].location) {
	case OOPS:
		while ((placed == 0)&&(n++<800)){
			placed=1;
			if(ntn[country].active==1){
				x = (rand()%(MAPX-8))+4;
				y = (rand()%(MAPY-8))+4;
			}
			else {
				x = (rand()%(MAPX-2))+1;
				y = (rand()%(MAPY-2))+1;
			}

			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[x][y].owner!=0) placed=0;

			if((movecost[x][y]<=0)||(movecost[x][y]>5)) placed=0;
		}
		break;
	case RANDOM:
		while ((placed == 0)&&(n<2000)){
			placed=1;
			if(ntn[country].active==1){
#if (MAPX>12)
				x = rand()%(MAPX-12)+6;
				y = rand()%(MAPY-12)+6;
#else
				x = rand()%(MAPX-8)+4;
				y = rand()%(MAPY-8)+4;
#endif
				/*important that no countries near*/
				for(i=x-4;i<=x+4;i++) for(j=y-4;j<=y+4;j++)
					if((sct[i][j].owner<MAXNTN)&&(sct[i][j].owner!=0)) placed=0;
			}
			else {
				x = (rand()%(MAPX-6))+3;
				y = (rand()%(MAPY-6))+3;
				/*important that no countries near*/
				for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++)
					if((sct[i][j].owner<MAXNTN)&&(sct[i][j].owner!=0)) placed=0;
			}
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[x][y].owner!=0) placed=0;
			if((movecost[x][y]<=0)||(movecost[x][y]>4)) placed=0;
			n++;
		}
		if (placed==1) for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
			if(sct[i][j].altitude!=WATER) sct[i][j].vegitation=GOOD;
		break;
	case FAIR:
		while ((placed == 0)&&(n<500)) {
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

			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[x][y].owner!=0) placed=0;

			n++;
			placed=1;
			if((movecost[x][y]>0)&&(movecost[x][y]<=4)){
#if(PWATER>50)
				for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
					if(sct[i][j].altitude==WATER) placed=0;
				/*important that no countries near*/
				for(i=x-3;i<=x+3;i++) for(j=y-3;j<=y+3;j++){
				if((sct[i][j].owner<MAXNTN)&&(sct[i][j].owner!=0)) placed=0;
				}
#else
				for(i=x-1;i<=x;i++) for(j=y-1;j<=y;j++)
					if(sct[i][j].altitude==WATER) placed=0;
				/*important that no countries near*/
				for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++){
				if((sct[i][j].owner<MAXNTN)&&(sct[i][j].owner!=0)) placed=0;
				}
#endif
			}
			else placed=0;
		}
		if (placed==1) for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
			if(sct[i][j].altitude!=WATER) sct[i][j].vegitation=GOOD;
		break;
	case GREAT:
		placed = 0;
		while ((placed == 0) && (n<1500)){
			placed=1;
			if(ntn[country].active==1){
#if (MAPX>40)
				x = rand()%(MAPX-40)+20;
				y = rand()%(MAPY-40)+20;
#else
				x = rand()%(MAPX-18)+9;
				y = rand()%(MAPY-18)+9;
#endif

				/*important that no countries near*/
				for(i=x-4;i<=x+4;i++) for(j=y-4;j<=y+4;j++){
				if((sct[i][j].owner<MAXNTN)&&(sct[i][j].owner!=0)) placed=0;
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
				for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++){
				if((sct[i][j].owner<MAXNTN)&&(sct[i][j].owner!=0)) placed=0;
				}
			}

			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[x][y].owner!=0) placed=0;

			n++;  /*variable to prevent infinite loop*/
			if((movecost[x][y]>0)&&(movecost[x][y]<4)){
#if(PWATER>50)
				/*if any water within 2 sectors placed = 0*/
				for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++)
					if(movecost[x][y]<0) placed=0;
#else 
				for(i=x-2;i<=x+1;i++) for(j=y-2;j<=y+1;j++)
					if(movecost[x][y]<0) placed=0;
#endif

				if (placed==1) switch(ntn[country].race) {
				case DWARF:
					sct[x][y].altitude=MOUNTAIN;
					sct[x][y].designation=MOUNTAIN;
					for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++) if((i!=x)&&(j!=y)&&(sct[i][j].altitude!=WATER)){
						if (rand()%3==0) {
						sct[i][j].altitude=MOUNTAIN;
						sct[i][j].designation=MOUNTAIN;
						sct[i][j].gold=rand()%6;
						sct[i][j].iron=rand()%6;
						}
						else {
						sct[i][j].altitude=HILL;
						sct[i][j].designation=HILL;
						sct[i][j].iron=rand()%4;
						sct[i][j].gold=rand()%4;
						}
					}
					break;
				case ELF:
					sct[x][y].vegitation = FORREST;
					for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++) if((i!=x)&&(j!=y)&&(sct[i][j].altitude!=WATER)) {
						if (rand()%3==0)
						sct[i][j].vegitation=FORREST;
						else sct[i][j].vegitation=WOOD;

						if (rand()%2==0)
							sct[i][j].gold=rand()%8;
						else sct[i][j].gold=rand()%4;
					}
					break;
				case ORC:
					sct[x][y].altitude=MOUNTAIN;
					for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++) if((i!=x)&&(j!=y)&&(sct[i][j].altitude!=WATER)) {
						if (rand()%3==0) {
						sct[i][j].altitude=MOUNTAIN;
						sct[i][j].designation=MOUNTAIN;
						}
						else {
						sct[i][j].altitude=HILL;
						sct[i][j].designation=HILL;
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
					for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++) if((i!=x)&&(j!=y)&&(sct[i][j].altitude!=WATER)) {
						if (rand()%2==0)
						sct[x][y].altitude = CLEAR;

						if (rand()%2==0)
						sct[i][j].vegitation=WOOD;
						else 
						sct[i][j].vegitation=GOOD;

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
		temp=ntn[country].tmil;
		ASOLD=ntn[country].tmil/3;
		temp-=ASOLD;
		ASTAT=GARRISON;
		AMOVE=0;
		AXLOC=ntn[country].capx;
		AYLOC=ntn[country].capy;
		while((armynum<MAXARM)&&(temp>0)) {
			armynum++;
			if(temp>100){
				ASOLD=100;
				temp-=100;
			}
			else {
				ASOLD=temp;
				temp=0;
			}
			AXLOC=ntn[country].capx;
			AYLOC=ntn[country].capy;
			ASTAT=DEFEND;
			AMOVE=ntn[country].maxmove;
		}
		if(temp>0) {
			armynum=0;
			ASOLD+=temp;
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
			if((movecost[i][j]>=0)&&(sct[i][j].owner==0)&&(sct[i][j].people==0)) {
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
			printf("FAIR PLACE FAILED, TRYING TO PLACE AGAIN\n");
			ntn[country].location=RANDOM;
			place();
		}
		else if(ntn[country].location==GREAT) {
			printf("GOOD PLACE FAILED, TRYING TO PLACE AGAIN\n");
			ntn[country].location=FAIR;
			place();
		}
	}
}

/*get class routine*/
getclass(race)
{
	short check=0;
	short tmp;
	while(check==0){
		printf("what type of nation would you like to be\n");
		printf("1. king (Humans, Dwarves, and Elves)\n");
		printf("2. emperor (Humans, Dwarves, and Elves)\n");
		printf("3. wizard (Humans, Dwarves, and Elves)\n");
		printf("4. theocracy (Humans Only)\n");
		printf("5. pirate (Humans & Orcs Only)\n");
		printf("6. trader (Elves & Dwarves Only)\n");
		printf("7. tyrant (Humans & Orcs Only)\n");
		printf("8. demon  (Orcs Only)\n");
		printf("9. dragon (Orcs Only)\n");
		printf("10. shadow (Orcs Only)\n\n");
		printf("\tinput:");
		scanf("%hd",&tmp);
		if((tmp>=1)&&(tmp<=10)) {
			if((race==HUMAN)&&((tmp<6)||(tmp==7))) check=1;
			else if((race==DWARF)&&((tmp<=3)||(tmp==6))) check=1;
			else if((race==ELF)&&((tmp==6)||(tmp<=3))) check=1;
			else if((race==ORC)&&((tmp==5)||(tmp>6))) check=1;
			else printf("bad input \n\n\n");
		} 
		else printf("\tinvalid input\n\n\n");
	}
	ntn[country].class=tmp;
}
