/* conquer : Copyright (c) 1988 by Ed Barlow.
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

int pwater;		/* percent water in world (0-100) */
extern short	country;
extern int	numleaders;
char **area_map;			/*Value Map of Areas*/
char **type;

#define TOHILL(x,y) if( nmountains != 0 ) { \
sct[(x)][(y)].altitude=HILL; \
nmountains--; \
}
#define TOMT(x,y) if( nmountains != 0 ) { \
sct[(x)][(y)].altitude=MOUNTAIN; \
nmountains--; \
}
#define TOPEAK(x,y) if( nmountains != 0 ) { \
sct[(x)][(y)].altitude=PEAK; \
nmountains--; \
}

void
makeworld(rflag)
int	rflag;		/* TRUE if you wish to read in a map from mapfiles */
{
	char passwd[PASSLTH+1],*getpass();
	char newstring[200];
	FILE *fopen();

	/*abort if datafile currently exists*/
	if(access(datafile,00) == 0) {
		printf("ABORTING: File %s exists\n",datafile);
		printf("\tthis means that a game is in progress. To proceed, you must remove \n");
		printf("\tthe existing data file. This will, of course, destroy that game.\n\n");
		exit(FAIL);
	}
printf("\n************************** WELCOME TO CONQUER **************************");
printf("\n*\tThe world will now be created...                               *");
printf("\n*\t                                                               *");
printf("\n*\tYour super user login will be 'god'.                           *");
printf("\n*\t                                                               *");
printf("\n*\tNon player countries will be read from the file 'nations'      *");
printf("\n*\tand will have the same password as god (which you are about to *");
printf("\n*\tenter). Add player nations with <conqrun -a>.  Have fun!!!     *");
printf("\n*\t                                                               *");
printf("\n*\tRemember to check the world out before playing to make sure    *");
printf("\n*\tno nations are in bad positions (surrounded by water... )      *");
printf("\n************************************************************************\n\n");

	printf("First, we must zero extraneous files from prior games\n");
	printf("\tignore any errors this causes\n");
	sprintf(newstring,"rm -f %s* %s* %s* %s* %s %s 2> /dev/null",
		exefile, msgfile, newsfile, isonfile, tradefile, timefile);
	printf("\t%s\n",newstring);
	system(newstring);
	printf("OK This has been done, Now to set up a new world\n\n");

	while(TRUE) {			/* password routine */
		strncpy(passwd,getpass("please enter new conquer super user password (remember this!):"),PASSLTH);
		strncpy(ntn[0].passwd,getpass("please reenter conquer password:"),PASSLTH);
		if((strlen(passwd)<2)
		||(strncmp(ntn[0].passwd,passwd,PASSLTH)!=0)){
			beep();
			printf("\ninvalid super user password\n");
		} else break;
  	}

	strncpy(ntn[0].passwd,crypt(passwd,SALT),PASSLTH);
	while(TRUE) {
		printf("\nplease enter the size of the world\n");

		printf("values should be divisible by 8 & greater than 23\n");
		printf("Enter number of X sectors: ");
		gets( passwd );
		world.mapx = atoi( passwd );
		if(((world.mapx % 8) != 0 ) || (world.mapx<24)){
			printf("ERROR: Invalid value entered\n");
			continue;
		}
		printf("Enter number of Y sectors: ");
		gets( passwd );
		world.mapy = atoi( passwd );
		if(((world.mapy % 8) != 0 ) || (world.mapy<24)){
			printf("ERROR: Invalid value entered\n");
			continue;
		}
		break;
	}

	getspace();	/* malloc space for this world */

	/* get amount of water to have in the world */
	while(TRUE) {
		printf("\nEnter percent water to have in world (0-100): ");
		gets( passwd );
		pwater = (-1);
		pwater = atoi( passwd );
		if((pwater<0) || (pwater>100 )){
			printf("ERROR: Invalid value entered\n");
			continue;
		}
		break;
	}

	if( rflag==FALSE ) createworld();
	else readmap();	/* read map in from mapfiles */
	rawmaterials();
	verifydata(__FILE__,__LINE__);
	writedata();

	/* initialize news file */
	sprintf(newstring,"%s0",newsfile);
	if( (fm=fopen(newstring,"w"))!=(FILE *)NULL ) {
		fprintf(fm,"1\tIMPORTANT WORLD NEWS\n");
		fprintf(fm,"5\tGLOBAL ANNOUNCEMENTS\n");
		fclose(fm);
	} else {
		fprintf(stderr,"error opening news file <%s>\n",newstring);
	}
}

