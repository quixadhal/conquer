/* Conquer: Copyright (c) 1988 by Edward M Barlow */
#include	<ctype.h>
#include	<stdio.h>
#include	"header.h"
#include	"data.h"

extern FILE *fnews;

extern char *HVegcost;
extern char *OVegcost;
extern char *EVegcost;
extern char *DVegcost;
extern char *HElecost;
extern char *OElecost;
extern char *EElecost;
extern char *DElecost;

#ifdef SYSV
char    *memset();
#endif

#ifdef CONQUER
int
move_file( from, to )
register char	*from;
register char	*to;
{
	if( unlink( to ) < 0 ) {
		fprintf( stderr, "unlink( %s ) failed \n", to );
		sleep( 2 );
		return( -1 );
	}

	if( link( from, to ) < 0 ) {
		fprintf( stderr, "link( %s, %s ) failed \n", from, to );
		sleep( 2 );
		return( -1 );
	}

	if( unlink( from ) < 0 ) {
		fprintf( stderr, "unlink( %s ) failed \n", from );
		sleep( 2 );
		return( -1 );
	}

	return( 0 );
} /* move_file() */
#endif CONQUER

int
max( a, b )
register int	a;
register int	b;
{
	return( (a > b) ? a : b );
}

int
min( a, b )
register int	a;
register int	b;
{
	return( (a < b) ? a : b );
}

void
verify_ntn( __file__, __line__ )
char	__file__[];
int	__line__;
{
	register struct nation	*nptr;
	register int		i;
	register int		country;
	struct army	*a;

	for( country = 0; country < NTOTAL; country++ ) {
		nptr = &ntn[country];

		if( nptr->tiron < 0.0 ) {
			fprintf( stderr, "file %s: line %d: nation[%d] iron = %ld\n",
				__file__, __line__, country, nptr->tiron );
			nptr->tiron = 0.0;
		}

		if( nptr->jewels < 0 ) {
			fprintf( stderr, "file %s: line %d: nation[%d] jewels = %ld\n",
				__file__, __line__, country, nptr->jewels );
			nptr->jewels = 0.0;
		}

		for( i = 0; i < MAXARM; i++ ) {
			a = &nptr->arm[i];
			if( a->sold < 0 ) {
				fprintf( stderr, "file %s: line %d: nation[%d] army[%d] sold = %d\n",
					__file__, __line__, country, i, nptr->arm[i].sold );
				nptr->arm[i].sold = 0;
			}
			if( a->xloc < 0 || a->xloc >= MAPX ) {
				fprintf( stderr, "file %s: line %d: nation[%d] army[%d] xlocation = %d\n",
					__file__, __line__, country, i, nptr->arm[i].xloc );
				nptr->arm[i].xloc = 0;
			}
			if( a->yloc < 0 || a->yloc >= MAPY ) {
				fprintf( stderr, "file %s: line %d: nation[%d] army[%d] ylocation = %d\n",
					__file__, __line__, country, i, nptr->arm[i].yloc );
				nptr->arm[i].yloc = 0;
			}
		} /* for */

		for( i = 0; i < NTOTAL; i++ ) {
			if( nptr->dstatus[i] > JIHAD ) {
				fprintf( stderr, "file %s: line %d: nation[%d] diplomatic status with %d = %d\n",
					__file__, __line__, country, i, nptr->dstatus[i] );
				nptr->dstatus[i] = WAR;
			}
			if( nptr->dstatus[i] < UNMET ) {
				fprintf( stderr, "file %s: line %d: nation[%d] diplomatic status with %d = %d\n",
					__file__, __line__, country, i, nptr->dstatus[i] );
				nptr->dstatus[i] = UNMET;
			}
		} /* for */
	} /* for */
} /* verify_ntn() */

