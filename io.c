/*io.c*/
/*Print and io subroutines for game*/

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
#include "patchlevel.h"
#include <ctype.h>

extern FILE *fexe;
/*offset of upper left hand corner*/
extern short xoffset, yoffset;
/*current cursor postion (relative to 00 in upper corner)*/
/*	position is 2*x,y*/
extern short xcurs,ycurs;
extern short redraw; 		/*redraw map in this turn if redraw is a 1*/
extern short hilmode,dismode;			/*display state*/
extern short country;			/* nation id of owner*/

/************************************************************************/
/*	GETSPACE() - malloc all space needed	 			*/
/************************************************************************/
void
getspace()
{
	sct = (struct s_sector **) m2alloc(MAPX,MAPY,sizeof(struct s_sector));
	occ = (char **) m2alloc(MAPX,MAPY,sizeof(char));
	movecost = (short **) m2alloc(MAPX,MAPY,sizeof(short));
}

#ifdef ADMIN
/************************************************************************/
/*	PRINTELE() - print a sector.altitude map 			*/
/************************************************************************/
void
printele()
{
	register int X, Y;
	printf("doing print of altitude\n");
	for(Y=0;Y<MAPY;Y++) {
		for(X=0;X<MAPX;X++) putc(sct[X][Y].altitude,stdout);
		putc('\n',stdout);
	}
}
#endif ADMIN
#ifdef ADMIN
/************************************************************************/
/*	PR_NTNS() - print nation marks					*/
/************************************************************************/
void
pr_ntns()
{
	register int X, Y;
	printf("doing print of nations\n");
	for(Y=0;Y<MAPY;Y++) {
		for(X=0;X<MAPX;X++) {
			if(sct[X][Y].owner==0)
				putc(sct[X][Y].altitude,stderr);
			else putc(ntn[sct[X][Y].owner].mark,stderr);
		}
		putc('\n',stderr);
	}
}
#endif ADMIN

/************************************************************************/
/*	WRITEDATA() - write data to datafile 				*/
/*	trashes/creates datafile in the process				*/
/************************************************************************/
void
writedata()
{
	long	bytes;
	int	fd;

	printf("\ndoing write of data\n");
	if((fd = creat(datafile,0666))==-1) {
		printf("cant open data.  check permissions\n");
		abrt();
	}

/* write world structure */
	if((bytes=write(fd,&world,sizeof(struct s_world)))!=sizeof(struct s_world))
	{
		printf("error writing world data\n");
		printf("wrong data format (%ld vs. %d)\n",bytes,sizeof(struct s_world) );
		abrt();
	}

	if((bytes=write(fd,*sct,MAPX*MAPY*sizeof(struct s_sector))) == -1)
	{
		printf("Wrong number of bytes (%ld) written for sct (should be %d)\n",bytes,MAPX*MAPY*sizeof(struct s_sector));
		abrt();
	};
	printf("writing %ld bytes of sector data\n",bytes);
	if((bytes=write(fd,ntn,NTOTAL*sizeof(struct s_nation))) == -1)
	{
		printf("Wrong number of bytes (%ld) written for ntn (should be %d)\n",bytes,NTOTAL*sizeof(struct s_nation));
		abrt();
	}
	printf("writing %ld bytes of nation data\n",bytes);
	close(fd);
}

/************************************************************************/
/*	READDATA()	-	read data & malloc space		*/
/************************************************************************/
void
readdata()
{
	int fd;
	int n_read;

	/*read in existing nation army and navy data*/
	/*check if file openable*/
	printf("reading data file\n");
	if( (fd = open(datafile,0)) < 0 ) {
		fprintf( stderr, "can not open %s \n", datafile );
		fprintf( stderr, "for help with conquer, type conquer -h\n");
		exit(FAIL);
	}

/* read world structure */
	if((n_read=read(fd,&world,sizeof(struct s_world)))!=sizeof(struct s_world))
	{
		printf("error reading world data\n");
		printf("wrong data format (%d vs. %d)\n",n_read, sizeof(struct s_world) );
		abrt();
	}
#ifdef DEBUG
	printf("reading %d bytes of world data\n",sizeof(struct s_world));
#endif DEBUG

	getspace();

	if((n_read=read(fd,*sct,MAPX*MAPY*sizeof(struct s_sector)))==0)
		printf("EOF\n");
	else if(n_read==-1) printf("error reading sector data (sct)\n");
	if(n_read != (MAPX*MAPY*sizeof(struct s_sector))) {
		printf("error reading sector data (sct)\n");
		printf( "wrong data format (%d vs. %d)\n",n_read,  MAPX*MAPY*sizeof(struct s_sector) );
		abrt();
	}
#ifdef DEBUG
	printf("reading %d bytes of sector data\n",n_read);
#endif DEBUG
	if((n_read=read(fd,ntn,NTOTAL*sizeof(struct s_nation))) == -1)
		printf("error reading s_nation data (ntn)\n");
	else if(n_read!= NTOTAL*sizeof(struct s_nation)) {
		printf("error reading s_nation data (ntn)\n");
		printf( "wrong data format (%d vs. %d)\n",n_read, NTOTAL*sizeof(struct s_nation) );
		abrt();
	}
#ifdef DEBUG
	printf("reading %d bytes of nation data\n",n_read);
#endif DEBUG
	close(fd);
} /* readdata() */

