#include	<ctype.h>
#include	<stdio.h>
#include	"header.h"
#include 	"data.h"

#ifdef SYSV
char    *memset();
#endif

int	powers[] = {
	WARRIOR,
	CAPTAIN,
	WARLORD,
	MI_MONST,
	AV_MONST,
	MA_MONST,
	SPY,
	KNOWALL,
	DERVISH,
	DESTROYER,
	HIDDEN,
	THE_VOID,
	ARCHITECT,
	VAMPIRE,
	HEALER,
	MINER,
	URBAN,
	STEEL,
	ARCHER,
	CAVALRY,
	BREEDER,
	0
};

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

verify_ntn( __file__, __line__ )
char	__file__[];
int	__line__;
{
	register struct nation	*nptr;
	register int		i;
	register int		country;
	struct army	*a;

	for( country = 0; country < 32; country++ ) {
		nptr = &ntn[country];

		if( nptr->tiron < 0.0 ) {
			fprintf( stderr, "file %s: line %d: ntn[%d].tiron = %ld\n",
				__file__, __line__, country, nptr->tiron );
		}

		if( nptr->jewels < 0 ) {
			fprintf( stderr, "file %s: line %d: ntn[%d].jewels = %ld\n",
				__file__, __line__, country, nptr->jewels );
		}

		for( i = 0; i < MAXARM; i++ ) {
			a = &nptr->arm[i];
			if( a->sold < 0 ) {
				fprintf( stderr, "file %s: line %d: ntn[%d].arm[%d].sold = %d\n",
					__file__, __line__, country, i, nptr->arm[i].sold );
				nptr->arm[i].sold = 0;
			}
			if( a->xloc < 0 || a->xloc >= MAPX ) {
				fprintf( stderr, "file %s: line %d: ntn[%d].arm[%d].xloc = %d\n",
					__file__, __line__, country, i, nptr->arm[i].xloc );
				nptr->arm[i].xloc = 0;
			}
			if( a->yloc < 0 || a->yloc >= MAPY ) {
				fprintf( stderr, "file %s: line %d: ntn[%d].arm[%d].yloc = %d\n",
					__file__, __line__, country, i, nptr->arm[i].yloc );
				nptr->arm[i].yloc = 0;
			}
		} /* for */

		for( i = 0; i < 32; i++ ) {
			if( nptr->dstatus[i] > JIHAD ) {
				fprintf( stderr, "file %s: line %d: ntn[%d].dstatus[%d] = %d\n",
					__file__, __line__, country, i, nptr->dstatus[i] );
				nptr->dstatus[i] = WAR;
			}
			if( nptr->dstatus[i] < UNMET ) {
				fprintf( stderr, "file %s: line %d: ntn[%d].dstatus[%d] = %d\n",
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
				sptr->people = 0;
			}

			if( sptr->owner != 0 && sptr->altitude == WATER ) {
				fprintf( stderr, "file %s: line %d: sct[%d][%d].owner = %s (a water sector)\n",__file__,__line__, x, y, ntn[sptr->owner].name );
				sptr->owner = 0;
			}
		} /* for */
	} /* for */
} /* verify_sct() */

verifydata( __file__, __line__ )
char	__file__[];
int	__line__;
{
	/* check for invalid values */
	verify_ntn( __file__, __line__ );
	verify_sct( __file__, __line__ );
}/* verifydata() */

get_number()
{
	char	buffer[ 20 ];

	echo();
	getstr( buffer );
	noecho();

	return( atoi( buffer ) );
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

			switch( sct[x][y].altitude ) {
			case PEAK:
			case WATER:
				continue;
			} /* switch */

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
			/* BUG: take into account THE_VOID, HIDDEN, and SPY */
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

		exit( 1 );
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

#ifndef NPCSWATER
/*
 *	water_2reachp()
 */

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

/*
 *	water_reachp()
 */

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

		exit( 1 );
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
#endif

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
/* tyranny */	{ 2,    1,     2,        0,    1,    1,     1,    0 },
/* demon */	{ 2,    0,     1,        0,    1,    0,     10,   0 },
/* dragon */	{ 0,    0,     0,        10,   20,   0,     0,    0 },
/* shadow */	{ 2,    0,     0,        0,    0,    5,     0,    0 },
/* miner */	{ 0,    0,    5,    0,    10,   10,   1,    5 },
};

int
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
	total += wght->magics * num_powers(country);
	total += wght->ships * nptr->tships / 10;

	return( total );
} /* score_one() */
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
	switch( sct[x][y].altitude ) {
	case WATER:
	case PEAK:
		return( 0 );
	}

	if( !isdigit( sct[x][y].vegetation ) )
		return( 0 );

	return( 1 );
}

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

int
num_powers(country)
{
int count_magic=0;
int try;
for( try = 0; powers[try] != 0; try++ )
	if( magic(country, powers[try] ) != 0 )
		count_magic++;
return(count_magic);
}