void
verify_sct( __file__, __line__ )
char	__file__[];
int	__line__;
{
	register struct s_sector	*sptr;
	register int		x, y;

	for( x = 0; x < MAPX; x++ ) {
		for( y = 0; y < MAPX; y++ ) {
			sptr = &sct[x][y];

			if( sptr->people < 0 ) {
				fprintf( stderr, "file %s: line %d: sct[%d][%d].people = %d\n", __file__, __line__, x, y, sptr->people );
				if( sptr->people < -1*BIG )
					sptr->people = BIG;
				else sptr->people = 0;
			}

			if( sptr->owner != 0 && sptr->altitude == WATER ) {
				fprintf( stderr, "file %s: line %d: sct[%d][%d].owner = %s (a water sector)\n",__file__,__line__, x, y, ntn[sptr->owner].name );
				sptr->owner = 0;
			}
			if( sptr->fortress > 12 ){
				fprintf( stderr, "file %s: line %d: sct[%d][%d].fortress = %d \n",__file__,__line__, x, y, sptr->fortress );
				sptr->fortress = 12;
			}
		} /* for */
	} /* for */
} /* verify_sct() */

void
verifydata( __file__, __line__ )
char	__file__[];
int	__line__;
{
	/* check for invalid values */
	verify_ntn( __file__, __line__ );
	verify_sct( __file__, __line__ );
}/* verifydata() */

int
get_number()
{
	char ch;
	int done=FALSE,count=0,xpos,ypos;
	int sum=0;

	/* this routine totally redone to allow deleting */
	while(!done) {
		ch=getch();
		if (isdigit(ch) && count<12) {
			/* only print numbers to the screen */
			addch(ch);
			refresh();
			sum *= 10L;
			count++;
			sum += (long)(ch-'0');
		}
		else if ((ch=='\b' || ch=='\177')&&(count))
		{
			/* only delete what was printed */
			getyx(stdscr,ypos,xpos);
			move(ypos,--xpos);
			addch(' ');
			move(ypos,xpos);
			refresh();
			sum /= 10L;
			count--;
		}
		else if (ch=='\n') {
			done=TRUE;
		}
	}
	return( sum );
}

#define INFINITE	1000

int		bx;		/* distination 'x' coordinate */
int		by;		/* destination 'y' coordinate */
int		moving_country;	/* country that is moving */

#define MAX_MOVE_UNITS	0x7f
unsigned char	history_reachp[ MAPX ][ MAPY ];

#if 1
int	level;
#endif

/*
 *	land_2reachp()
 */

