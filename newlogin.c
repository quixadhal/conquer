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

extern int pwater;		/* percent water in world (0-100) */
extern FILE *fexe, *fopen();
extern short country;
int	numleaders;

/* Teraform the area around somebodies capitol */
/* this gives everybody some chance of success */
void
teraform( x,y,range, chance )
int x,y;
int range,chance;
{
	int i,j;
	switch(curntn->race) {
	case DWARF:
		sct[x][y].altitude=MOUNTAIN;
		for(i=x-range;i<=x+range;i++)
		for(j=y-range;j<=y+range;j++)
		if((i!=x)&&(j!=y)&&(ONMAP(i,j))
		&&(sct[i][j].altitude!=WATER)){
			if (rand()%3==0)
				sct[i][j].altitude=MOUNTAIN;
			else	sct[i][j].altitude=HILL;

			if( rand()%100 < chance ) getmetal( &sct[i][j] );
		}
		return;
	case ELF:
		sct[x][y].vegetation = FOREST;
		for(i=x-range;i<=x+range;i++)
		for(j=y-range;j<=y+range;j++)
		if((i!=x)&&(j!=y)&&(ONMAP(i,j))
		&&(sct[i][j].altitude!=WATER)) {
			if (rand()%3==0)
				sct[i][j].vegetation=FOREST;
			else	sct[i][j].vegetation=WOOD;
			if( rand()%100 < chance ) getjewel( &sct[i][j] );
		}
		return;
	case ORC:
		sct[x][y].altitude=MOUNTAIN;
		for(i=x-range;i<=x+range;i++)
		for(j=y-range;j<=y+range;j++)
		if((i!=x)&&(j!=y)&&(ONMAP(i,j))
		&&(sct[i][j].altitude!=WATER)) {
			if (rand()%3==0)
				sct[i][j].altitude=MOUNTAIN;
			else	sct[i][j].altitude=HILL;

			if( rand()%100 < chance ) {
				if(rand()%2==0)
					getmetal( &sct[i][j] );
				else	getjewel( &sct[i][j] );
			}
		}
		return;
	case HUMAN:
		sct[x][y].altitude = CLEAR;
		for(i=x-range;i<=x+range;i++)
		for(j=y-range;j<=y+range;j++)
		if((i!=x)&&(j!=y)&&(ONMAP(i,j))
		&&(sct[i][j].altitude!=WATER)) {
			if (rand()%2==0)
			sct[x][y].altitude = CLEAR;

			if (rand()%2==0)
				sct[i][j].vegetation=WOOD;
			else	sct[i][j].vegetation=GOOD;

			if( rand()%100 < chance ) {
				if (rand()%2==0)
					getmetal( &sct[i][j] );
				else	getjewel( &sct[i][j] );
			}
		}
		return;
	}
}