void
createworld()	/* create world */
{
	int	i,j;
	register int x,y;
	int	n;		/*count used in string searches*/
	float	avvalue;	/*Average water tvalue of sectors*/
	int	number[5];	/*Number of sectors with type=[0-4]*/
	int	alloc = NUMAREAS * 2;	/*Value Allocated*/
	char	**tplace;	/*Temp: to see if an area is placed*/
	int	X, Y, chance;
	int	X1,Y1;
	int	X2,Y2;
	int	valid;
	int	rnd;
	long	nmountains;

	tplace = (char **) m2alloc(MAPX,MAPY,sizeof(char));
	area_map = (char **) m2alloc(MAPX,MAPY,sizeof(char));
	type = (char **) m2alloc(MAPX,MAPY,sizeof(char));

	printf("\n\ncreating world\n");
	printf("\tin the beginning, the world was a set of bits ordered in\n");
	printf("\ta random way.  Then the conquer game administrator (hereafter\n");
	printf("\tknown as god) decreed 'conqrun -m'!!!\n");
	printf("\nday 1... and the variables were initialized\n\n");
	/*initialize variables */
	avvalue = (((float) (100-pwater)/25.0)); /*Average water tvalue of sectors*/
	for(i=0;i<MAXX;i++) for(j=0;j<MAXY;j++)
		tplace[i][j] = area_map[i][j] = 0;

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
		if(tplace[X][Y] == 0) {
			tplace[X][Y]=1;
			area_map[X][Y]=4;
			number[4]=number[4] - 1;
			/*place surrounding sectors*/
			if(tplace[X+1][Y] == 0) {
				rnd = rand()%100 + 1; /*1 to 100*/
				if((rnd<25) && (number[4]>0)) {
					area_map[X+1][Y]=4;
					number[4]=number[4]-1;
					tplace[X+1][Y]=1;
				}
				if(rnd>25 && number[3]>0) {
					area_map[X+1][Y]=3;
					number[3]=number[3]-1;
					tplace[X+1][Y]=1;
				}
			}
			if(tplace[X-1][Y] == 0) {
				rnd = rand()%100 + 1 ; /*(1 to 100)*/
				if(rnd<25 && number[4]>0) {
					area_map[X-1][Y]=4;
					number[4]=number[4]-1;
					tplace[X-1][Y]=1;
				}
				if(rnd>25 && number[3]>0) {
					area_map[X-1][Y]=3;
					number[3]=number[3]-1;
					tplace[X-1][Y]=1;
				}
			}
			if(tplace[X][Y+1] == 0) {
				rnd = rand()%100 + 1 ; /*(1 to 100)*/
				if(rnd<25 && number[4]>0) {
					area_map[X][Y+1]=4;
					number[4]=number[4]-1;
					tplace[X][Y+1]=1;
				}
				if(rnd>25 && number[3]>0) {
					area_map[X][Y+1]=3;
					number[3]=number[3]-1;
					tplace[X][Y+1]=1;
				}
			}
			if(tplace[X][Y-1] == 0) {
				rnd = rand()%100 + 1 ; /*(1 to 100)*/
				if(rnd<25 && number[4]>0) {
					area_map[X][Y-1]=4;
					number[4]=number[4]-1;
					tplace[X][Y-1]=1;
				}
				if(rnd>25 && number[3]>0) {
					area_map[X][Y-1]=3;
					number[3]=number[3]-1;
					tplace[X][Y-1]=1;
				}
			}
		}
	}

	/* place all other areas*/
	for(X=0;X<MAXX;X++) for(Y=0;Y<MAXY;Y++) {
		while(tplace[X][Y] == 0) {
			rnd = rand()%5; /*(0 to 4)*/
			if(number[rnd]>0) {
				area_map[X][Y]=rnd;
				number[rnd]=number[rnd]-1;
				tplace[X][Y]=1;
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
				fprintf(stderr,"ERROR\n");
				abrt();
			}
		}
	}

	/*calculate all 50% areas*/
	for(X=0;X<MAPX;X++) for(Y=0;Y<MAPY;Y++) {
		if(type[X][Y] == HALF)
			if(rand()%100 >= (100-pwater)) {
				type[X][Y] = LAND;
			}
			else type[X][Y] = WATER;
	}

	chance=0;
	for(X=0;X<MAPX;X++) for(Y=0;Y<MAPY;Y++)
		if(type[X][Y] == WATER) chance++;
	printf("day 2... and god decreed that water should have %d / %d sectors\n",chance,NUMSECTS);

	/*Newly added code to smooth the world out*/
	for(X=1;X<MAPX-1;X++) for(Y=1;Y<MAPY-1;Y++) {
		chance = 0;
		/*count # of land and sea sides*/
		for(i=X-1;i<=X+1;i++) for(j=Y-1;j<=Y+1;j++)
			if(type[i][j] == LAND) chance++;
		if(rand()%9 < chance) type[X][Y] = LAND;
		else type[X][Y] = WATER;
	}
	chance=0;
	for(X=0;X<MAPX;X++) for(Y=0;Y<MAPY;Y++)
		if(type[X][Y] == WATER) chance++;
	printf("\tbut god was not pleased and smoothed the oceans to %d / %d sectors\n\n",chance,NUMSECTS);

	/*Adjust world given sectors as land or sea, place vegetation,
	designation, and altitude */

	for(i=0;i<MAPX;i++) for(j=0;j<MAPY;j++)
		if(type[i][j]==LAND) sct[i][j].altitude = CLEAR;
		else sct[i][j].altitude = WATER;

	/* place  */
	avvalue	= PMOUNT * (100-pwater);
	avvalue	/= 10000;
	nmountains	= NUMSECTS * avvalue;
	printf("day 3... god created %ld mountains and hills\n",nmountains);

	/* heuristic says that 5 is cutoff number to stop placing ranges */
	/* and 1 third of mountains are placed as random hills		*/
	while(nmountains>5 && nmountains> (NUMSECTS*avvalue)/3) {
		X1 = rand()%(MAPX-8);	/* Place one endpoint of range */
		Y1 = rand()%(MAPY-8);
		X2 = (rand()%8) + X1;	/* Place second endpoint */
		Y2 = (rand()%8) + Y1;

		valid = FALSE;
		for(x=X1;x<=X2;x++){
			if(X1<X2)	y=((Y2-Y1)*(x-X1)/(X2-X1))+Y1;
			else		y=Y1;
			for(j=y-2;j<=y+2;j++) if(j>=0)
			if( type[x][j] != LAND && rand()%2==0 ) {
				valid = TRUE;
				break;
			}
		}
		if( valid==TRUE) continue;

		/*fill in mountain range:
			 	y-2	y-1	y	y+1	y+2
			%peak	0	10	20	10	0
			%mtn	10	40	80	40	10
			%hill	40	30	0	30	40
		*/
		for(x=X1;x<=X2;x++) {
			if(X1<X2)	y=((Y2-Y1)*(x-X1)/(X2-X1))+Y1;
			else		y=Y1;

			if(type[x][y] == LAND){
				if(rand()%100>80) {	TOPEAK(x,y);
				} else {		TOMT(x,y) 
				}
			}

			if((y < MAPY - 1) && type[x][y+1] == LAND) {
				rnd=rand()%100+1;
				if(rnd>90) {		TOPEAK(x,y+1);
				} else if(rnd>50) {	TOMT(x,y+1);
				} else if(rnd>20) {	TOHILL(x,y+1);
				}
			}
			if((y!=0) && type[x][y-1] == LAND ) {
				rnd=rand()%100+1;
				if(rnd>90) {		TOPEAK(x,y-1);
				} else if(rnd>50) {	TOMT(x,y-1);
				} else if(rnd>20) {	TOHILL(x,y-1);
				}
			}
			if((y>=2)
			&&(type[x][y-2] == LAND )) {
				rnd=rand()%100+1;
				if(rnd>90) { 		TOMT(x,y-2);
				} else if(rnd>50){	TOHILL(x,y-2);
				}
			}
			if((y < MAPY - 2)
			&&(type[x][y+2] == LAND )) {
				rnd=rand()%100+1;
				if(rnd>90) {		TOMT(x,y+2);
				} else if(rnd>50) {	TOHILL(x,y+2);
				}
			}
		}
	}

	/*fill in random hills to work out,not to left of to water*/
	while(nmountains>0) {
		x = rand()%(MAPX-1);
		y = rand()%(MAPY-1);
		if((type[x][y]==LAND)&&(type[x+1][y]==LAND))
			TOHILL(x,y);
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

	free(tplace);
	free(type);
	free(area_map);
}