int
land_2reachp( ax, ay, move_points )
int	ax;
int	ay;
int	move_points;
{
	register int	i = 0;
	int	delta_x, delta_y;
	int	x_abs_delta, y_abs_delta;
	int	own;
	int	dx[ 8 ];
	int	dy[ 8 ];

	delta_x = bx - ax;
	delta_y = by - ay;

	/* Have we got where we are going? */
	if( delta_x == 0 && delta_y == 0 ) {
		return( 1 );
	}

	/* Any move points left? (optimization) */
	if( move_points == 0 ) {
		return( 0 );
	}

	x_abs_delta = (delta_x < 0) ? -delta_x : delta_x;
	y_abs_delta = (delta_y < 0) ? -delta_y : delta_y;

	/* couldn't reach if all moves cost 1 (optimization) */
	if( max( x_abs_delta, y_abs_delta ) > move_points ) {
		return( 0 );
	}

	{
		register int	inc_x;
		register int	inc_y;

		inc_x = (delta_x < 0 ) ? -1 : 1;
		inc_y = (delta_y < 0 ) ? -1 : 1;

		/*I HAVE CHANGED THIS CODE FROM THE ORIGINAL TO OPTIMIZE IT*/
		/*I think it should work well*/
		if( y_abs_delta == 0) {
			/* try 'x' movements first */
			dx[i] = inc_x; dy[i++] = 0;
			dx[i] = inc_x; dy[i++] = inc_y;
			dx[i] = inc_x; dy[i++] = -inc_y;
			dx[i] = 0; dy[i++] = inc_y;
			dx[i] = 0; dy[i++] = -inc_y;
			dx[i] = -inc_x; dy[i++] = inc_y;
			dx[i] = -inc_x; dy[i++] = 0;
			dx[i] = -inc_x; dy[i++] = -inc_y;
		} else if( x_abs_delta == 0 ) {
			/* try 'y' movements first */
			dx[i] = 0; dy[i++] = inc_y;
			dx[i] = inc_x; dy[i++] = inc_y;
			dx[i] = -inc_x; dy[i++] = inc_y;
			dx[i] = inc_x; dy[i++] = 0;
			dx[i] = -inc_x; dy[i++] = 0;
			dx[i] = inc_x; dy[i++] = -inc_y;
			dx[i] = 0; dy[i++] = -inc_y;
			dx[i] = -inc_x; dy[i++] = -inc_y;
		} else {	/* x_abs_delta != 0, 0 != y_abs_delta */
			/* try diagonal movements first */
			dx[i] = inc_x; dy[i++] = inc_y;

			dx[i] = 0; dy[i++] = inc_y;
			dx[i] = inc_x; dy[i++] = 0;

			dx[i] = -inc_x; dy[i++] = inc_y;
			dx[i] = inc_x; dy[i++] = -inc_y;

			dx[i] = -inc_x; dy[i++] = 0;
			dx[i] = 0; dy[i++] = -inc_y;

			dx[i] = -inc_x; dy[i++] = -inc_y;
		} /* if */
	} /* block */

	{
		register int	x, y;
		register int	new_mp;

		for( i = 0; i < 8; i++ ) {
			if( (x = ax + dx[i]) < 0 || x >= MAPX )
				continue;
			if( (y = ay + dy[i]) < 0 || y >= MAPY )
				continue;

			if( sct[x][y].altitude == PEAK)
				continue;
			if( sct[x][y].altitude == WATER)
				continue;

			new_mp = move_points - movecost[ x ][ y ];
			if( new_mp < 0 )
				continue;

			/*
			*	If we have been to this sector before
			*	in fewer move points this path is not
			*	going to do any better.
			*/
			if( history_reachp[x][y] >= new_mp ) {
				continue;
			}
			history_reachp[x][y] = new_mp;

			/*
			*	Test for a hostile army
			*/
			/* BUG: should engage if army is hostile but does not own sector */
			/* BUG: take into account THE_VOID, HIDDEN, and NINJA */
			/* BUG: NEUTRAL does not allow to pass */
			if( (own = sct[x][y].owner) > 0 &&
			ntn[own].dstatus[moving_country] >= WAR &&
			x != bx && y != by &&
			solds_in_sector( x, y, own ) > 0 ) {
				continue;	/* at war with the owner, may not pass */
			}

			level++;
			if( land_2reachp( x, y, new_mp ) ) {
				level--;
				return( 1 );
			} /* if */
			level--;
		} /* for */
	} /* block */
	return( 0 );
} /* land_2reachp() */

/*
 *	land_reachp()
 */
#ifdef ADMIN
int
land_reachp( ax, ay, gx, gy, move_points, movee )
int	ax;
int	ay;
int	gx;
int	gy;
int	move_points;
int	movee;
{
	int	result;

	if( move_points >= MAX_MOVE_UNITS ) {
		fprintf( stderr, "land_reachp(): move_points = %d\n",
			move_points );

		abrt();
	}

	/* Are we starting or ending in the water or on a peak? */
	if( sct[ax][ay].altitude == WATER || sct[ax][ay].altitude == PEAK )
		return( 0 );
	if( sct[gx][gy].altitude == WATER || sct[gx][gy].altitude == PEAK )
		return( 0 );

#ifdef SYSV
	memset( history_reachp, 0, sizeof(history_reachp) );
#else
	bzero((char *) history_reachp,sizeof(history_reachp));
#endif

	history_reachp[ax][ay] = move_points;

	bx = gx;
	by = gy;
	moving_country = movee;

	level = 1;
	result = land_2reachp( ax, ay, move_points );
	return( result );
} /* land_reachp() */
#endif ADMIN
#ifdef ADMIN
/*
 *	water_2reachp()
 */