void
mailtopc(string)
char	*string;
{
	int	ctry;
	for( ctry=0; ctry<NTOTAL; ctry++){
		if((ctry==0)||(ispc(ntn[ctry].active))){
			mailopen(ctry);
			fprintf(fm,"%s",string);
			mailclose();
		}
	}
}

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
	char tempo[8],strin[80];
	char passwd[PASSLTH+1],*getpass();
	register i;

	printf("\nPreparing to add player\n");

	while(more==TRUE) {
		points=MAXPTS;
		country=0;
		/*find valid nation number type*/
		for(i=1;i<NTOTAL;i++)
			if(ntn[i].active==INACTIVE) {
				country=i;
				curntn = &ntn[country];
				break;
			}
		printf("first valid nation id is %d\n",country);

		if(country==0) {
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

		valid=FALSE;
		/*get name*/
		while(valid==FALSE) {
			valid=TRUE;
			printf("\nwhat name would you like your nation to have:");
			gets(curntn->name);

			if((strlen(curntn->name)<=1)
			 ||(strlen(curntn->name)>NAMELTH)){
				printf("\ninvalid name");
				valid=FALSE;
			}

			/*check if already used*/
			if((strcmp(curntn->name,"god")==0)
			||(strcmp(curntn->name,"unowned")==0)){
				printf("\nname already used");
				valid=FALSE;
			}

			for(i=1;i<NTOTAL;i++)
				if((i!=country)&&(strcmp(ntn[i].name,curntn->name)==0)) {
					printf("\nname already used");
					valid=FALSE;
				}
		}


		valid=FALSE;
		while(valid==FALSE) {			/* password routine */
			strncpy(passwd,getpass("Enter national password (remember this!):"),PASSLTH);
			strncpy(curntn->passwd,getpass("Please reenter password:"),PASSLTH);
			if((strlen(passwd)<2)
			||(strncmp(curntn->passwd,passwd,PASSLTH)!=0)){
				beep();
				printf("\ninvalid user password\n");
			} else valid=TRUE;
		}
		strncpy(curntn->passwd,crypt(passwd,SALT),PASSLTH);

		/*get your name*/
		valid=FALSE;
		while(valid==FALSE) {
			valid=TRUE;
			printf("\nEnter the name of your country's leader (Groo, The Ed, Gandalf...)");
			printf("\n\t(maximum %d characters):",LEADERLTH);
			gets(tempc);
			if((strlen(tempc)>LEADERLTH)||(strlen(tempc)<2)) {
				beep();
				printf("\ninvalid name (too short or long)");
				valid=FALSE;
			}
			else strcpy(curntn->leader,tempc);
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
				printf("Dwarves have MINING skills\n");
				curntn->powers=MINER;
				x=MINER;
				CHGMGK;
				points -= getclass(DWARF);
				curntn->race=DWARF;
				curntn->tgold=NLDGOLD;
				curntn->tfood=NLDFOOD;
				curntn->jewels=NLDJEWEL;
				curntn->metals=NLDMETAL;
				curntn->tciv= NLDCIVIL;
				curntn->tmil= NLDMILIT;
				curntn->repro= NLDREPRO;
				curntn->maxmove= NLDMMOVE;
				curntn->aplus= NLDAPLUS;
				curntn->dplus= NLDDPLUS;
				curntn->location=RANDOM;
				points-=startcost();
				break;
			case 'e':
				printf("\nelf chosen\n");
				printf("Elves are magically cloaked (VOID power)\n");
				curntn->powers=THE_VOID;
				x=THE_VOID;
				CHGMGK;
				points -= getclass(ELF);
				curntn->race=ELF;
				curntn->tgold=NLEGOLD;
				curntn->tfood=NLEFOOD;
				curntn->jewels=NLEJEWEL;
				curntn->metals=NLEMETAL;
				curntn->tciv=NLECIVIL;
				curntn->tmil=NLEMILIT;
				curntn->repro=NLEREPRO;
				curntn->maxmove=NLEMMOVE;
				curntn->aplus=NLEAPLUS;
				curntn->dplus=NLEDPLUS;
				curntn->location=FAIR;
				points-=startcost();
				break;
			case 'o':
				printf("\norc chosen\n");
				/*MINOR MONSTER POWER INATE TO ORCS*/
				printf("your leader is a monster!\n");
				curntn->powers=MI_MONST;
				x=MI_MONST;
				CHGMGK;
				points -= getclass(ORC);
				curntn->race=ORC;
				curntn->tgold=NLOGOLD;
				curntn->tfood=NLOFOOD;
				curntn->jewels=NLOJEWEL;
				curntn->metals=NLOMETAL;
				curntn->tciv=NLOCIVIL;
				curntn->tmil=NLOMILIT;
				curntn->repro=NLOREPRO;
				curntn->maxmove=NLOMMOVE;
				curntn->aplus=NLOAPLUS;
				curntn->dplus=NLODPLUS;
				curntn->location=RANDOM;
				points-=startcost();
				break;
			case 'h':
				printf("\nhuman chosen\n");
				curntn->race=HUMAN;
				printf("Humans have the combat skill of a WARRIOR\n");
				curntn->powers = WARRIOR;
				x=WARRIOR;
				CHGMGK;
				points -= getclass(HUMAN);
				curntn->tgold=NLHGOLD;
				curntn->tfood=NLHFOOD;
				curntn->jewels=NLHJEWEL;
				curntn->metals=NLHMETAL;
				curntn->tciv=NLHCIVIL;
				curntn->tmil=NLHMILIT;
				curntn->repro=NLHREPRO;
				curntn->maxmove=NLHMMOVE;
				curntn->aplus=NLHAPLUS;
				curntn->dplus=NLHDPLUS;
				curntn->location=RANDOM;
				points-=startcost();
				break;
			default:
				printf("\ninvalid race\n ");
				valid=0;
			}
		}

		valid=FALSE;
		if( curntn->race == ORC ) {	/* orcs are always evil */
			valid=TRUE;
			curntn->active=PC_EVIL;
		}
		while(valid==FALSE) {
			valid=TRUE;
			printf("\nplease enter alignment ((g)ood,(n)eutral,(e)vil): ");
			scanf("%1s",tempo);
			switch(tempo[0]) {
				case 'g': curntn->active=PC_GOOD; break;
				case 'n': curntn->active=PC_NEUTRAL; break;
				case 'e': curntn->active=PC_EVIL; break;
				default :
					printf("\nsorry - please enter alignment ((g)ood,(n)eutral,(e)vil): ");
					valid=FALSE;
					break;
			}
		}
		valid = npctype(curntn->active);
		printf("ok... alignment is %s\n",allignment[valid]);

		/* get nation mark */
		valid=FALSE;
		curntn->mark= (*curntn->name);
		printf("\ntesting first letter of name (%c) for nation mark...",curntn->mark);
		if( markok(curntn->mark, TRUE)==TRUE )
			curntn->mark= curntn->mark;
		else if( islower(curntn->mark) &&
		markok( toupper(curntn->mark), TRUE )==TRUE )
			curntn->mark= toupper(curntn->mark);
		else if( isupper(curntn->mark) &&
		markok( tolower(curntn->mark) , TRUE)==TRUE )
			curntn->mark= tolower(curntn->mark);
		else while(TRUE) {
			printf("\nplease enter new national mark (for maps):");
			printf("\n This can be any of the following:");
			for (tempc[0]='!';tempc[0]<='~';tempc[0]++) {
				if( markok( tempc[0], FALSE ) )
					printf(" %c",tempc[0]);
			}
			printf("\n");
			scanf("%1s",tempc);
			if( markok( tempc[0], TRUE ) ){
				curntn->mark=(*tempc);
				printf("\nvalid...");
				break;
			}
		}

		printf("mark currently is %c\n",curntn->mark);

		while(points>0) {
			printf("\n\nwhat would you like to buy with your remaining %d points\n\n",points);
			printf("\t1. population (%ld/pt):\t\tnow have %ld civilians\n",NLPOP,curntn->tciv);
			printf("\t2. more gold talons ($%ld/pt):\tnow have %ld gold talons\n",NLGOLD,curntn->tgold);
			printf("\t3. better location (%d pt):\t\tlocation is now is %c\n",NLLOCCOST,curntn->location);
			printf("\t4. more soldiers (%ld/pt):\t\tnow have %ld soldiers\n",NLSOLD,curntn->tmil);
			if( curntn->race != ORC ){
			printf("\t5. better attack (%d%%/pt):\t\tnow is +%d\n ",NLATTACK,curntn->aplus);
			printf("\t6. better defence (%d%%/pt):\t\tnow is +%d\n",NLDEFENCE,curntn->dplus);
			printf("\t7. higher reproduction (+%d%%/%d pt):\trate is now %d%%/year\n",NLREPRO,NLREPCOST,curntn->repro);
			printf("\t8. higher movement (%d/pt): \t\tnow move %d sectors\n",NLMOVE,curntn->maxmove);
			} else {
			printf("\t5. better attack (%d%%/pt):\t\tnow is +%d\n ",NLATTACK/2,curntn->aplus);
			printf("\t6. better defence (%d%%/pt):\t\tnow is +%d\n",NLDEFENCE/2,curntn->dplus);
			printf("\t7. higher reproduction (+%d%%/%d pt):\trate is now %d%%/year\n",NLREPRO_ORC,NLREPCOST,curntn->repro);
			}
			printf("\t9. double raw resources (%ld pts): \tfood now %ld\n",NLDBLCOST*curntn->tfood/NLHFOOD,curntn->tfood);
			printf("\t                                \tjewels now %ld\n",curntn->jewels);
			printf("\t                                \tmetals now %ld\n",curntn->metals);
			printf("\t10. additional random magic power (%d pts)\n",NLMAGIC);
			printf("\t11. additional %d leaders (%d pts)\tnation has %d %ss\n",NLEADER,NLEADPT,numleaders,unittype[getleader(curntn->class)%UTYPE]);

			printf("\nWhat option to buy:");
			if(scanf("%d",&n)==1) switch(n) {

			case 1:
				printf("additional population costs 1 pt per %d\n",NLPOP);
				printf("how many points to spend on population:");
				scanf("%d",&temp);
				putchar('\n');
				if(points <= 0) {
					printf("Purchase aborted...");
				} if(points >= temp) {
					points -= temp;
					curntn->tciv+=temp*NLPOP;
				}
				else printf("You dont have enough points left");
				break;
			case 2:
				printf("you now have $%ld gold talons\n",curntn->tgold);
				printf("and can buy more at $%ld per point\n",NLGOLD);
				printf("how many points to spend on added gold talons:");
				scanf("%d",&temp);
				putchar('\n');
				if(points<=0) {
					printf("Purchase aborted....");
				} else if(points>=temp) {
					points-=temp;
					curntn->tgold+=temp*NLGOLD;
				}
				else printf("You dont have enough points left");
				break;
			case 3:
				if(curntn->location==GREAT) break;

				if(points>=NLLOCCOST) {
					points -=NLLOCCOST;
					if(curntn->location==RANDOM){
						curntn->location=FAIR;
					} else if(curntn->location==FAIR){
						curntn->location=GREAT;
					}
				} else {
					printf("Too Few Points Left...");
					break;
				}
				break;
			case 4:
				printf("you start with %ld soldiers\n",curntn->tmil);
				printf("additional military costs 1 / %d\n",NLSOLD);
				printf("how many points to spend?");
				scanf("%d",&temp);
				putchar('\n');
				if (points <= 0) {
					printf("Purchase aborted...");
				} else if(points >= temp) {
					points -= temp;
					curntn->tmil+=temp*NLSOLD;
				}
				else printf("You dont have enough points left");
				break;
			case 5:
				printf("now have %d percent attack bonus\n",curntn->aplus);
				if(curntn->race == ORC ) {
					printf("orcs cost additional for combat bonuses\n");
					printf("an additional %d percent per point\n",NLATTACK/2);
				} else
				printf("an additional %d percent per point\n",NLATTACK);
				printf("how many points do you wish to spend?");
				scanf("%d",&temp);
				putchar('\n');
				if(points <= 0) {
					printf("Purchase aborted...");
				} else if(points >= temp) {
					points -= temp;
					if(curntn->race == ORC )
					curntn->aplus+=temp*NLATTACK/2;
					else
					curntn->aplus+=temp*NLATTACK;
				}
				else printf("You dont have enough points left");
				break;
			case 6:
				if(magic(country,VAMPIRE)==1) {
				printf("you have vampire power and cant add to combat bonus\n");
				break;
				}
				printf("now have %d percent defence bonus\n",curntn->dplus);
				if(curntn->race == ORC ) {
					printf("orcs cost additional for combat bonuses\n");
					printf("an additional %d percent per point\n",NLDEFENCE/2);
				} else
				printf("an additional %d percent per point\n",NLDEFENCE);
				printf("how many points do you wish to spend?");
				scanf("%d",&temp);
				putchar('\n');
				if(points <= 0) {
					printf("Purchase aborted...");
				} else if(points >= temp) {
					points -= temp;
					if(curntn->race == ORC )
					curntn->dplus+=temp*NLDEFENCE/2;
					else
					curntn->dplus+=temp*NLDEFENCE;
				}
				else printf("You dont have enough points left");
				break;
			case 7:
				if(curntn->race==ORC)
				printf("repro rate costs %d points per %d percent per year\n",NLREPCOST,NLREPRO_ORC);
				else
				printf("repro rate costs %d points per %d percent per year\n",NLREPCOST,NLREPRO);
				printf("you now have %d percent\n",curntn->repro);
				if((curntn->race!=ORC)
				&&(curntn->repro>=10)){
					printf("you have the maximum rate");
					break;
				}
				else if(curntn->repro>=14){
					printf("you have the maximum rate");
					break;
				}
				printf("how many purchasing points to spend?:");
				scanf("%d",&temp);
				putchar('\n');
				if(temp < points) {
					printf("You don't have enough points left");
				} else if (temp < 0) {
					printf("Negative, huh?  Who you trying to kid?");
				} else if (temp%NLREPCOST != 0) {
					printf("You must spend in multiples of %d",NLREPCOST);
				} else {
					if(curntn->race != ORC) {
						temp = temp/NLREPCOST*NLREPRO;
						if(curntn->repro+temp > 10) {
							printf("That exceeds the 10% limit");
						} else {
							points -= (temp*NLREPCOST/NLREPRO);
							curntn->repro += temp;
						}
					} else {
						temp = temp/NLREPCOST*NLREPRO_ORC;
						if(curntn->repro+temp > 14) {
							printf("That exceeds the 14% limit");
						} else {
							points -= (temp*NLREPCOST/NLREPRO_ORC);
							curntn->repro += temp;
						}
					}
				}
				break;
			case 8:
				if(curntn->race == ORC ) {
					printf("orcs cant add to movement\n");
					break;
				}
				printf("additional movement costs 1 per +%d sct/turn\n",NLMOVE);
				printf("you start with a rate of %d\n",curntn->maxmove);
				printf("you now have a rate of %d\n",curntn->maxmove+NLMOVE);
				putchar('\n');
				if(points >= 1) {
					points -= 1;
					curntn->maxmove+=NLMOVE;
				}
				else printf("You dont have enough points left");
				break;
			case 9:
				printf("doubling raw materials\n");
				if((curntn->tfood<800000L)
				&&(points >=NLDBLCOST*curntn->tfood/NLHFOOD)) {
					points-=NLDBLCOST*curntn->tfood/NLHFOOD;
					curntn->tfood*=2;
					curntn->jewels*=2;
					curntn->metals*=2;
				}
				else printf("sorry\n");
				break;
			case 10:
				printf("choosing basic magic at %d point cost\n",NLMAGIC);
				printf("log in and read the magic screen to be informed of your powers\n");
				if(points >0) {
					points-=NLMAGIC;
					loop=0;
					while(loop==0) if((x=getmagic((rand()%M_MGK+M_MIL)))!=0){
						CHGMGK;
						loop=1;
					}
				}
				else printf("sorry not enough points\n");
				break;
			case 11:
				/* check if not more than 1/2 armies will be leaders */
				if( numleaders + NLEADER > MAXARM/2 ) {
				printf("\tsorry... that would give you too many leaders\n");
				break;
				}
				if( points >= NLEADPT ) {
				printf("\tadding %d leaders for %d points\n",NLEADER,NLEADPT);
				numleaders += NLEADER;
				points -= NLEADPT;
				} else printf("sorry not enough points\n");
				break;
			default:
				printf("invalid option - hit return");
				scanf("%*s");
			} else	scanf("%*s");
		}
		check();

		printnat();
		printf("\nhit 'y' if OK?");
		getchar();
		if(getchar()!='y'){
			curntn->active=INACTIVE;
			getchar();
			curntn->powers=0;
			printf("\n OK, nation deleted\n");
			printf("\nhit return to add another nation");
			printf("\nhit any other key to continue?");
			if(getchar()=='\n') more=TRUE;
			else more=FALSE;
			putchar('\n');
			fclose(fexe);
		}
		else {
			place(-1,-1);
			getchar();
			printf("\nNation is now added to world");
			printf("\nhit return to add another nation");
			printf("\nhit any other key to continue?");
			if(getchar()=='\n') more=TRUE;
			else more=FALSE;
			putchar('\n');
			att_setup(country);	/* setup values ntn attributes */
			fclose(fexe);
			sprintf(strin,"NOTICE: Nation %s added to world on turn %d\n",curntn->name,TURN);
			mailtopc(strin);
			/* cannot clear until after placement and initializing */
			curntn->powers=0;
		}
	}
	att_base();	/* calculate base nation attributes */
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
	printf("gold talons......%ld\n",ntn[i].tgold);
	printf("maxmove is ......%d sctrs\n",ntn[i].maxmove);
	printf("jewels is .......%ld\n",ntn[i].jewels);
	printf("# military ......%ld\n",ntn[i].tmil);
	printf("# civilians .....%ld\n",ntn[i].tciv);
	printf("repro is ........%d percent\n",ntn[i].repro);
	printf("total metal .....%ld\n",ntn[i].metals);
	printf("total food ......%ld\n",ntn[i].tfood);
	printf("total ships .....%d\n",ntn[i].tships);
	printf("total leaders ...%d\n",numleaders);
}