void
rawmaterials() 		 /*PLACE EACH SECTOR'S RAW MATERIALS */
{
	int i,j;
	register int x,y;
	int X1,Y1;
	int valid;
	int nmountains;
	struct	s_sector	*sptr;

	TURN=1;

	nmountains = 10 * (END_NORMAL+1);
	for(i=0;i<=END_NORMAL;i++) nmountains -= ( *(tg_value+i) - '0');

	printf("\nday 4... and god placed the worlds raw materials\n");
	for(y=0;y<MAPY;y++) for(x=0;x<MAPX;x++) {

		sptr = &sct[x][y];

		/*default designations*/
		sptr->designation=DNODESIG;
		sptr->tradegood = TG_none;

		/* default setup of sector */
		sptr->owner = sptr->metal = sptr->jewels = sptr->fortress = 0;
		sptr->people = 0l;

		if( is_habitable(x,y)==FALSE ) continue;

		/* exotic trade goods !!! */
		if(rand()%100 < TRADEPCT) {
			valid = FALSE;
			if((rand()%100 < METALPCT )||sptr->altitude==MOUNTAIN){
				getmetal( sptr );	/* metal */
			} else if((rand()%(100-METALPCT))<JEWELPCT ) {
				getjewel( sptr );	/* jewel */
			} else while( valid == FALSE ) { /* random good */
				j=rand()%nmountains;
				for(i=0;i<=END_NORMAL;i++) {
					j -= (10 - ( *(tg_value+i) - '0'));
					if(j <= 0) break;
				}	/* 'i' now random trade good type */

				/* fish	- next to water */
				if(i==TG_fish){
				for(X1=x-1;X1<=x+1;X1++)for(Y1=y-1;Y1<=y+1;Y1++)
				if((ONMAP(X1,Y1))
				&&(sct[X1][Y1].altitude==WATER)){
					valid=TRUE;
					break;
				}

				if(valid==FALSE) continue;
				}

				/* corn,fruit - should be arable land */
				if(((i==TG_corn)||(i==TG_fruit))
				&&(tofood(sptr,0)<6)) continue;

				/* timber,pine,oak - wood/forest */
				if(((i==TG_timber)||(i==TG_pine)||(i==TG_oak))
				&&(sptr->vegetation!=FOREST)
				&&(sptr->vegetation!=WOOD)) continue;

				valid = TRUE;
				sptr->tradegood = i;
			}
			if(sptr->tradegood == TG_none)
				printf("??? DEBUG -tradegood==NONE\n");
		}
	}

	printf("\nday 5... and god decreed that world would be populated\n");
	printf("\tby all manner of creatures; big ones; little one; fat ones;\n");
	printf("\tskinny ones; orange ones; turquois ones; bright blue ones\n");
	printf("\tWAIT!!! god has suddenly realized that smurfs were taking things\n");
	printf("\ttoo far and stopped creating new ones to place everybody on the map...\n");
	populate();
	MERCMEN = ST_MMEN;
	MERCATT = ST_MATT;
	MERCDEF = ST_MDEF;
	printf("\nday 6... and god, who believed in a two day weekend, took off to\n\tthe local pub to celebrate...\n\n");
	printf("day 7... and god rested (to get rid of that stupid hangover)\n");
	printf("\tand thought about logging in to see what the world looks like\n");
	printf("\tand about telling players to add themselves to the game\n");
	printf("\twith the 'conqrun -a' command.\n");
}