int
water_2reachp( ax, ay, move_points )
int	ax;
int	ay;
int	move_points;
{
	register int	i = 0;
	int	delta_x;
	int	delta_y;
	int	dx[ 8 ];
	int	dy[ 8 ];

	/* this path uses too many move units */
	if( move_points < 0 )
		return( 0 );

	/*
	*	If we have been to this sector before in fewer move points
	*	this path is not going to do any better.
	*/
	if( history_reachp[ ax ][ ay ] <= move_points )
		return( 0 );

	history_reachp[ ax ][ ay ] = move_points;

	delta_x = ax - bx;
	delta_y = ay - by;

	/* Have we got where we are going? */
	if( delta_x == 0 && delta_y == 0 )
		return( 1 );

	/* Have we run into ground, but not reached our destination? */
	if( sct[ax][ay].altitude != WATER )
		return( 0 );

	/* Any move points left? (optimization) */
	if( move_points == 0 )
		return( 0 );

	/* couldn't reach if all moves cost 1 (optimization) */
	if( max( abs( delta_x ), abs( delta_y ) ) > move_points )
		return( 0 );

	/* BUG: test for an enemy navy */

	{
		register int	inc_x;
		register int	inc_y;

		inc_x = (delta_x < 0 ) ? -1 : (delta_x > 0) ? 1 : 0;
		inc_y = (delta_y < 0 ) ? -1 : (delta_y > 0) ? 1 : 0;

		if( abs(delta_x) > abs(delta_y) ) {
			/* try 'x' movements first */
			dx[i] = inc_x; dy[i++] = 0;
			dx[i] = inc_x; dy[i++] = inc_y;
			dx[i] = inc_x; dy[i++] = -inc_y;
			dx[i] = 0; dy[i++] = inc_y;
			dx[i] = 0; dy[i++] = -inc_y;
			dx[i] = -inc_x; dy[i++] = inc_y;
			dx[i] = -inc_x; dy[i++] = 0;
			dx[i] = -inc_x; dy[i++] = -inc_y;
		} else {	/* abs(delta_x) < abs(delta_y) */
			/* try 'y' movements first */
			dx[i] = 0; dy[i++] = inc_y;
			dx[i] = inc_x; dy[i++] = inc_y;
			dx[i] = -inc_x; dy[i++] = inc_y;
			dx[i] = inc_x; dy[i++] = 0;
			dx[i] = -inc_x; dy[i++] = 0;
			dx[i] = inc_x; dy[i++] = -inc_y;
			dx[i] = 0; dy[i++] = -inc_y;
			dx[i] = -inc_x; dy[i++] = -inc_y;
		} /* if */
	} /* block */

	{
		register int	x, y;
		register int	new_mp;

		for( i = 0; i < 8; i++ ) {
			if( (x = ax + dx[i]) < 0 || x >= MAPX )
				continue;
			if( (y = ay + dy[i]) < 0 || y >= MAPY )
				continue;

			new_mp = move_points - 1;
			if( new_mp < 0 )
				continue;

			if( water_2reachp( x, y, new_mp ) )
				return( 1 );
		} /* for */
	} /* block */

	return( 0 );
} /* water_2reachp() */
#endif ADMIN
#ifdef 0
/*
 *	water_reachp()
 */

int
water_reachp( ax, ay, gx, gy, move_points, movee )
int	ax;
int	ay;
int	gx;
int	gy;
int	move_points;
int	movee;
{
	if( move_points >= MAX_MOVE_UNITS ) {
		fprintf( stderr, "water_reachp(): move_points = %d\n",
			move_points );

		abrt();
	}

#ifdef SYSV
	memset(history_reachp, MAX_MOVE_UNITS, sizeof(history_reachp));
#else
	{ register int i,j;
		for (i=0; i < MAPX ; i++)
		for (j=0; j < MAPY ; j++ )
			history_reachp [i] [j] = MAX_MOVE_UNITS ;
	}/* eof memset replacement block */
#endif

	history_reachp[ ax ][ ay ] = 0;

	bx = gx;
	by = gy;
	moving_country = movee;

	return( water_2reachp( ax, ay, move_points ) );
} /* water_reachp() */
#endif 0

/*
 *	solds_in_sector()
 */

int
solds_in_sector( x, y, country )
int	x;
int	y;
int	country;
{
	register struct nation	*nptr = &ntn[country];
	register int	j;
	register int	total = 0;

	for( j = 0; j < MAXARM; j++ ) {
		if( nptr->arm[j].sold == 0 )
			continue;

		if( nptr->arm[j].xloc == x && nptr->arm[j].yloc == y )
			total += nptr->arm[j].sold;
	}

	return( total );
} /* solds_in_sector() */
#ifdef ADMIN
/*
 *	score_one()
 */

