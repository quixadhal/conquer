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

/*Create a world*/

/*DEFINE TEMPORARY VARIABLES FROM MAKEFILE*/
#include <ctype.h>
#include <stdio.h>
#include "header.h"
#include "data.h"

#define HALF 2
#define LAND 3

extern short country;
int area_map[MAXX][MAXY];/*Value Map of Areas*/
int type[MAPX][MAPY];

makeworld()
{
	register int i,j;
	register int x,y;
	int n; /*count used in string searches*/
	float avvalue; /*Average water tvalue of sectors*/
	int number[5]; /*Number of sectors with type=[0-4]*/
	char passwd[12];
	int alloc = NUMAREAS * 2;/*Value Allocated*/
	int place[MAXX][MAXY] ;/*Temporary matrix to see if an area is placed*/
	int X, Y;
	int X1,Y1;
	int X2,Y2;
	int valid;
	int nranges;
	int rnd;
	int tempfd;
	char newstring[40];

	/*abort if datafile currently exists*/
	if(tempfd=open(datafile,0)!=-1) {
		printf("ABORTING: File %s exists\n",datafile);
		printf("\tthis means that a game is in progress. To proceed, you must remove \n");
		printf("\tthe existing data file. This will, of course, destroy that game.\n\n");
		exit(1);
	}
	printf("\n**********************WELCOME TO CONQUER**********************");
	printf("\nThe world will now be created...Your super user login will be 'god'.");
	printf("\nNon player countries will be read from data stored in the nations file"); 
	printf("\n& will have the same password as god (about to be entered). Add player");
	printf("\nnations with the command <conquer -a>.  Have fun!!!\n");
	printf("\nRemember to check the world out before playing to make sure");
	printf("\nno nations are in bad positions (surrounded by water... )");
	printf("******************************************************************\n\n");

	printf("First, we must zero extraneous files from prior games\n");
	sprintf(newstring,"rm %s*\n",exefile);
	printf("\t%s",newstring);
	system(newstring);
	sprintf(newstring,"rm %s*\n",msgfile);
	printf("\t%s",newstring);
	system(newstring);
	sprintf(newstring,"> %s",newsfile);
	printf("\t%s\n",newstring);
	system(newstring);
	printf("OK This has been done, Now to set up a new world\n\n");

	printf("please enter new super user password (remember this!):");
	scanf("%s",passwd);
	getchar();
	printf("please reenter password:");
	scanf("%s",ntn[0].passwd);
	getchar();
	if((strlen(ntn[0].passwd)<2)
	||(strncmp(passwd,ntn[0].passwd,PASSLTH)!=0)) {
		printf("\ninvalid super user password\n");
		exit(1);
	}
	strncpy(ntn[0].passwd,crypt(passwd,SALT),PASSLTH);

	printf("\n\ncreating world\n");
	/*initialize variables */
	avvalue = (((float) PWATER/25.0)); /*Average water tvalue of sectors*/
	for(i=0;i<MAXX;i++) for(j=0;j<MAXY;j++) {
		place[i][j]=0;
		area_map[i][j]=0;
	}
	for(i=0;i<MAPX;i++) for(j=0;j<MAPY;j++) sct[i][j].vegetation=NONE;

	for(i=0;i<5;i++) number[i] = NUMAREAS/5;  /*areas with type=[i]*/
	number[2]=NUMAREAS - 4*number[0]; /*correct for roundoff*/

	/*determine # of area types to areas*/
	for(i=0;i<250;i++) {
		if((avvalue*NUMAREAS)>alloc) {
			/*have not allocated high enough so Allocate more */
			x = rand()%4;   /*0 to 3*/
			if(number[x]>0) {
				number[x] = number[x] - 1;
				number[x+1] = number[x+1] + 1;
				alloc = alloc + 1;
			}
		}
		else	{
			/*have allocated too much  Allocate less */
			x = (rand()%4) +1; /*1 to 4*/
			if(number[x]>0) {
				number[x] = number[x] - 1;
				number[x-1] = number[x-1] + 1;
				alloc = alloc - 1;
			}
		}
	}

	i=0;
	while((number[4]>0)&&(i<500)) {
		i++;
		/*place a full land sector anywhere but on edge*/
		X = ((rand()%(MAXX-2))+1); /*1 to MAXX-2)*/
		Y = ((rand()%(MAXY-2))+1); /*1 to MAXY-2)*/
		if(place[X][Y] == 0) {
			place[X][Y]=1;
			area_map[X][Y]=4;
			number[4]=number[4] - 1;
			/*place surrounding sectors*/
			if(place[X+1][Y] == 0) {
				rnd = rand()%100 + 1; /*1 to 100*/
				if((rnd<25) && (number[4]>0)) {
					area_map[X+1][Y]=4;
					number[4]=number[4]-1;
					place[X+1][Y]=1;
				}
				if(rnd>25 && number[3]>0) {
					area_map[X+1][Y]=3;
					number[3]=number[3]-1;
					place[X+1][Y]=1;
				}
			}
			if(place[X-1][Y] == 0) {
				rnd = rand()%100 + 1 ; /*(1 to 100)*/
				if(rnd<25 && number[4]>0) {
					area_map[X-1][Y]=4;
					number[4]=number[4]-1;
					place[X-1][Y]=1;
				}
				if(rnd>25 && number[3]>0) {
					area_map[X-1][Y]=3;
					number[3]=number[3]-1;
					place[X-1][Y]=1;
				}
			}
			if(place[X][Y+1] == 0) {
				rnd = rand()%100 + 1 ; /*(1 to 100)*/
				if(rnd<25 && number[4]>0) {
					area_map[X][Y+1]=4;
					number[4]=number[4]-1;
					place[X][Y+1]=1;
				}
				if(rnd>25 && number[3]>0) {
					area_map[X][Y+1]=3;
					number[3]=number[3]-1;
					place[X][Y+1]=1;
				}
			}
			if(place[X][Y-1] == 0) {
				rnd = rand()%100 + 1 ; /*(1 to 100)*/
				if(rnd<25 && number[4]>0) {
					area_map[X][Y-1]=4;
					number[4]=number[4]-1;
					place[X][Y-1]=1;
				}
				if(rnd>25 && number[3]>0) {
					area_map[X][Y-1]=3;
					number[3]=number[3]-1;
					place[X][Y-1]=1;
				}
			}
		}
	}

	/* place all other areas*/
	for(X=0;X<MAXX;X++) for(Y=0;Y<MAXY;Y++) {
		while(place[X][Y] == 0) {
			rnd = rand()%5; /*(0 to 4)*/
			if(number[rnd]>0) {
				area_map[X][Y]=rnd;
				number[rnd]=number[rnd]-1;
				place[X][Y]=1;
			}
		}
	}

	/*ALL AREAS PLACED, NOW DETERMINE SECTOR TYPE
 *fill in each area with sectors
 *      1)   water
 *      2)   water with major islands (25% land)
 *      3)   50/50 water/land 
 *      4)   land with major water (75% Land)
 *      5)   land 
 */
	for(Y=0;Y<MAXY;Y++) for(X=0;X<MAXX;X++) {
		/*fill in edges*/
		fill_edge(X,Y);
		/*fill in center*/
		for(i=1;i<7;i++) {
			for(j=1;j<7;j++) switch(area_map[X][Y]){

			case 0:
				if((rand()%100) < 95) type[X*8+i][Y*8+j]=WATER;
				else type[X*8+i][Y*8+j]=HALF;
				break;
			case 1:
				if(rand()%2 == 0) type[X*8+i][Y*8+j]=WATER;
				else type[X*8+i][Y*8+j]=HALF;
				break;
			case 2:
				if (rand()%2 == 0) type[X*8+i][Y*8+j]=WATER;
				else type[X*8+i][Y*8+j]=LAND;
				break;
			case 3:
				if (rand()%2 == 0) type[X*8+i][Y*8+j]=LAND;
				else type[X*8+i][Y*8+j]=HALF;
				break;
			case 4:
				if ((rand()%100) < 95) type[X*8+i][Y*8+j]=LAND;
				else type[X*8+i][Y*8+j]=HALF;
				break;
			default:
				printf("ERROR");
			}
		}
	}

	/*calculate all 50% areas*/
	for(X=0;X<MAPX;X++) for(Y=0;Y<MAPY;Y++) {
		if(type[X][Y] == HALF)
			if(rand()%2 == 0) {
				type[X][Y] = LAND;
			}
			else type[X][Y] = WATER;
	}


	/*Adjust world given sectors as land or sea, place vegetation, designation, 
 and altitude */

	for(i=0;i<MAPX;i++) for(j=0;j<MAPY;j++)
		if(type[i][j]==LAND) sct[i][j].altitude = CLEAR;
		else sct[i][j].altitude = WATER;

	/*place mountain ranges */

	nranges=(rand()%10+3)*avvalue;
	printf("%d mountain ranges to be placed",nranges);

	while(nranges>0) {
		/*Place one range randomly*/
		X1 = rand()%(MAPX-6);
		Y1 = rand()%(MAPY-6);
		if((type[X1][Y1]==LAND)
		&&(type[X1+1][Y1+1]==LAND)
		&&(type[X1+1][Y1]==LAND)
		&&(type[X1][Y1+1]==LAND)
		&&(type[X1+2][Y1+2]==LAND)) {
			/*place second endpoint */
			valid = 0;
			i=0;
			nranges--;
			while((valid==0) && (i<500)) {
				i++;
				X2 = (rand()%7) + X1;
				Y2 = (rand()%7) + Y1;
				if(type[X2][Y2] == LAND) {
					valid = 1;
					/*fill in mountain range*/
					for(x=X1;x<=X2;x++) {
						if(X1<X2) y=((Y2-Y1)*(x-X1)/(X2-X1))+Y1;
						else y=Y1;
						if(type[x][y] == LAND)
							if(rand()%100>80) sct[x][y].altitude=PEAK;
							else sct[x][y].altitude=MOUNTAIN;
						if((y < MAPY - 1) 
						&& type[x][y+1] == LAND) {
							rnd=rand()%100+1;
							if(rnd>90) sct[x][y+1].altitude=PEAK;
							else if(rnd>50) sct[x][y+1].altitude=MOUNTAIN;
							else if(rnd>20) sct[x][y+1].altitude=HILL;
						}
						if((y!=0)
						&& type[x][y-1] == LAND ) {
							rnd=rand()%100+1;
							if(rnd>90) sct[x][y-1].altitude=PEAK;
							else if(rnd>50) sct[x][y-1].altitude=MOUNTAIN;
							else if(rnd>20) sct[x][y-1].altitude=HILL;
						}
						if((y>=2)
						&&(type[x][y-2] == LAND )) {
							rnd=rand()%100+1;
							if(rnd>90) sct[x][y-2].altitude=MOUNTAIN;
							else if(rnd>50) sct[x][y-2].altitude=HILL;
						}
						if((y < MAPY - 2)
						&&(type[x][y+2] == LAND )) {
							rnd=rand()%100+1;
							if(rnd>90) sct[x][y+2].altitude=MOUNTAIN;
							else if(rnd>50) sct[x][y+2].altitude=HILL;
						}
					}
				}
			}
		}
	}

	/*fill in random hills to work out,not to left of to water*/
	for(i=1;i<NUMAREAS*3;i++) {
		x = rand()%(MAPX-1);
		y = rand()%(MAPY-1);
		if((type[x][y]==LAND)&&(type[x+1][y]==LAND))
			sct[x][y].altitude=HILL;
	}

	/*make sure no peak or mountain is next to water*/
	for(y=1;y<MAPY-1;y++) for(x=1;x<MAPX-1;x++)
		if((sct[x][y].altitude==PEAK)
		||(sct[x][y].altitude==MOUNTAIN))
			for(i=0;i<=2;i++) for(j=0;j<=2;j++)
				if(sct[x+i-1][y+j-1].altitude==WATER)
					sct[x][y].altitude=HILL;

	/*FIGURE OUT SECTOR VEGETATION TYPE
 *use sector.altitude, and sector to determine vegetation
 *from water is distance from nearest water
 */

	/*char veg[]="VDW46973JSI~"*/
	/*char veg[]="VDWBLGWFJSI~"*/

	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++)
		if(type[x][y]==LAND)
		{
			sct[x][y].vegetation=(*(veg+3+rand()%5));
			/*if hill then decrement vegetation*/
			if(sct[x][y].altitude==HILL) {
				for(n=3;n<9;n++)
					if(sct[x][y].vegetation==(*(veg+n)))
						sct[x][y].vegetation=(*(veg+n-1));
				if(area_map[x/8][y/8]<=1) sct[x][y].vegetation=VOLCANO;
			}
			else if(sct[x][y].altitude==MOUNTAIN)
				if((rand()%6==4)&&((y>MAPY/2+8)||(y<MAPY/2-8)))
					sct[x][y].vegetation=ICE;
				else sct[x][y].vegetation=(*(veg+2+rand()%3));
			else if(sct[x][y].altitude==PEAK)
				if((rand()%3==0)&&((y>MAPY/2+8)||(y<MAPY/2-8)))
					sct[x][y].vegetation=ICE;
				else sct[x][y].vegetation=VOLCANO;
		}

	/*REWORK POLEAR/EQUATORIAL sector.vegetation*/
	/*Determine which areas are North Pole and Equatorial*/
	/*char veg[]="VDW46973JSI~"*/
	/*char veg[]="VDWBLGWFJSI~"*/
	/*               012345678901*/

	/*polar work*/
	for(x=0;x<MAPX;x++)
	{
		for(y=0;y<6;y++) if(type[x][y]==LAND)
		{
			if(rand()%4 == 0) sct[x][y].vegetation = ICE;
			else for(n=3;n<10;n++)
				if(sct[x][y].vegetation==(*(veg+n)))
					sct[x][y].vegetation=(*(veg+(n-1)));
		}
		for(y=MAPY-7;y<MAPY;y++) if(type[x][y]==LAND)
		{
			if(rand()%4 == 0) sct[x][y].vegetation = ICE;
			else for(n=3;n<10;n++)
				if(sct[x][y].vegetation==(*(veg+n)) )
					sct[x][y].vegetation=(*(veg+(n-1)));
		}
	}

	/*insert equator*/
	for(y=(MAPY/2)-8;y<=(MAPY/2)+8;y++) for(x=0;x<MAPX;x++)
		if(type[x][y]==LAND)
			if(rand()%10 ==0) sct[x][y].vegetation=DESERT;
			/*increment vegetation if between Waste and Jungle*/
			else for(n=2;n<9;n++)
				if((sct[x][y].vegetation==(*(veg+n)))
				&&(sct[x][y].altitude==CLEAR)
				&&(rand()%4==0)) 
				sct[x][y].vegetation=(*(veg+(n+1)));

	/*char veg[]="VDW46973JSI~"*/
	/*char veg[]="VDWBLGWFJSI~"*/
	/*               012345678901*/

	for(y=((MAPY/2)-2);y<=((MAPY/2)+2);y++) for(x=0;x<MAPX;x++)
		if((type[x][y]==LAND)&&(sct[x][y].altitude==CLEAR))
			if(rand()%10 == 0) sct[x][y].vegetation=DESERT;
			else if(rand()%10 == 0) sct[x][y].vegetation=JUNGLE;
			else if(rand()%10 == 0) sct[x][y].vegetation=SWAMP;
			/*increment vegetation again, but only Waste to Light*/
			else for(n=2;n<4;n++)
				if(sct[x][y].vegetation==(*(veg+n)))
					sct[x][y].vegetation=(*(veg+(n+1)));

	/*expand swamps*/
	for(y=2;y<MAPY;y++) for(x=2;x<MAPX;x++)
		if(sct[x][y].vegetation==SWAMP)
			for(i=0;i<2;i++) for(j=0;j<2;j++)
				if((type[x-i][y-j]==LAND)&&((rand()%3)==0))
					sct[x-i][y-j].vegetation=SWAMP;
	/*expand deserts*/
	for(y=2;y<MAPY;y++) for(x=2;x<MAPX;x++)
		if(sct[x][y].vegetation==DESERT)
			for(i=0;i<2;i++) for(j=0;j<2;j++)
				if((type[x-i][y-j]==LAND)&&((rand()%3)==0))
					sct[x-i][y-j].vegetation=DESERT;

	/*change all volcanos to peaks */
	for(y=1;y<MAPY;y++) for(x=1;x<MAPX;x++)
		if(sct[x][y].vegetation==VOLCANO) sct[x][y].altitude=PEAK;

	/*make sure no desert is next to water*/
	for(y=1;y<MAPY-1;y++) for(x=1;x<MAPX-1;x++)
		if(sct[x][y].vegetation==DESERT)
			for(i=0;i<=2;i++) for(j=0;j<=2;j++)
				if(sct[x+i-1][y+j-1].altitude==WATER)
					sct[x][y].vegetation=LT_VEG;

	/*PLACE EACH SECTOR'S RAW MATERIALS */

	printf("\nplacing raw materials\n");
	for(y=0;y<MAPY;y++) for(x=0;x<MAPX;x++) {

		/*gold*/
		if(rand()%10==0) sct[x][y].gold=rand()%4+4;
		else if(rand()%4==0) sct[x][y].gold=rand()%4;
		else sct[x][y].gold=0;

		/*iron*/
		if(rand()%10==0) sct[x][y].iron=rand()%4+4;
		else if(rand()%4==0) sct[x][y].iron=rand()%4;
		else sct[x][y].iron=0;

		/*default designations*/
		sct[x][y].designation=sct[x][y].vegetation;

		/*default owner is unowned*/
		sct[x][y].owner=0;
	}
	populate();
	close(tempfd);
	writedata();
}