/*****************************************************************/
/* PLACE(): put nation on the map.  Fill out army structures too */
/*****************************************************************/
void
place(xloc,yloc)
int	xloc,yloc;	/* if not -1,-1 should place in this spot */
{
	int	placed=0,armysize=100;
	short	armynum=0;
	long	people;
	int	x,y,i,j,temp,t;
	int	n=0, leadtype;
	long	soldsleft;	/* soldiers left to place */

	if( xloc != -1 && yloc != -1 && is_habitable(xloc,yloc)) {
		placed=1;
		x = xloc;
		y = yloc;
	}

	switch(curntn->location) {
	case OOPS:
		while((placed == 0)&&(n++<2000)){
			if(ispc(curntn->active)){
				x = (rand()%(MAPX-8))+4;
				y = (rand()%(MAPY-8))+4;
			} else {
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
		}
		teraform( x,y,1,25 );
		break;
	case RANDOM:
		while ((placed == 0)&&(n++<2000)){
			if(ispc(curntn->active)){
				if(MAPX>12){
					x = rand()%(MAPX-12)+6;
					y = rand()%(MAPY-12)+6;
				} else {
					x = rand()%(MAPX-8)+4;
					y = rand()%(MAPY-8)+4;
				}
				if(is_habitable(x,y)) placed=1;
				/*important that no countries near*/
				for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++)
					if((isntn(ntn[sct[i][j].owner].active))
					&&(sct[i][j].owner!=0)) placed=0;
			} else {
				x = (rand()%(MAPX-6))+3;
				y = (rand()%(MAPY-6))+3;
				if(is_habitable(x,y)) placed=1;
				/*important that no countries near*/
				for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++)
					if((isntn(ntn[sct[i][j].owner].active))
					&&(sct[i][j].owner!=0)) placed=0;
			}
			temp=0;
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[i][j].altitude==WATER) temp++;
			if(temp>=7) placed=0;
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[i][j].owner!=0) placed=0;
		}
		teraform( x,y,1,40 );
		break;
	case FAIR:
		while ((placed == 0)&&(n++<2000)) {
			if(ispc(curntn->active)){
				if(MAPX>24) {
					x = rand()%(MAPX-24)+12;
					y = rand()%(MAPY-24)+12;
				} else {
					x = rand()%(MAPX-14)+7;
					y = rand()%(MAPY-14)+7;
				}
			} else {
				x = rand()%(MAPX-10)+5;
				y = rand()%(MAPY-10)+5;
			}

			if(!is_habitable(x,y)) continue;
			if(tofood( &sct[x][y],country)<DESFOOD) continue;

			placed=1;
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[i][j].owner!=0) placed=0;

			if(pwater>50) {
				temp=0;
				for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[i][j].altitude==WATER) temp++;
				if(temp>=7) placed=0;

				/*important that no countries near*/
				for(i=x-3;i<=x+3;i++) for(j=y-3;j<=y+3;j++){
				if((isntn(ntn[sct[i][j].owner].active))
					&&(sct[i][j].owner!=0)) placed=0;
				}
			} else {
				temp=0;
				for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
					if(sct[i][j].altitude==WATER) temp++;
				if(temp>=5) placed=0;

				/*important that no countries near*/
				for(i=x-3;i<=x+3;i++) for(j=y-3;j<=y+3;j++){
				if((isntn(ntn[sct[i][j].owner].active))
				&&(sct[i][j].owner!=0)) placed=0;
				}
			}
		}

		teraform( x,y,1,65 );
		break;
	case GREAT:
		placed = 0;
		while ((placed == 0) && (n++<2000)){
			if(ispc(curntn->active)){
				if (MAPX>40){
					x = rand()%(MAPX-40)+20;
					y = rand()%(MAPY-40)+20;
				}else{
					x = rand()%(MAPX-18)+9;
					y = rand()%(MAPY-18)+9;
				}

				if(is_habitable(x,y)) placed=1;
				/*important that no countries near*/
				for(i=x-4;i<=x+4;i++) for(j=y-4;j<=y+4;j++){
				if((isntn(ntn[sct[i][j].owner].active))
				&&( sct[i][j].owner!=0)) placed=0;
				}
			} else {
				if(MAPX>24){
					x = rand()%(MAPX-24)+12;
					y = rand()%(MAPY-24)+12;
				}else {
					x = rand()%(MAPX-12)+6;
					y = rand()%(MAPY-12)+6;
				}
				if(is_habitable(x,y)) placed=1;
				/*important that no countries near*/
				for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++){
				if((isntn(ntn[sct[i][j].owner].active))
					&&(sct[i][j].owner!=0)) placed=0;
				}
			}

			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[i][j].owner!=0) placed=0;

			temp=0;
			/*if any water within 2 sectors placed = 0*/
			for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++)
				if(tofood( &sct[x][y],country)<=0)
					temp++;

			if( pwater>50 ) {
				if(temp>=18) placed=0;
			} else {
				if(temp>=15) placed=0;
			}
		}
		teraform( x,y,1,100 );
	}

	/*done with one try*/
	if(placed==1) {
		curntn->capx = x;
		curntn->capy = y;
		sct[x][y].designation=DCAPITOL;
		sct[x][y].tradegood=rand()%(END_KNOWLEDGE-END_SPOILRATE)+END_SPOILRATE+1;
		sct[x][y].jewels=0;
		sct[x][y].metal=0;
		sct[x][y].owner=country;
		sct[x][y].people=curntn->tciv;
		sct[x][y].fortress=5;

		/* put all military into armies of armysize */
		armysize = (TAKESECTOR*12)/10;
		if(armysize<100) armysize=100;
		/* cant have more than 50% leaders */
		if( MAXARM < numleaders * 2 ) numleaders = MAXARM / 2;
		armynum=0;
		soldsleft = curntn->tmil;
		P_ASOLD = curntn->tmil/MILINCAP;
		soldsleft-=P_ASOLD;
		P_ATYPE=defaultunit(country);
		P_ASTAT=GARRISON;
		P_AMOVE=0;
		P_AXLOC=curntn->capx;
		P_AYLOC=curntn->capy;
		armynum++;

		armysize = max( armysize, soldsleft  / (MAXARM-numleaders-1));

		/* give you your leaders */
		leadtype = getleader(curntn->class);
		P_ATYPE = leadtype-1;	/* This is the national leader */
		P_ASOLD = *(unitminsth+((leadtype-1)%UTYPE));
		P_AXLOC = curntn->capx;
		P_AYLOC = curntn->capy;
		P_ASTAT = DEFEND;
		P_AMOVE = 2*curntn->maxmove;
		armynum++;
		numleaders--;
		while ((armynum < MAXARM)&&(numleaders>0)) {
			P_ATYPE=leadtype;
			P_ASOLD= *(unitminsth+(leadtype%UTYPE));
			P_AXLOC=curntn->capx;
			P_AYLOC=curntn->capy;
			P_ASTAT=DEFEND;
			P_AMOVE=2*curntn->maxmove;
			armynum++;
			numleaders--;
		}

		/* give you the rest of your armies */
		while((armynum < MAXARM)&&(soldsleft >0)) {
			P_ATYPE=defaultunit(country);
			if(soldsleft >= armysize){
				P_ASOLD=armysize;
				soldsleft -=armysize;
			} else {
				P_ASOLD=soldsleft ;
				soldsleft=0;
			}
			P_AXLOC=curntn->capx;
			P_AYLOC=curntn->capy;
			P_ASTAT=DEFEND;
			P_AMOVE=curntn->maxmove;
			armynum++;
		}

		if(soldsleft >0) {
			curntn->arm[0].sold += soldsleft;
			curntn->arm[0].unittyp = A_INFANTRY;
		}

		/* give you some terain to start with: pc nations get more*/
		if ((isnotpc(curntn->active))&&(curntn->location==GREAT)) t=1;
		else if (isnotpc(curntn->active)) t=1;
		else if (curntn->location==OOPS) t=0;
		else if (curntn->location==RANDOM) t=0;
		else if (curntn->location==FAIR) t=1;
		else if (curntn->location==GREAT) t=2;
		else printf("error");
		if( t==1 )
			people = sct[x][y].people / 12;
		else if( t==2 )
			people = sct[x][y].people / 18;

		curntn->tsctrs=1;
		for(i=x-t;i<=x+t;i++) for(j=y-t;j<=y+t;j++)
			if((tofood( &sct[i][j],country)>=DESFOOD)
			&&(sct[i][j].owner==0)
			&&(is_habitable(i,j)==TRUE)
			&&(sct[i][j].people==0)) {
				curntn->tsctrs++;
				sct[i][j].owner=country;
				sct[i][j].designation=DFARM;
				sct[i][j].people=people;
				sct[x][y].people-=people;
			}
	}
	else {
		if(curntn->location==OOPS) printf("ERROR\n");
		else if(curntn->location==RANDOM) {
			printf("RANDOM PLACE FAILED, TRYING TO PLACE AGAIN\n");
			curntn->location=OOPS;
			place(-1,-1);
		}
		else if(curntn->location==FAIR) {
			printf("FAIR PLACE FAILED, TRYING AGAIN - adding %d people to nation\n",NLPOP);
			/*give back one point -> NLPOP people*/
			curntn->tciv+=NLPOP;
			curntn->location=RANDOM;
			place(-1,-1);
		}
		else if(curntn->location==GREAT) {
			printf("GOOD PLACE FAILED, TRYING AGAIN - adding %d people to nation\n",NLPOP);
			/*give back one point -> NLPOP people*/
			curntn->tciv+=NLPOP;
			curntn->location=FAIR;
			place(-1,-1);
		}
	}
}