struct wght {
	int	sectors;
	int	civilians;
	int	soldiers;
	int	gold;
	int	jewels;
	int	iron;
	int	magics;
	int	ships;
} weights[] = {
/*		Per 2  1000    1000      100K 100K   100K   Magic  10 */
/*	Races   Sector People  Soldiers  Gold Jewels Iron   Power Ship */
/* NPC */	{ 2,    1,     0,        0,    1,    1,     1,    0 },
/* kingdom */	{ 2,    1,     2,        3,    0,    0,     0,    0 },
/* empire */	{ 3,    0,     0,        0,    0,    0,     0,    0 },
/* wizard */	{ 0,    2,     1,        0,    3,    5,     20,   0 },
/* theocracy */	{ 2,    1,     0,        0,    3,    0,     7,    0 },
/* pirate */	{ 0,    0,     5,        0,    10,   10,    1,    5 },
/* trader */	{ 2,    1,     0,        0,    1,    1,     1,    8 },
/* warlord */	{ 2,    1,     2,        0,    1,    1,     1,    0 },
/* demon */	{ 2,    0,     1,        0,    1,    0,     10,   0 },
/* dragon */	{ 0,    0,     0,        10,   20,   0,     0,    0 },
/* shadow */	{ 2,    0,     0,        0,    0,    5,     0,    0 },
/* miner */	{ 0,    0,    5,    0,    10,   10,   1,    5 },
};

long
score_one( country )
int	country;
{
	register struct nation	*nptr = &ntn[ country ];
	register long		total = 0;
	register struct wght	*wght = &weights[ nptr-> class ];

	total += wght->sectors * nptr->tsctrs / 2;
	total += wght->civilians * nptr->tciv / 1000;
	total += wght->soldiers * nptr->tmil / 1000;
	if(nptr->tgold > 0 ) total += wght->gold * nptr->tgold / 100000;
	total += wght->jewels * nptr->jewels / 100000;
	total += wght->iron * nptr->tiron / 100000;
	total += wght->magics * num_powers(country,M_MIL);
	total += wght->magics * num_powers(country,M_CIV);
	total += wght->magics * num_powers(country,M_MGK);
	total += wght->ships * nptr->tships / 10;

	return( total );
} /* score_one() */
#endif ADMIN
/*
 *	print_accum()
 */

	/* max number of print_accum() calls in one printf() */
#define MAX_BUFFER	4
#define BUFFER_SIZE	20

/*
 *	is_habitable()
 */

int
is_habitable( x, y )
int	x;
int	y;
{
	if( sct[x][y].altitude==WATER ) return( 0 );
	if( sct[x][y].altitude==PEAK ) return( 0 );

	if( tofood( sct[x][y].vegetation,sct[x][y].owner ) < DESFOOD) return(0);

	return( 1 );
}

#ifdef CONQUER
int
units_in_sector(x,y,country)
int	x;
int	y;
{
int count=0;
int armynum, nvynum;
for(armynum=0;armynum<MAXARM;armynum++)
	if((ASOLD>0)&&(AXLOC==x)&&(AYLOC==y)) count++;
for(nvynum=0;nvynum<MAXNAVY;nvynum++)
	if(((NWAR+NMER)!=0)&&(NXLOC==x)&&(NYLOC==y)) count++;
return(count);
}
#endif CONQUER

int
num_powers(country,type)
int country,type;
{
int count_magic=0;
int try;
long start, end;
switch(type){
	case M_MGK:
		start=S_MGK;
		end=E_MGK;
		break;
	case M_CIV:
		start=S_CIV;
		end=E_CIV;
		break;
	case M_MIL:
		start=S_MIL;
		end=E_MIL;
		break;
	default:
		printf("fatal error in num_powers");
		abrt();
}
for( try = start; try < start+end; try++ )
	if( magic(country, powers[try] ) == 1 ) count_magic++;
return(count_magic);
}

/* returns food value of sector */
/* 4 is limit of livable land */
tofood(vegetation,cntry)
int cntry;
register char	vegetation;
{
	int i=0;
	int foodvalue;
#ifdef DERVDESG
	if ((magic(cntry,DERVISH)||magic(cntry,DESTROYER))
	&&(vegetation==DESERT || vegetation==ICE)) return(6);
#endif DERVDESG
	while( vegetation != *(veg+i) ) {
		if( *(veg+i) == '0') break;
		else i++;
	}
	foodvalue = *(vegfood+i);
	if(ntn[cntry].race == ELF){
		if(vegetation == FOREST) foodvalue++;
		else if(vegetation == BARREN) foodvalue--;
	}
	return( foodvalue );
}