/*fill: subroutine to fill in a square edges with land or sea*/
fill_edge(AX,AY)
{
/*      1)   water
 *      2)   water with major islands (25% land)
 *      3)   50/50 water/land 
 *      4)   land with major water (75% Land)
 *      5)   land 
 */
	register int i;
	int edgearea, X0, Y0, X1, Y1, X2, Y2, X3, Y3, X4, Y4;
	int area;

	X0=AX;
	Y0=AY;
	X1=X0-1;
	Y1=Y0;
	X2=X0+1;
	Y2=Y0;
	X3=X0;
	Y3=Y0-1;
	X4=X0;
	Y4=Y0+1;

	/*NORMALIZE FOR EDGE OF WORLD*/
  	if( X1 < 0 ) X1 = MAXX - 1;
  	if( X2 >= MAXX ) X2 = 0;
  	if( Y3 < 0 ) Y3 = MAXY - 1;
  	if( Y4 >= MAXY ) Y4 = 0;

	area=area_map[X0][Y0];
	/*fill in south*/
	edgearea=area_map[X4][Y4];
	if(area + edgearea > 6) for(i=0;i<8;i++) type[X0*8+i][Y0*8+7] = LAND;
	else if((area + edgearea)>3) {
		for(i=0;i<8;i++)
			if(rand()%2 == 0) type[(X0*8)+i][Y0*8+7] = LAND;
			else type[(X0*8)+i][Y0*8+7] = WATER;
	}
	else for(i=0;i<8;i++) type[(X0*8)+i][Y0*8+7] = WATER;
	/*fill in east*/
	edgearea=area_map[X2][Y2];
	if(area + edgearea > 6) for(i=0;i<8;i++) type[X0*8+7][Y0*8+i] = LAND;
	else if((area + edgearea)>3) {
		for(i=0;i<8;i++)
			if(rand()%2==0) type[X0*8+7][Y0*8+i] = LAND;
			else type[X0*8+7][Y0*8+i] = WATER;
	}
	else for(i=0;i<8;i++) type[X0*8+7][Y0*8+i] = WATER;
	/*fill in west*/
	edgearea=area_map[X1][Y1];
	if(area + edgearea > 6) for(i=0;i<=7;i++) type[X0*8][Y0*8+i] = LAND;
	else if((area + edgearea)>3) {
		for(i=0;i<8 ;i++)
			if(rand()%2==0) type[X0*8][Y0*8+i] = LAND;
			else type[X0*8][Y0*8+i] = WATER;
	}
	else for(i=0;i<8 ;i++) type[X0*8][Y0*8+i] = WATER;
	/*fill in north*/
	edgearea=area_map[X3][Y3];
	if(area + edgearea > 6) for(i=0;i<8 ;i++) type[X0*8+i][Y0*8] = LAND;
	else if((area + edgearea)>3) {
		for(i=0;i<8 ;i++)
			if(rand()%2==0) type[(X0*8)+i][Y0*8] = LAND;
			else type[(X0*8)+i][Y0*8] = WATER;
	}
	else for(i=0;i<8 ;i++) type[(X0*8)+i][Y0*8] = WATER;
}