/*get class routine*/
/* return the number of points needed */
int
getclass(race)
{
	short chk=FALSE;
	short tmp;
	while(chk==FALSE){
		printf("what type of nation would you like to be\n");
		if(race!=ORC){
			printf("1. king      (Humans, Dwarves, and Elves)\n");
			printf("2. emperor   (Humans, Dwarves, and Elves)\n");
		}
		if((race!=ORC)&&(race!=DWARF)){
			printf("3. wizard    (Humans and Elves)..................Cost = %d Points\n",2*NLMAGIC);
			printf("\tWizards have WYZARD and SUMMON powers. \n");
		}
		if(race==HUMAN){
			printf("4. theocracy (Humans Only).......................Cost = %d Points\n",NLMAGIC);
			printf("\tTheocracies have RELIGION power. \n");
		}
		if((race==HUMAN)||(race==ORC)||(race==DWARF)){
			printf("5. pirate    (No Elves)..........................Cost = %d Points\n",NLMAGIC);
			printf("\tPirates have SAILOR power\n");
		}
		if((race==ELF)||(race==HUMAN)){
			printf("6. trader    (Humans & Elves Only)...............Cost = %d Points\n",NLMAGIC);
			printf("\tTraders have URBAN power\n");
		}
		if(race==HUMAN) {
			printf("7. warlord   (No Elves)..........................Cost = %d Point\n",((int)(2*NLMAGIC*8/10)));
			printf("\tHuman Warlords get CAPTAIN, and WARLORD powers\n");
		} else if((race==ORC)||(race==DWARF)) {
			printf("7. warlord   (No Elves)..........................Cost = %d Points\n",((int)(3*NLMAGIC*8/10)));
			printf("\tWarlords get WARRIOR, CAPTAIN, and WARLORD powers\n");
		}
		if( race==ORC) {
			printf("8. demon     (Orcs Only).........................Cost = %d Points\n",4*NLMAGIC/3);
			printf("\tDemons have DESTROYER power\n");
			printf("9. dragon    (Orcs Only).........................Cost = %d Points\n",((int)(3*NLMAGIC*7/10)));
			printf("\tDragons have MINOR, AVERAGE, and MAJOR MONSTER powers\n");
			printf("10. shadow    (Orcs Only)........................Cost = %d Points\n",NLMAGIC);
			printf("\tShadows have VOID power\n");
		}
		printf("\tinput:");
		scanf("%hd",&tmp);
		if((tmp>C_NPC)&&(tmp<=C_END)) {
			if((race==HUMAN)&&((tmp<=C_WARLORD)))
				chk=TRUE;
			else if((race==DWARF)&&((tmp<C_WIZARD)||(tmp==C_PIRATE)||(tmp==C_WARLORD)))
				chk=TRUE;
			else if((race==ELF)&&((tmp==C_TRADER)||(tmp<=C_WIZARD)))
				chk=TRUE;
			else if((race==ORC)&&((tmp==C_PIRATE)||(tmp>=C_WARLORD)))
				chk=TRUE;
			else printf("bad input \n\n\n");
		} else {
			printf("\tinvalid input\n\n\n");
			getchar();
		}
	}
	curntn->class=tmp;
	return( doclass( tmp, TRUE ) );
}