/*jewel cost for civilian power = Base * 2**( #mgk/2 + #civ + #mil/2 )	*/
/*race			magical		civilian	military	*/
/*	elves -		50K		50K		50K		*/
/*	dwarves -	80K		40K		40K		*/
/*	humans -	100K		25K		50K		*/
/*	orcs -		100K		50K		25K		*/

/* returns cost of magic power - returns -1 if invalid */
long
getmgkcost(type,country)
int type, country;
{
	int i;
	long cost;
	long base=50000L;
	int npowers;
	switch(type) {
	case M_MGK:
		if(ntn[country].race==DWARF)		base=80000L;
		else if(ntn[country].race==HUMAN)	base=100000L;
		else if(ntn[country].race==ORC)		base=100000L;
		npowers=num_powers(country,M_CIV)+num_powers(country,M_MIL)+1
		+2*num_powers(country,M_MGK);
		npowers/=2;
		break;
	case M_CIV:
		if(ntn[country].race==DWARF)		base=40000L;
		else if(ntn[country].race==HUMAN)	base=25000L;
		npowers=num_powers(country,M_MGK)+num_powers(country,M_MIL)+1
		+2*num_powers(country,M_CIV);
		npowers/=2;
		break;
	case M_MIL:
		if(ntn[country].race==DWARF)		base=40000L;
		else if(ntn[country].race==ORC)		base=25000L;
		npowers=num_powers(country,M_CIV)+num_powers(country,M_MGK)+1
		+2*num_powers(country,M_MIL);
		npowers/=2;
		break;
	default:
		return(-1);
	}
	cost = base;
	for (i=1; i<npowers; i++) {
		cost <<= 1;
		if (cost > BIG)
			return(BIG/2L);
	}
	return(cost);
}

int
todigit(character)
register int	character;
{
	if( character >= '0' && character <= '9' )
		return( character - '0' );
	return( -1 );
}

void
prep(country)
int country;
{
	short armynum,nvynum;
	int save,i,j;

	/*set occ to 0*/
	for(i=0;i<MAPX;i++) for(j=0;j<MAPY;j++) occ[i][j]=0;

	save=country;
	/*set occ to country of occupant army*/
	for(country=0;country<NTOTAL;country++) if(ntn[country].active!=0) {
			for(armynum=0;armynum<MAXARM;armynum++){
				if((ASOLD>0)&&(ASTAT!=SCOUT)){
					if((occ[AXLOC][AYLOC]== 0)
					||(occ[AXLOC][AYLOC]== country))
						occ[AXLOC][AYLOC]= country;
					else occ[AXLOC][AYLOC]= MAXNTN+5;
				}
			}
			for(nvynum=0;nvynum<MAXNAVY;nvynum++){
				if(NWAR+NMER>0){
					if((occ[NXLOC][NYLOC]== 0)
					||(occ[NXLOC][NYLOC]== country))
						occ[NXLOC][NYLOC]= country;
					else occ[NXLOC][NYLOC]= MAXNTN+5;
				}
			}
		}
	country=save;
}

