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
#include <stdio.h>
#include "header.h"
#include "data.h"

int
main()
{
	FILE *fp, *fopen();
	if((fp=fopen("helpscript","w"))==NULL) {
		printf("cant open helpscript file\n");
		exit(FAIL);
	}

	/* create sed script */
	fprintf(fp,"s/XOWNER/%s/g\n",OWNER);
	fprintf(fp,"s/XLOGIN/%s/g\n",LOGIN);
	fprintf(fp,"s/XVERSION/%s/g\n",VERSION);
	fprintf(fp,"s/XPWATER/%d/g\n",PWATER);
	fprintf(fp,"s/XPVULCAN/%d/g\n",PVULCAN);
	fprintf(fp,"s/XPMOUNT/%d/g\n",PMOUNT);
	fprintf(fp,"s/XPSTORM/%d/g\n",PSTORM);
	fprintf(fp,"s/XMAPX/%d/g\n",MAPX);
	fprintf(fp,"s/XMAPY/%d/g\n",MAPY);
	fprintf(fp,"s/XMAXNTN/%d/g\n",MAXNTN);
	fprintf(fp,"s/XMAXPTS/%d/g\n",MAXPTS);
	fprintf(fp,"s/XMAXARM/%d/g\n",MAXARM);
	fprintf(fp,"s/XMAXNAVY/%d/g\n",MAXNAVY);

	/* check all the defined options */
#ifdef LZARD
	fprintf(fp,"s/XLZARD/(True) /g\n");
#else
	fprintf(fp,"s/XLZARD/(False)/g\n");
#endif
#ifdef OGOD
	fprintf(fp,"s/XOGOD/(True) /g\n");
#else
	fprintf(fp,"s/XOGOD/(False)/g\n");
#endif
#ifdef DERVDESG
	fprintf(fp,"s/XDERVDESG/Dervish can/g\n");
#else
	fprintf(fp,"s/XDERVDESG/Dervish cannot/g");
#endif
#ifdef SYSV
	fprintf(fp,"s/XSYSV/(True) /g\n");
#else
	fprintf(fp,"s/XSYSV/(False)/g\n");
#endif
#ifdef MONSTER
	fprintf(fp,"s/XMONSTER/(True) /g\n");
#else
	fprintf(fp,"s/XMONSTER/(False)/g\n");
#endif
#ifdef NPC
	fprintf(fp,"s/XNPC/(True) /g\n");
#else
	fprintf(fp,"s/XNPC/(False)/g\n");
#endif
#ifdef CHEAT
	fprintf(fp,"s/XCHEAT/(True) /g\n");
#else
	fprintf(fp,"s/XCHEAT/(False)/g\n");
#endif
#ifdef CMOVE
	fprintf(fp,"s/XCMOVE/(True) /g\n");
#else
	fprintf(fp,"s/XCMOVE/(False)/g\n");
#endif
#ifdef RANEVENT
	fprintf(fp,"s/XRANEVENT/(True) /g\n");
#else
	fprintf(fp,"s/XRANEVENT/(False)/g\n");
#endif
#ifdef VULCANIZE
	fprintf(fp,"s/XVULCANIZE/(True) /g\n");
#else
	fprintf(fp,"s/XVULCANIZE/(False)/g\n");
#endif
#ifdef STORMS
	fprintf(fp,"s/XSTORMS/(True) /g\n");
#else
	fprintf(fp,"s/XSTORMS/(False)/g\n");
#endif
#ifdef ORCTAKE
	fprintf(fp,"s/XORCTAKE/(True) /g\n");
#else
	fprintf(fp,"s/XORCTAKE/(False)/g\n");
#endif
#ifdef HIDELOC
	fprintf(fp,"s/XHIDELOC/(True) /g\n");
#else
	fprintf(fp,"s/XHIDELOC/(False)/g\n");
#endif
#ifdef TRADE
	fprintf(fp,"s/XTRADE/(True) /g\n");
#else
	fprintf(fp,"s/XTRADE/(False)/g\n");
#endif
	/* map sectors */
	fprintf(fp,"s/ZMOUNTAIN/MOUNTAIN (%c)/g\n",MOUNTAIN);
	fprintf(fp,"s/ZHILL/HILL (%c)/g\n",HILL);
	fprintf(fp,"s/ZCLEAR/CLEAR (%c)/g\n",CLEAR);
	fprintf(fp,"s/ZWATER/WATER (%c)/g\n",WATER);
	fprintf(fp,"s/ZPEAK/PEAK (%c)/g\n",PEAK);
	fprintf(fp,"s/ZVOLCANO/VOLCANO (%c)/g\n",VOLCANO);
	fprintf(fp,"s/ZDESERT/DESERT (%c)/g\n",DESERT);
	fprintf(fp,"s/ZJUNGLE/JUNGLE (%c)/g\n",JUNGLE);
	fprintf(fp,"s/ZTUNDRA/TUNDRA (%c)/g\n",TUNDRA);
	fprintf(fp,"s/ZICE/ICE (%c)/g\n",ICE);
	fprintf(fp,"s/ZSWAMP/SWAMP (%c)/g\n",SWAMP);
	fprintf(fp,"s/ZGOOD/GOOD (%c)/g\n",GOOD);
	fprintf(fp,"s/ZFOREST/FOREST (%c)/g\n",FOREST);
	fprintf(fp,"s/ZLIGHT/LIGHT VEG (%c)/g\n",LT_VEG);
	fprintf(fp,"s/ZWOOD/WOOD (%c)/g\n",WOOD);
	fprintf(fp,"s/ZBARREN/BARREN (%c)/g\n",BARREN);

	/* designations */
	fprintf(fp,"s/ZMINE/MINE\t\t (%c)/g\n",DMINE);
	fprintf(fp,"s/ZFARM/FARM\t\t (%c)/g\n",DFARM);
	fprintf(fp,"s/ZGOLD/GOLD MINE\t (%c)/g\n",DGOLDMINE);
	fprintf(fp,"s/ZCASTLE/CASTLE\t\t (%c)/g\n",DCASTLE);
	fprintf(fp,"s/ZDEVASTATED/DEVASTATED\t (%c)/g\n",DDEVASTATED);
	fprintf(fp,"s/ZTOWN/TOWN\t\t (%c)/g\n",DCITY);
	fprintf(fp,"s/ZCAPITOL/CAPITOL\t\t (%c)/g\n",DCAPITOL);

	/* other defines */
	fprintf(fp,"s/XNORANDEVENTS/%d/g\n",NORANDEVENTS);
	fprintf(fp,"s/XLANDSEE/%d/g\n",LANDSEE);
	fprintf(fp,"s/XNAVYSEE/%d/g\n",NAVYSEE);
	fprintf(fp,"s/XARMYSEE/%d/g\n",ARMYSEE);
	fprintf(fp,"s/XTAXFOOD/%ld/g\n",TAXFOOD);
	fprintf(fp,"s/XTAXIRON/%ld/g\n",TAXIRON);
	fprintf(fp,"s/XTAXGOLD/%ld/g\n",TAXGOLD);
	fprintf(fp,"s/XTAXCAP/%ld/g\n",TAXCAP);
	fprintf(fp,"s/XTAXCITY/%ld/g\n",TAXCITY);
	fprintf(fp,"s/XSHIPMAINT/%ld/g\n",SHIPMAINT);
	fprintf(fp,"s/XSHIPCP/%ld/g\n",SHIPHOLD);
	fprintf(fp,"s/XDESCOST/%ld/g\n",DESCOST);
	fprintf(fp,"s/XFORTCOST/%ld/g\n",FORTCOST);
	fprintf(fp,"s/XWARSHPCOST/%ld/g\n",WARSHPCOST);
	fprintf(fp,"s/XMERSHPCOST/%ld/g\n",MERSHPCOST);
	fprintf(fp,"s/XSHIPCREW/%ld/g\n",SHIPCREW);
	fprintf(fp,"s/XSHIPHOLD/%d/g\n",SHIPHOLD);
	fprintf(fp,"s/XCITYLIMIT/%d/g\n",CITYLIMIT);
	fprintf(fp,"s/XMILRATIO/%d/g\n",MILRATIO);
	fprintf(fp,"s/XMILINCAP/%d/g\n",MILINCAP);
	fprintf(fp,"s/XBRIBE/%ld/g\n",BRIBE);
	fprintf(fp,"s/XDESFOOD/%d/g\n",DESFOOD);
	fprintf(fp,"s/XTOMUCHMINED/%d/g\n",TOMUCHMINED);
	fprintf(fp,"s/XTOMANYPEOPLE/%d/g\n",TOMANYPEOPLE);
	fprintf(fp,"s/XFINDPERCENT/%d/g\n",FINDPERCENT);
	fprintf(fp,"s/XTAKEPRICE/%d/g\n",TAKEPRICE);

	close(fp);
	exit(SUCCESS);
}