#ifdef ADMIN
/************************************************************************/
/*	PRINTVEG() -	print a vegetation map subroutine		*/
/************************************************************************/
void
printveg()
{
	register int X, Y;
	printf("doing print of vegetation\n");
	for(Y=0;Y<MAPY;Y++) {
		for(X=0;X<MAPX;X++) putc(sct[X][Y].vegetation,stderr);
		putc('\n',stderr);
	}
}
#endif ADMIN
#ifdef CONQUER
/************************************************************************/
/*	OFFMAP()	deal if cursor is off the map			*/
/************************************************************************/
void
offmap()
{
	/*set offset offsets can not be < 0*/
	if(xcurs<1){
		if(XREAL<=0) {
			xoffset=0;
			xcurs=0;
		}
		else {
			redraw=TRUE;
			xoffset-=15;
			xcurs+=15;
		}
	}
	else if(xcurs >= (COLS-23)/2){
		if(XREAL<MAPX) {
			redraw=TRUE;
			xoffset+=15;
			xcurs-=15;
		}
	}
	if(XREAL>=MAPX) xcurs=MAPX-1-xoffset;
	if(xoffset<0) {
		xcurs += xoffset;
		xoffset=0;
	}
	if(xcurs<0) {
		xoffset += xcurs;
		xcurs=0;
	}
	else if(xcurs >= (COLS-23)/2) {
		redraw=TRUE;
		xoffset+=15;
		xcurs-=15;
	}

	if(ycurs<1){
		if(YREAL<=0) {
			yoffset=0;
			ycurs=0;
		}
		else {
			redraw=TRUE;
			ycurs+=15;
			yoffset-=15;
		}
	}
	else if(ycurs >= SCREEN_Y_SIZE-1){
		if(YREAL<MAPY) {
			redraw=TRUE;
			yoffset+=15;
			ycurs-=15;
		}
	}
	if(YREAL>=MAPY) ycurs=MAPY-1-yoffset;
	if(yoffset<0) {
		ycurs += yoffset;
		yoffset=0;
	}
	if(ycurs<0) {
		yoffset += ycurs;
		ycurs=0;
	}
	else if(ycurs >= SCREEN_Y_SIZE-1) {
		redraw=TRUE;
		yoffset+=15;
		ycurs-=15;
	}
	whatcansee();
}
#endif CONQUER
#ifdef CONQUER
/************************************************************************/
/*	PRINTSCORE()	- like it says					*/
/************************************************************************/
void
printscore()
{
	int i;
	int nationid; 	/*current nation id */

	printf("Conquer %s.%d: %s of Year %d, Turn %d\n",VERSION,PATCHLEVEL,
		PSEASON(TURN),YEAR(TURN), TURN);
	printf("id      name   race    class    align  score    talons military  civilians sect\n");
	for (nationid=1; nationid<NTOTAL; nationid++) {
		if(!isntn(ntn[nationid].active)) continue;
		printf("%2d ",nationid);
		printf("%9s ",ntn[nationid].name);
		for(i=1;i<8;i++)
			if(ntn[nationid].race==*(races+i)[0])
				printf("%6s ",*(races+i));
		printf("%8s ",*(Class+ntn[nationid].class));
 		printf(" %7s ",allignment[npctype(ntn[nationid].active)]);
   		printf("%6ld  %8ld %8ld   %8ld %4d\n",
			ntn[nationid].score ,ntn[nationid].tgold
			,ntn[nationid].tmil ,ntn[nationid].tciv
			,ntn[nationid].tsctrs );
	}
}
#endif CONQUER