/*destroy nation--special case if capitol not owned by other nation*/
void
destroy(country)
int country;
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
		for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++)
		if(sct[x][y].owner==country) {
			sct[x][y].people=0;
			sct[x][y].owner=0;
			if(tofood(sct[x][y].vegetation,0)>=DESFOOD)
				sct[x][y].designation=DNODESIG;
#ifdef DERVDESG
			if((sct[x][y].vegetation==DESERT)
			||(sct[x][y].vegetation==ICE))
				sct[x][y].designation=sct[x][y].vegetation;
#endif DERVDESG
		}
	}
	/*slowly take over and all people flee*/
	else if(ntn[sct[ntn[country].capx][ntn[country].capy].owner].race!=ntn[country].race){
		for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++)
		if(sct[x][y].owner==country) {
			/*all kinds of refugees to neighboring countries*/
			flee(x,y,1,0);
			sct[x][y].people=0;
			sct[x][y].owner=0;
			if(tofood(sct[x][y].vegetation,0)>=DESFOOD)
				sct[x][y].designation=DNODESIG;
#ifdef DERVDESG
			if((sct[x][y].vegetation==DESERT)
			||(sct[x][y].vegetation==ICE))
				sct[x][y].designation=sct[x][y].vegetation;
#endif DERVDESG
		}
	}
	/*else same race, so give all land to conqueror*/
	else {
		for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++)
		if(sct[x][y].owner==country){
			sct[x][y].owner=sct[ntn[country].capx][ntn[country].capy].owner;
			if(tofood(sct[x][y].vegetation,0)>=DESFOOD)
				sct[x][y].designation=DNODESIG;
#ifdef DERVDESG
			if((sct[x][y].vegetation==DESERT)
			||(sct[x][y].vegetation==ICE))
				sct[x][y].designation=sct[x][y].vegetation;
#endif DERVDESG
		}
	}
	return;
}

#define ALPHA_SIZE	128

/*movecost contains movement cost unless water  -1 or unenterable land (-2)*/
/* if water and not ajacent to land will cost -4*/
void
updmove(race,country)
int country;
char race;
{
	register struct s_sector	*sptr;
	register int	i,j;
	int x,y;
	short	veg_cost[ ALPHA_SIZE ];
	short	ele_cost[ ALPHA_SIZE ];

	for( j = 0; j < ALPHA_SIZE; j++ )
		veg_cost[j] = ele_cost[j] = -1;

	for( j = 0; veg[j] != '0'; j++ ) {
		switch( race ) {
		case ELF:
			veg_cost[ veg[j] ] = todigit( EVegcost[j] );
			break;
		case DWARF:
			veg_cost[ veg[j] ] = todigit( DVegcost[j] );
			break;
		case ORC:
			veg_cost[ veg[j] ] = todigit( OVegcost[j] );
			break;
		case HUMAN:
		default:
			veg_cost[ veg[j] ] = todigit( HVegcost[j] );
			break;
		} /* switch */
	} /* for */

	for( j = 0; ele[j] != '0'; j++ ) {
		switch( race ) {
		case ELF:
			ele_cost[ ele[j] ] = todigit( EElecost[j] );
			break;
		case DWARF:
			ele_cost[ ele[j] ] = todigit( DElecost[j] );
			break;
		case ORC:
			ele_cost[ ele[j] ] = todigit( OElecost[j] );
			break;
		case HUMAN:
		default:
			ele_cost[ ele[j] ] = todigit( HElecost[j] );
			break;
		} /* switch */
	} /* for */

	for(x=0;x<MAPX;x++) for(y=0;y<MAPX;y++){
		sptr = &sct[x][y];

		if(race==GOD) movecost[x][y] = 0;
		else
		if(sptr->altitude==WATER) {
			movecost[x][y] = -4;
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
			if( i>=0 && j>=0 && i<MAPX && j<MAPY)
				if( sct[i][j].altitude != WATER)
					movecost[x][y] = -1;
		} else
		if(((magic(country,DERVISH)==1)
			||(magic(country,DESTROYER)==1))
		&&((sptr->vegetation==ICE)
			||(sptr->vegetation==DESERT))) {
				movecost[x][y] = 1;
		} else {
			if( veg_cost[ sptr->vegetation ] == -1
			|| ele_cost[ sptr->altitude ] == -1 )
				movecost[x][y] = -2;
			else
				movecost[x][y] = veg_cost[ sptr->vegetation ] + ele_cost[ sptr->altitude ];
		} /* if */
	} /* for */
} /* updmove() */

