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

/*include files*/
#include <ctype.h>
#include "header.h"
#include "data.h"

/*Declarations*/
extern struct s_sector sct[MAPX][MAPY];
extern struct nation ntn[NTOTAL];   /* player nation stats */
extern FILE *fexe;

/*offset of upper left hand corner*/
extern short xoffset;
extern short yoffset;
/*current cursor postion (relative to 00 in upper corner)*/
/*	position is 2*x,y*/
extern short xcurs;
extern short ycurs;
/*redraw map in this turn if redraw is a 1*/
extern short redraw;
/*display state*/
extern short hilmode;
extern short dismode;
/* nation id of owner*/
extern short country;
#ifdef ADMIN
/*print a sector.altitude map subroutine*/
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
void
pr_ntns()
{
	register int X, Y;
	printf("doing print of nations\n");
	for(Y=0;Y<MAPY;Y++) {
		for(X=0;X<MAPX;X++) {
			if(sct[X][Y].owner==0)
				putc(sct[X][Y].altitude,stdout);
			else putc(ntn[sct[X][Y].owner].mark,stdout);
		}
		putc('\n',stdout);
	}
}
#endif ADMIN

/*print all data--trashes/creates datafile in the process*/
void
writedata()
{
	int fd;
	printf("\ndoing write of data\n");
	if((fd = creat(datafile,0666))==-1) {
		printf("cant open data.  check permissions\n");
		return;
	}
	write(fd,sct,sizeof(sct));
	write(fd,ntn,sizeof(ntn));
	close(fd);
}

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
		exit(FAIL);
	}
	if((n_read=read(fd,sct,sizeof(sct)))==0) printf("EOF\n");
	else if(n_read==-1) printf("error reading sector data (sct)\n");
	if(n_read!=sizeof(sct)) {
		printf( "wrong data format (%d vs. %d)\n",n_read, sizeof(sct) );
		abrt();
	}
	if((n_read=read(fd,ntn,sizeof(ntn)))==0) printf("EOF\n");
	else if(n_read==-1) printf("error reading nation data (ntn)\n");
	if(n_read!=sizeof(ntn)) {
		printf( "wrong data format (%d vs. %d)\n",n_read, sizeof(ntn) );
		abrt();
	}
	close(fd);
} /* readdata() */

#ifdef ADMIN
/*print a map subroutine*/
void
printveg()
{
	register int X, Y;
	printf("doing print of vegetation\n");
	for(Y=0;Y<MAPY;Y++) {
		for(X=0;X<MAPX;X++) putc(sct[X][Y].vegetation,stdout);
		putc('\n',stdout);
	}
}
#endif ADMIN
#ifdef CONQUER
void
offmap()
{
	redraw=FALSE;
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
	else if(xcurs >= (COLS-22)/2){
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
	else if(xcurs >= (COLS-22)/2) {
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
	else if(ycurs >= SCREEN_Y_SIZE){
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
	else if(ycurs >= SCREEN_Y_SIZE) {
		redraw=TRUE;
		yoffset+=15;
		ycurs-=15;
	}
}
#endif CONQUER
#ifdef CONQUER
void
printscore()
{
	int i;
	int nationid; 	/*current nation id */

	printf("id      name   race    class  score      gold  military  civilians sectors\n");
	for (nationid=1; nationid<MAXNTN; nationid++) {
		if(ntn[nationid].active==0) continue;
		printf("%2d ",nationid);
		printf("%9s ",ntn[nationid].name);
		for(i=1;i<8;i++)
			if(ntn[nationid].race==*(races+i)[0])
				printf("%6s ",*(races+i));
		if(ntn[nationid].active>=2) printf("     NPC ");
		else printf("%8s ",*(Class+ntn[nationid].class));
  		printf("%6ld  %8ld  %8ld   %8ld   %5d\n",
			ntn[nationid].score ,ntn[nationid].tgold
			,ntn[nationid].tmil ,ntn[nationid].tciv
			,ntn[nationid].tsctrs );
	}
}
#endif CONQUER

void
flee(x,y,z,slaver)
int x,y,z,slaver;
{
	/*count is number of acceptable sectors*/
	int count=0;
	int slaves=0;
	int i,j;

	if(slaver==TRUE){
		slaves= sct[x][y].people/4;
		sct[x][y].people-=slaves;
	}

	/*flee*/
	sct[x][y].people*=6;
	sct[x][y].people/=10;
	/*check if next to anybody of the sectors owners race*/
	for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++)
		if(i>=0&&i<MAPX&&j>=0&&j<MAPY
		&&(ntn[sct[i][j].owner].race==ntn[sct[x][y].owner].race))
			count++;

	if(count>0) {
	if(z==0) if(slaver==TRUE){
			mvprintw(LINES-2,20,"CIVILIANS ABANDON SECTOR (%d slaves)",slaves);
		}else{
			mvaddstr(LINES-2,20,"CIVILIANS ABANDON SECTOR");
		}
	for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++)
		if(i>=0&&i<MAPX&&j>=0&&j<MAPY
		&&(ntn[sct[i][j].owner].race==ntn[sct[x][y].owner].race)) {
			sct[i][j].people += sct[x][y].people / count;
			if(z==0) SADJCIV2;
		}
	}
	else {
	sct[x][y].people /= 2;
	for(i=x-4;i<=x+4;i++) for(j=y-4;j<=y+4;j++)
		if(i>=0&&i<MAPX&&j>=0&&j<MAPY
		&&(ntn[sct[i][j].owner].race==ntn[sct[x][y].owner].race))
			count++;
	if(count>0) {
	if(z==0) mvaddstr(LINES-2,20,"PEOPLE FLEE SECTOR AND HALF DIE");
	for(i=x-4;i<=x+4;i++) for(j=y-4;j<=y+4;j++)
		if(i>=0&&i<MAPX&&j>=0&&j<MAPY
		&&(ntn[sct[i][j].owner].race==ntn[sct[x][y].owner].race)) {
			sct[i][j].people += sct[x][y].people / count;
			if(z==0) SADJCIV2;
		}
	}
	else if(z==0) mvaddstr(LINES-2,20,"PEOPLE IN SECTOR DIE");
	}

	sct[x][y].people = slaves;
	if(z==0) SADJCIV;
	sct[x][y].fortress=0;
	/*SINFORT;*/
	if(tofood(sct[XREAL][YREAL].vegetation,sct[XREAL][YREAL].owner)!=0) {
		sct[x][y].designation=DDEVASTATED;
		if(z==0) SADJDES2;
	}
}