int
doclass( tmp, isupd )
short	tmp;
int	isupd;	/* true if update, false if interactive */
{
	short i;
	long x;

	/* determine number of leaders you want */
	if((tmp == C_TRADER) || (tmp <= C_WIZARD))
		numleaders = 5;
	else	numleaders = 7;

	switch(tmp){
	case C_WIZARD:
		curntn->powers |= SUMMON;
		x=SUMMON;
		if( isupd ) CHGMGK;
		curntn->powers |= WYZARD;
		x=WYZARD;
		if( isupd ) CHGMGK;
		return(2*NLMAGIC);
	case C_PRIEST:
		curntn->powers|=RELIGION;
		x=RELIGION;
		if( isupd ) CHGMGK;
		return(NLMAGIC);
	case C_PIRATE:
		curntn->powers|=SAILOR;
		x=SAILOR;
		if( isupd ) CHGMGK;
		return(NLMAGIC);
	case C_TRADER:
		curntn->powers|=URBAN;
		x=URBAN;
		if( isupd ) CHGMGK;
		return(NLMAGIC);
	case C_WARLORD:
		i=0;
		if(magic(country,WARRIOR)!=TRUE){
			curntn->powers|=WARRIOR;
			x=WARRIOR;
			if( isupd ) CHGMGK;
			i++;
		}
		if(magic(country,CAPTAIN)!=TRUE){
			curntn->powers|=CAPTAIN;
			x=CAPTAIN;
			if( isupd ) CHGMGK;
			i++;
		}
		if(magic(country,WARLORD)!=TRUE){
			curntn->powers|=WARLORD;
			x=WARLORD;
			if( isupd ) CHGMGK;
			i++;
		}
		return((int)(i*NLMAGIC*8/10)); /* 20% discount applied */
	case C_DEMON:
		curntn->powers|=DESTROYER;
		x=DESTROYER;
		if( isupd ) CHGMGK;
		return((int)(4*NLMAGIC/3));
	case C_DRAGON:
		curntn->powers|=MI_MONST;
		x=MI_MONST;
		if( isupd ) CHGMGK;
		curntn->powers|=AV_MONST;
		x=AV_MONST;
		if( isupd ) CHGMGK;
		curntn->powers|=MA_MONST;
		x=MA_MONST;
		if( isupd ) CHGMGK;
		return((int)(3*NLMAGIC*7/10));	/* 30% discount applied */
	case C_SHADOW:
		curntn->powers|=THE_VOID;
		x=THE_VOID;
		if( isupd ) CHGMGK;
		return(NLMAGIC);
	default:
		return(0);
	}
}