/* ALLOCATE POPULATIONS OF THE WORLD*/
populate()
{
	int i=0,x=0,y=0,j=0;
	int nvynum=0,army2num=0,armynum=0;
	int temp;
	int cnum=0;
	FILE *fp, *fopen();
	int done=0;
	char line[80];


	/*randomly scatter lizard city (want in DESERTS/swamp/Ice) */
	/*don't reproduce. Their cities are fortified and stockpiled */
#ifdef LZARD 
	strncpy(ntn[NLIZARD].name,"lizard",10);
	strncpy(ntn[NLIZARD].leader,"dragon",10);
	strcpy(ntn[NLIZARD].passwd,ntn[0].passwd);
	ntn[NLIZARD].race=LIZARD;
	ntn[NLIZARD].mark='*';
	ntn[NLIZARD].active=2;
	ntn[NLIZARD].aplus=0;
	ntn[NLIZARD].dplus=0;
	ntn[NLIZARD].maxmove=0;
	ntn[NLIZARD].repro=0;
	ntn[NLIZARD].powers=KNOWALL;
	for(i=0;i<NTOTAL;i++) if(i!=NLIZARD) {
		ntn[NLIZARD].dstatus[i]=WAR;
		ntn[i].dstatus[NLIZARD]=WAR;
	}

	armynum=0;
	country=NLIZARD;
	while(armynum<MAXARM-2){
		x = (rand()%MAPX);
		y = (rand()%MAPY);
		if (is_habitable(x,y)) {
			sct[x][y].designation = DCASTLE;
			sct[x][y].fortress = 5+rand()%5;
			sct[x][y].gold = 15+rand()%20;
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(i>=0&&j>=0&&i<MAPX&&j<MAPX)
					if(sct[i][j].altitude!=WATER)
						sct[i][j].owner = NLIZARD;
			AMOVE=0;
			AXLOC=x;
			AYLOC=y;
			ASTAT=GARRISON;
			ASOLD=750+100*rand()%10;
			armynum++;
			AMOVE=8;
			AXLOC=x;
			AYLOC=y;
			ASTAT=ATTACK;
			ASOLD=750+100*rand()%10;
			armynum++;
		}
	}
	printf("%d lizards placed\n",armynum/2);
#endif

	/* Place Brigands, Barbarians, and Nomads*/
#ifdef MONSTER 
	armynum=0;
	army2num=0;
	strcpy(ntn[NBARBARIAN].name,"bbarian");
	strcpy(ntn[NBARBARIAN].leader,"shaman");
	strcpy(ntn[NBARBARIAN].passwd,ntn[0].passwd);
	ntn[NBARBARIAN].race=BARBARIAN;
	ntn[NBARBARIAN].powers=KNOWALL;
	ntn[NBARBARIAN].mark='*';
	ntn[NBARBARIAN].active=2;
	ntn[NBARBARIAN].aplus=0;
	ntn[NBARBARIAN].dplus=10;
	ntn[NBARBARIAN].maxmove=4;
	ntn[NBARBARIAN].repro=5;
	for(i=0;i<NTOTAL;i++) if(i!=NBARBARIAN) {
		ntn[NBARBARIAN].dstatus[i]=WAR;
		ntn[i].dstatus[NBARBARIAN]=WAR;
	}
	ntn[NBARBARIAN].dstatus[NBARBARIAN]=NEUTRAL;

	strcpy(ntn[NNOMAD].name,"nomad");
	strcpy(ntn[NNOMAD].leader,"khan");
	strcpy(ntn[NNOMAD].passwd,ntn[0].passwd);
	ntn[NNOMAD].powers=KNOWALL;
	ntn[NNOMAD].race=NOMAD;
	ntn[NNOMAD].mark='*';
	ntn[NNOMAD].active=2;
	ntn[NNOMAD].aplus=10;
	ntn[NNOMAD].dplus=0;
	ntn[NNOMAD].maxmove=4;
	ntn[NNOMAD].repro=5;
	for(i=0;i<NTOTAL;i++) if(i!=NNOMAD) {
		ntn[NNOMAD].dstatus[i]=WAR;
		ntn[i].dstatus[NNOMAD]=WAR;
	}
	ntn[NNOMAD].dstatus[NNOMAD]=NEUTRAL;

	strcpy(ntn[NPIRATE].name,"pirate");
	strcpy(ntn[NPIRATE].leader,"captain");
	strcpy(ntn[NPIRATE].passwd,ntn[0].passwd);
	ntn[NPIRATE].powers=KNOWALL;
	ntn[NPIRATE].race=PIRATE;
	ntn[NPIRATE].mark='*';
	ntn[NPIRATE].active=2;
	ntn[NPIRATE].aplus=20;
	ntn[NPIRATE].dplus=0;
	ntn[NPIRATE].maxmove=4;
	ntn[NPIRATE].repro=5;
	for(i=0;i<NTOTAL;i++) if(i!=NPIRATE) {
		ntn[NPIRATE].dstatus[i]=WAR;
		ntn[i].dstatus[NPIRATE]=WAR;
	}
	ntn[NPIRATE].dstatus[NPIRATE]=NEUTRAL;

	temp=(rand()%10+1)*(rand()%10+1);
	for(i=0;i<temp;i++){
		/*50% chance will be on edge, else anywhere*/
		if(rand()%2==0){
			if(rand()%2==0) {
				x=(rand()%20);
				y=(rand()%20);
			}
			else {
				x=(MAPX-(rand()%20)-1);
				y=(MAPY-(rand()%20)-1);
			}
			if(rand()%2==0) x = rand()%MAPX;
			else y = rand()%MAPY;
		}
		else {
			x = rand()%MAPX;
			y = rand()%MAPY;
		}

		/* now place people*/
		if (is_habitable(x,y)) {
			if(rand()%2==0) {
				sct[x][y].owner = NBARBARIAN;
				country=NBARBARIAN;
				AXLOC=x;
				AYLOC=y;
				ASTAT=ATTACK;
				ASOLD=200+100*rand()%10;
				if(armynum<MAXARM-1) armynum++;
			}
			else {
				sct[x][y].owner = NNOMAD;
				ntn[NNOMAD].arm[army2num].xloc=x;
				ntn[NNOMAD].arm[army2num].yloc=y;
				ntn[NNOMAD].arm[army2num].stat=ATTACK;
				ntn[NNOMAD].arm[army2num].sold=100+100*rand()%15;
				if(army2num<MAXARM-1) army2num++;
			}
		}
		else if(nvynum<MAXNAVY) {
			country=NPIRATE;
			NXLOC=x;
			NYLOC=y;
			NWAR=2*(1+rand()%10);
			nvynum++;
		}
	}
	printf("all random population placed\n");
#endif

	/*set up god but dont place*/
	strcpy(ntn[0].name,"unowned");
	strcpy(ntn[0].leader,"god");
	ntn[cnum].race=GOD;
	ntn[cnum].mark='-';
	ntn[cnum].active=0;
	ntn[cnum].location=GOD;
	ntn[cnum].powers=KNOWALL;
	cnum++;

	if ((fp=fopen(helpfile,"r"))==NULL) {
		printf("\terror on read of %s file\n",helpfile);
		printf("\tdo you wish to use default help file (y or n)?");
		if(getchar()=='y'){
		sprintf(line,"cp %s/%s %s",DEFAULTDIR,helpfile,helpfile);
		printf("\n%s\n",line);
		system(line);
		} else {
			printf("\nOK; no NPC nations used\n");
			return;
		}
		getchar();
	}

#ifdef NPC

	/*open npcsfile file*/
	if ((fp=fopen(npcsfile,"r"))==NULL) {
		printf("error on read of %s file\n",npcsfile);
		printf("do you wish to use default NPC nations file (y or n)?");
		if(getchar()=='y'){
		sprintf(line,"%s/%s",DEFAULTDIR,npcsfile);
		if ((fp=fopen(line,"r"))==NULL) {
			printf("\nsorry; error on read of %s file\n",line);
			return;
		} else printf("\nOK; default nations used\n");
		} else {
			printf("\nOK; no NPC nations used\n");
			return;
		}
	}
	else printf("reading npc nation data from file: %s\n",npcsfile);

	/*set up npc nation*/
	if(fgets(line,80,fp)==NULL) done=1;
  	while(done==0) {
  		/*read and parse a new line*/
  		if(line[0]!='#') {
  			sscanf(line,"%s %s %c %c %c %hd %hd %hd %ld %ld %ld %hd %hd",
 			ntn[cnum].name,ntn[cnum].leader,&ntn[cnum].race,
  			&ntn[cnum].mark,&ntn[cnum].location,&ntn[cnum].aplus,
  			&ntn[cnum].dplus,&ntn[cnum].maxmove,&ntn[cnum].tgold,
  			&ntn[cnum].tmil,&ntn[cnum].tciv,&ntn[cnum].repro,
  			&ntn[cnum].active);
  			ntn[cnum].active++;
  			ntn[cnum].class=0;
  			strcpy(ntn[cnum].passwd,ntn[0].passwd);
			country=cnum;
			if(ntn[country].race==HUMAN){
				ntn[country].powers=WARRIOR;
				exenewmgk(WARRIOR);
			}
			else if(ntn[country].race==DWARF){
				ntn[country].powers=MINER;
				exenewmgk(MINER);
			}
			else if(ntn[country].race==ELF){
				ntn[country].powers=THE_VOID;
				exenewmgk(THE_VOID);
			}
			else if(ntn[country].race==ORC){
				ntn[country].powers=MI_MONST;
				exenewmgk(MI_MONST);
			}
			else ntn[country].powers=WARRIOR;
			ntn[country].tfood=24000L;
			ntn[country].tiron=10000L;
			ntn[country].jewels=10000L;
			printf("\tnation %d: %s",cnum,line);
			cnum++;
			place();
		}
		if(fgets(line,80,fp)==NULL) done=1;
	}
	printf("all npc nations placed\n");
#endif
}