/*fill: subroutine to fill in a square edges with land or sea*/
void
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
void
populate()
{
	int	i=0,x=0,y=0,j=0,xloc,yloc;
	int	nvynum=0,armynum=0,points,shipsize,temp,cnum;
	short	short1,short2;			/*temporary short variables */
	short	class;

	int	loopcnt=0;	
	int	pirarmy=0,barbarmy=0,nomadarmy=0,lizarmy=0; 
	short	npirates=0,nbarbarians=0,nnomads=0,nlizards=0;

	FILE *fp, *fopen();
	char line[80],allign;
	char fname[80];

	/*set up god but dont place*/
	curntn = &ntn[0];
	strcpy(curntn->name,"unowned");
	strcpy(curntn->leader,"god");
	curntn->race=GOD;
	curntn->location=GOD;
	curntn->powers=KNOWALL;	/* so god can see the map */
	curntn->powers|=NINJA;	/* so god can see exact unit numbers */
	curntn->mark='-';

	for( country=1; country<NTOTAL; country++ ) {
		curntn = &ntn[country];
		if( isactive( curntn->active )) {
			printf("THIS SHOULDNT HAPPEN\n");
			continue;
		}
		strcpy(curntn->passwd,ntn[0].passwd);
		curntn->powers=0;
		curntn->repro=0;
		curntn->active=INACTIVE;
		curntn->maxmove=0;
		curntn->mark='-';
	}

	for( country=0; country<NTOTAL; country++ ) {
		for(i=country;i<NTOTAL;i++) {
			ntn[country].dstatus[i]=UNMET;
			ntn[i].dstatus[country]=UNMET;
		}
	}

#ifdef MONSTER
	for( country=NTOTAL-1; country>=NTOTAL-4; country-- ) {
		curntn = &ntn[country];
		if( country==NTOTAL-1 ) {
			strncpy(curntn->name,"lizard",10);
			strncpy(curntn->leader,"dragon",10);
			curntn->active=NPC_LIZARD;
			curntn->race=LIZARD;
		} else if( country==NTOTAL-2 ) {
			strcpy(curntn->name,"savages");
			strcpy(curntn->leader,"shaman");
			curntn->active=NPC_BARBARIAN;
			curntn->race=BARBARIAN;
		} else if( country==NTOTAL-3 ) {
			strcpy(curntn->name,"nomad");
			strcpy(curntn->leader,"khan");
			curntn->active=NPC_NOMAD;
			curntn->race=NOMAD;
		} else if( country==NTOTAL-4 ) {
			strcpy(curntn->name,"pirate");
			strcpy(curntn->leader,"captain");
			curntn->active=NPC_PIRATE;
			curntn->race=PIRATE;
		}

		curntn->aplus=0;
		curntn->dplus=0;
		curntn->powers=KNOWALL;
		curntn->mark='*';
		curntn->maxmove=12;
		curntn->repro=5;

		/* everybody hates monster nations */
		if(ismonst(ntn[country].active)) 
		for(i=1;i<NTOTAL;i++) {
			ntn[country].dstatus[i]=WAR;
			ntn[i].dstatus[country]=WAR;
		}
	}

	/* create one inhabitant per MONSTER sectors */
	temp = (NUMSECTS)/MONSTER;
	while( temp-- >0 ) switch( rand()%4 ) {
		case 0: if(npirates < MAXNAVY )		npirates++;
			break;
		case 1: if(nbarbarians < MAXARM )	nbarbarians++;
			break;
		case 2: if(nnomads < MAXARM )		nnomads++;
			break;
		case 3: if(rand()%3==0) {
				if(nlizards < MAXARM/2 ) nlizards++;
			} else	if(nnomads < MAXARM )	nnomads++;
			break;
	}
	printf("placing %d lizards, %d pirates, %d barbarians, and %d nomads\n",
		nlizards,npirates,nbarbarians,nnomads);

	while((nlizards+npirates+nbarbarians+nnomads > 0 )&&(loopcnt++ <5000)) {
		if( nlizards>0 ) {
			for(country=1;country<NTOTAL;country++)
				if( ntn[country].active == NPC_LIZARD ) break;
			armynum=lizarmy;
		} else	if( nbarbarians>0 ) {	
			for(country=1;country<NTOTAL;country++)
				if( ntn[country].active==NPC_BARBARIAN ) break;
			armynum=barbarmy;
		} else	if( nnomads>0 )	 {	
			for(country=1;country<NTOTAL;country++)
				if( ntn[country].active == NPC_NOMAD ) break;
			armynum=nomadarmy;
		} else	if( npirates>0 ) {
			for(country=1;country<NTOTAL;country++)
				if( ntn[country].active == NPC_PIRATE ) break;
			armynum=pirarmy;
		}

		curntn = &ntn[country];

		/*50% chance will be close to map edge, else anywhere*/
		if((rand()%2==0)&&(curntn->active!=NPC_LIZARD)){
			if(rand()%2==0) {
				x=(rand()%20);
				y=(rand()%20);
			} else {
				x=(MAPX-(rand()%20)-1);
				y=(MAPY-(rand()%20)-1);
			}
			if(rand()%2==0) x = rand()%MAPX;
			else y = rand()%MAPY;
		} else {
			x = rand()%MAPX;
			y = rand()%MAPY;
		}

		if(sct[x][y].owner!=0)	continue;
		if(!is_habitable(x,y))	continue;

		sct[x][y].owner = country;

		/* now place people*/
		switch( curntn->active ) {
		case NPC_LIZARD:
			nlizards--;
			sct[x][y].designation = DFORT;
			sct[x][y].metal = 0;
			sct[x][y].jewels = 8 + (i=rand()%20);
			sct[x][y].tradegood = TG_platinum;
			/* make fortificaton consistant with gold */
			sct[x][y].fortress = 5+i/4;
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if((ONMAP(i,j))&&(sct[i][j].altitude!=WATER))
					sct[i][j].owner = country;
			P_AMOVE=0;
			P_AXLOC=x;
			P_AYLOC=y;
			P_ASTAT=GARRISON;
			P_ASOLD=750+100*(rand()%10);
			P_ATYPE=defaultunit(country);
			armynum++;
			lizarmy++;
			P_AMOVE=8;
			P_AXLOC=x;
			P_AYLOC=y;
			P_ASTAT=ATTACK;
			P_ASOLD=750+100*(rand()%10);
			P_ATYPE=defaultunit(country);
			lizarmy++;
			break;
		case NPC_PIRATE:
			/* pirates must be on islands */
			temp=TRUE;
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++) 
				if((ONMAP(i,j)) && ( x!=i || y!=j)
				&&( sct[i][j].altitude!=WATER)) {
					/* fifty percent chance of failure */
					/* for every non-land sector       */
					if (sct[i][j].owner!=0 || rand()%2==0)
						temp=FALSE;
				}

			if(temp==FALSE) {
				sct[x][y].owner = 0;
				break;
			}

			/* build the island */
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
			if((ONMAP(i,j)) && ( x!=i || y!=j))
			if( sct[i][j].altitude!=WATER ) {
				sct[i][j].altitude=WATER;
				sct[i][j].vegetation=WATER;
				sct[i][j].tradegood=TG_none;
				sct[i][j].jewels=0;
				sct[i][j].metal=0;
			}

			npirates--;
			sct[x][y].designation = DBASECAMP;
			P_AMOVE=8;
			P_AXLOC=x;
			P_AYLOC=y;
			P_ASTAT=ATTACK;
			P_ASOLD=150+100*(rand()%3);
			P_ATYPE=defaultunit(country);
			pirarmy++;
			P_NXLOC=x;
			P_NYLOC=y;
			P_NPEOP=0;
			P_NARMY=MAXARM;
			shipsize = N_LIGHT;
			(void) NADD_WAR( rand()%5+2 );
			shipsize = N_MEDIUM;
			(void) NADD_WAR( rand()%3+1 );
			shipsize = N_HEAVY;
			(void) NADD_WAR( rand()%2 );
			P_NCREW=SHIPCREW;
			nvynum++;
			break;
		case NPC_NOMAD:
			nnomads--;
			P_AXLOC=x;
			P_AYLOC=y;
			P_ASTAT=ATTACK;
			P_ASOLD=100+100*(rand()%8);
			P_ATYPE=defaultunit(country);
			nomadarmy++;
			break;
		case NPC_BARBARIAN:
			nbarbarians--;
			P_AXLOC=x;
			P_AYLOC=y;
			P_ASTAT=ATTACK;
			P_ASOLD=100+100*(rand()%4);
			P_ATYPE=defaultunit(country);
			barbarmy++;
			break;
		}
	}

	/* put random monsters around the world */
	for(country=1;country<NTOTAL;country++) {
		if( ntn[country].active != NPC_BARBARIAN ) continue;
		curntn = &ntn[country];
		armynum=barbarmy;
		while(armynum<MAXARM) {
			x = rand()%MAPX;
			y = rand()%MAPY;
			if (is_habitable(x,y)&&sct[x][y].owner==0) {
				sct[x][y].owner = country;
				if( sct[x][y].jewels==0 ) 
					getjewel( &(sct[x][y]) );
				P_AXLOC= x;
				P_AYLOC= y;
				P_ASTAT= ATTACK;
				P_ATYPE= MINMONSTER + rand()%(MAXMONSTER-MINMONSTER+1);
				P_ASOLD= *(unitminsth+(P_ATYPE%UTYPE));
				P_AMOVE= 10;
				armynum++;
			}
		}
	}

	printf("all random population and monsters placed\n");