/************************************************************************/
/*	FLEE() - civilains in x,y flee from somebody			*/
/*	slaver means 25% of populace stays				*/
/* 	isupd is TRUE if it is update					*/
/************************************************************************/
void
flee(x,y,isupd,slaver)
int x,y,isupd,slaver;
{
	int count=0;	/*count is number of acceptable sectors to go to */
	int svcountry=country;
	int slaves=0;
	int i,j;
	country=sct[x][y].owner;

	if(slaver==TRUE){
		slaves= sct[x][y].people/4;
		sct[x][y].people-=slaves;
	}

	/*flee*/
	sct[x][y].people*=6;
	sct[x][y].people/=10;
	/*check if next to anybody of the sectors owners race*/
	for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++)
		if(ONMAP(i,j)
		&&(ntn[sct[i][j].owner].race==ntn[sct[x][y].owner].race))
			count++;

	if(count>0) {
#ifdef CONQUER
	if(isupd==0) {
		if(slaver==TRUE){
			mvprintw(LINES-2,20,"CIVILIANS ABANDON SECTOR (%d slaves)",slaves);
		}else{
			mvaddstr(LINES-2,20,"CIVILIANS ABANDON SECTOR");
		}
	}
#endif CONQUER
	for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++)
		if(ONMAP(i,j)
		&&(ntn[sct[i][j].owner].race==ntn[sct[x][y].owner].race)) {
			sct[i][j].people += sct[x][y].people / count;
			if(isupd==0) SADJCIV2;
		}
	} else {
		sct[x][y].people /= 2;
		for(i=x-4;i<=x+4;i++) for(j=y-4;j<=y+4;j++)
			if(ONMAP(i,j)
			&&(ntn[sct[i][j].owner].race==ntn[sct[x][y].owner].race))
				count++;
		if(count>0) {
#ifdef CONQUER
		if(isupd==0) mvaddstr(LINES-2,20,"PEOPLE FLEE SECTOR AND HALF DIE");
#endif CONQUER
		for(i=x-4;i<=x+4;i++) for(j=y-4;j<=y+4;j++)
			if(ONMAP(i,j)
			&&(ntn[sct[i][j].owner].race==ntn[sct[x][y].owner].race)) {
				sct[i][j].people += sct[x][y].people / count;
				if(isupd==0) SADJCIV2;
			}
		}
#ifdef CONQUER
		else if(isupd==0) mvaddstr(LINES-2,20,"PEOPLE IN SECTOR DIE");
#endif CONQUER
	}

	sct[x][y].people = slaves;
	if(isupd==0) SADJCIV;
	sct[x][y].fortress=0;
	/*SINFORT;*/
	if(tofood( &sct[XREAL][YREAL],sct[XREAL][YREAL].owner)!=0) {
		DEVASTATE(x,y);
		if(isupd==0) SADJDES2;
	}
	country=svcountry;
}
#ifdef ADMIN
/************************************************************************/
/*	READMAP()	- read a map in from map files 			*/
/*	returns TRUE for success, FALSE for fail			*/
/************************************************************************/
int
readmap()
{
	FILE	*mapfile;
	char	line[128];
	register int x,y;

	/* read in ele.map */
	strcpy(line,scenario);
	strcat(line,".ele");
	if ((mapfile=fopen(line,"r"))==NULL) {
		fprintf(stderr,"error on read of %s file\n",line);
		return(TRUE);
	} else fprintf(stderr,"reading elevation map file from %s\n",line );

	y=0;
	while( TRUE ) {
		if(fgets( line, 128, mapfile )==NULL) break;
		for(x=0;x<MAPX;x++) sct[x][y].altitude = line[x];
		y++;
		if(y>=MAPY) break;
	}
	fprintf(stderr,"done reading %d lines of %d characters\n",y,strlen(line));

	/* read in veg.map */
	strcpy(line,scenario);
	strcat(line,".veg");
	if ((mapfile=fopen(line,"r"))==NULL) {
		fprintf(stderr,"error on read of %s file\n",line);
		return(TRUE);
	} else fprintf(stderr,"reading vegetation map file from %s\n",line );
	y=0;
	while( TRUE ) {
		if(fgets( line, 128, mapfile )==NULL) break;
		for(x=0;x<MAPX;x++) sct[x][y].vegetation = line[x];
		y++;
		if(y>=MAPY) break;
	}
	fprintf(stderr,"done reading %d lines of %d characters\n",y,strlen(line));

	return(TRUE);
}
#endif ADMIN

/*********************************************************************/
/* M2ALLOC() - two dimensional array allocator (because C is stupid) */
/*********************************************************************/
char **m2alloc(nrows, ncols, entrysize)
int	nrows;		/* row dimension */
int	ncols;		/* column dimension */
int	entrysize;	/* # bytes in items to be stored */
{
	char	**baseaddr;
	int	j;
	entrysize *= ncols;
	baseaddr = (char **)
		malloc( (unsigned) (nrows*(sizeof(char *)+entrysize)));

	if( baseaddr == (char **) NULL ) {
		printf("OOPS - cant allocate %d by %d blocks of %d bytes\n",nrows,ncols,entrysize);
		abrt();
	}
	if(nrows>0){
		*baseaddr = (char *) (baseaddr + nrows);
		for(j=1; j<nrows; j++)
			baseaddr[j] = baseaddr[j-1] + entrysize;
	}
	return(baseaddr);
}