int
startcost()	/* cant be used for npc nations yet!!! see below */
{
	float	points;	/* points */

	points = ((float)curntn->tciv)/NLPOP;
	points += ((float)curntn->tgold)/NLGOLD;
	points += ((float)curntn->tmil)/NLSOLD;
	if(curntn->race==ORC) {
		points += ((float)curntn->repro)*NLREPCOST/(NLREPRO_ORC);
		points += ((float)curntn->aplus*2)/NLATTACK;
		points += ((float)curntn->dplus*2)/NLDEFENCE;
	} else {
		points += ((float)curntn->aplus)/NLATTACK;
		points += ((float)curntn->dplus)/NLDEFENCE;
		points += ((float)curntn->repro)*NLREPCOST/NLREPRO;
	}
	points += ((float)curntn->maxmove)/NLMOVE;
	if(curntn->location==FAIR)
		points += NLLOCCOST;
	else if(curntn->location==GREAT)
		points += 2*NLLOCCOST;
	/* points+=NLDBLCOST*curntn->tfood/NLHFOOD; */
	points -= (TURN-1) / LATESTART;	/* extra points if you start late */
	if( TURN > 1 )
	printf("point cost for nation %d is %.2f (bonus for latestart is %f)\n",country,points,(float) (TURN-1)/LATESTART);

	points += 1.0;	/* round up */
	return((int) points);
}