spreadsheet(nation)
int nation;
{
	register struct s_sector	*sptr;
	register struct nation		*nptr;
	long	product;
	long	city_pop, cap_pop;
	int x,y;

	nptr = &ntn[ nation ];

	spread.revfood = spread.revjewels = spread.reviron = spread.revcap = spread.revcity = 0L;
	spread.incity = spread.incap = spread.ingold = spread.infarm = spread.iniron = 0;
	spread.food = nptr->tfood;
	spread.gold = nptr->tgold;
	spread.iron = nptr->tiron;
	spread.jewels = nptr->jewels;
	spread.sectors = 0;
	spread.civilians = 0;

	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) {
		sptr = &sct[x][y];
		if(sptr->owner!=nation) continue;

		/*update nation file for owner*/
		spread.sectors++;
		spread.civilians += sptr->people;

		/*PRODUCE*/
		/*increase tmin based on mined stuff...*/
		if(sptr->designation==DMINE) {
			spread.iniron += sptr->people;
			product = sptr->iron * (long) sptr->people;
			if(sptr->people>TOMANYPEOPLE) product/=2;
			if(magic(sptr->owner,MINER)==1) product*=2;
			if(magic(sptr->owner,STEEL)==1) product*=2;
			spread.iron += product;
			spread.reviron += product*TAXIRON/100L;
		}
		/*harvest food*/
		else if(sptr->designation==DFARM) {
			spread.infarm += sptr->people;
			product= (long)tofood(sptr->vegetation,sptr->owner) * (long) sptr->people;
			if(sptr->people>TOMANYPEOPLE) product/=2;

			spread.food += product;
			spread.revfood += product*TAXFOOD/100L;
		}
		/*gold mines produce gold*/
		else if(sptr->designation==DGOLDMINE) {
			spread.ingold += sptr->people;
			product = sptr->gold * (long) sptr->people;

			if(sptr->people>TOMANYPEOPLE) product/=2;

			if(magic(sptr->owner,MINER)==1) product*=2;

			spread.jewels += product;
			spread.revjewels += product*TAXGOLD/100L;
		}
		else if(sptr->designation==DCAPITOL) {
			/* it had better be your capitol */
			if((x!=nptr->capx)&&(y!=nptr->capy)) {
				if(sct[nptr->capx][nptr->capy].designation != DCAPITOL){
					nptr->capx=x;
					nptr->capy=y;
					cap_pop  = sptr->people;
					city_pop = 0;
				} else {
					cap_pop = 0;
					city_pop = sptr->people;
					printf("changing %d %d to a town",x,y);
					sptr->designation = DCITY;
				}
			} else {
				city_pop = 0;
				cap_pop = sptr->people;
			}
			spread.incap += cap_pop;
			spread.incity += city_pop;

			if( magic(sptr->owner, ARCHITECT ) ) {
				city_pop *= 2L;
				cap_pop *= 2L;
			}

			spread.revcity +=  city_pop * TAXCITY / 100L;
			spread.revcap +=  cap_pop * TAXCAP / 100L;
		}
		else if(sptr->designation==DCITY) {
			spread.incity += sptr->people;
			city_pop = sptr->people;
			if( magic(sptr->owner, ARCHITECT ) )
				city_pop *= 2L;
			spread.gold +=  city_pop * TAXCITY / 100L;
			spread.revcity +=  city_pop * TAXCITY / 100L;
		}
#ifndef DERVDESG
		else if(((magic(sptr->owner,DERVISH)==1)
			||(magic(sptr->owner,DESTROYER)==1))
		&&((sptr->vegetation==ICE) ||(sptr->vegetation==DESERT))
		&&(sptr->people>0)) {
			product= 6L * sptr->people;
			spread.food += product;
			spread.revfood += product*TAXFOOD/100L;
		}
#endif DERVDESG
	}
	spread.gold += spread.revfood + spread.revjewels + spread.reviron + spread.revcity + spread.revcap;
}

#ifdef CONQUER

/* string inputing routine to allow deleting */
get_nname(str)
char str[];
{
	char ch;
	int done=0,count=0,maxsize,xpos,ypos;

	maxsize=12;	/* nation name input size */
	while(!done) {
		ch=getch();
		if (isprint(ch)) {
			if (count<maxsize) {
				/* only input displayable characters */
				addch(ch);
				refresh();
				str[count++] = ch;
			}
		}
		else if ((ch=='\b' || ch=='\177')&&(count))
		{
			/* only delete what was printed */
			getyx(stdscr,ypos,xpos);
			move(ypos,--xpos);
			addch(' ');
			move(ypos,xpos);
			refresh();
			count--;
		} else if (ch=='\n') {
			done=1;
		}
	}
	str[count++] = '\0';
	return(count);
}
#endif CONQUER