#endif MONSTER

	for (i=0;i<MAXHELP;i++) {
	sprintf(fname,"%s/%s%d",DEFAULTDIR,helpfile,i);
	if ((fp=fopen(fname,"r"))==NULL) {
		printf("\tcannot find helpfile <%s>\n",fname);
		printf("\tplease move it to %s\n",DEFAULTDIR);
	}
	}

#ifdef NPC
	printf("\nDo you want NPC nations in this campaign? (y or n)");
	while( ((i=getchar()) != 'y')&&(i != 'n') ) ;
	if( i!='y' ) return;
	if((fp=fopen(npcsfile,"r"))==NULL) {
		printf("error on read of %s file\n",npcsfile);
		printf("Do you wish to use default NPC nations file (y or n)?");
		while( ((i=getchar()) != 'y')&&(i != 'n') ) ;
		if( i=='y'){
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
	printf("reading npc nation data from file: %s\n",npcsfile);
	printf("and adding 1 nation per %d land sectors\n",MONSTER);

	cnum=1;
	while(fgets(line,80,fp)!=NULL) {
		/*read and parse a new line*/
		if(line[0]!='#') {
			xloc = yloc = -1;
			sscanf(line,"%s %s %c %c %c %hd %hd %hd %ld %ld %d %hd %c %d %d %hd",
			ntn[cnum].name,ntn[cnum].leader,&ntn[cnum].race,
			&ntn[cnum].mark,&ntn[cnum].location,&ntn[cnum].aplus,
			&ntn[cnum].dplus,&short1,&ntn[cnum].tgold,
			&ntn[cnum].tmil,&points,&short2,&allign,&xloc,&yloc,
			&class);

			country=cnum;
			curntn = &ntn[country];
			if( cnum > MAPX*MAPY/MONSTER*(100-pwater)/100 ) {
				printf("world too small to add npc nation %d %s\n",cnum,curntn->name);
				continue;
			} 
			if( isactive(ntn[cnum].active) ) {
				printf("Too few nations permitted in world to add npc nation %d %s\n",cnum,curntn->name);
				continue;
			}
			curntn->class = (short)class;
			printf("adding npc nation %s (%s)\n",curntn->name,*(Class+curntn->class));
			curntn->maxmove = short1;
			curntn->repro = short2;
			if( allign == 'G' )
				curntn->active = PC_GOOD;
			else if( allign == 'N' )
				curntn->active = PC_NEUTRAL;
			else if( allign == 'E' )
				curntn->active = PC_EVIL;
			else if( allign == 'g' )
				curntn->active = GOOD_6FREE;
			else if( allign == 'n' )
				curntn->active = NEUTRAL_6FREE;
			else if( allign == 'e' )
				curntn->active = EVIL_6FREE;
			else if( allign == 'i' )
				curntn->active = ISOLATIONIST;
			else {
				printf("invalid nation allignment (%c) line is:\n\t%s\n",allign,line);
				abrt();
			}
			strcpy(curntn->passwd,ntn[0].passwd);

			points -= doclass( class, FALSE );
			points -= startcost();
			if(points < 10 ) {
				printf("ERROR IN NATIONS FILE IN NATION %s\n",ntn[cnum].name);
				printf("nation doesnt have enough points left for 10000 civilians\n");
				abrt();
			}
			curntn->tciv = 1000L * points;

			if(curntn->race==HUMAN){
				curntn->powers|=WARRIOR;
				exenewmgk(WARRIOR);
			} else if(curntn->race==DWARF){
				curntn->powers|=MINER;
				exenewmgk(MINER);
			} else if(curntn->race==ELF){
				curntn->powers|=THE_VOID;
				exenewmgk(THE_VOID);
			} else if(curntn->race==ORC){
				curntn->powers|=MI_MONST;
				exenewmgk(MI_MONST);
			} else {
				curntn->powers|=WARRIOR;
				exenewmgk(WARRIOR);
			}
			curntn->tfood= curntn->tciv * 3;
			curntn->metals=10000L;
			curntn->jewels=10000L;
			if (cnum < NTOTAL) cnum++;
			place(xloc,yloc);
			att_setup(country);	/* nation attributes */
		}
	}
	att_base();	/* get nation attributes */
	printf("all npc nations placed\n");
#endif NPC
}
