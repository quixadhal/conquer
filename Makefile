#	conquer: Copyright (c) 1988 by Edward M Barlow
#
#	BY CHANGING THIS FILE, YOU AGREE TO ABIDE BY THE LIMITATIONS STATED IN
#	THE LIMITED USE CONTRACT CONTAINED IN THE FILE "header.h"
#
MAKE	= /bin/make
CC	= /bin/cc
RM	= /bin/rm -f

#	LN must be "ln -s" if source, data, and default span disks
LN	= ln
CP	= cp
NULL	= 2>/dev/null

#	This should be installed by whomever you want to own the game.
#	I recommend "games" or "root".

#if the final link does not compile change to the line below
#LIBRARIES = -lcurses -ltermcap
LIBRARIES = -lcurses

#	this is the name of the user executable
#	the user executable contains commands for the games players
GAME = conquer
#	this is the name of the administrative executable
#	the administrative executable contains commands for the game super user
ADMIN = conqrun

#	This directory is where the executables will be stored
#	This would be the equivalent of /usr/games
EXEDIR = /c28/smile/game/runv

#	GAME IDENTIFICATION
#	GAMEID is the game identifier
#	DATA is the directory where an individual Conquer game data will be
#	stored.  It is the directory you use in the -d option of the game.
#	"make new_game" will build a world in that directory.  The current
#	game will be automatically executed upon login (no need for -d)
#	but if other games are compiled, you need to use -d (ie. the
#	game automatically looks at the DATA directory it was compiled with.
GAMEID = 1
DATA = $(EXEDIR)/lib$(GAMEID)

#	The following CFLAGS should be set by a normal user
CFLAGS  = -DDEFAULTDIR=\"$(DATA)\" -O -s -DEXEDIR=\"$(EXEDIR)\"
#	The following CFLAGS should be used if you wish to debug the game
#CFLAGS  = -DDEFAULTDIR=\"$(DATA)\" -DDEBUG -g -DEXEDIR=\"$(EXEDIR)\"

# AFILS are files needed for game updating...
AFILS = combat.c cexecute.c io.c admin.c makeworl.c  navy.c spew.c \
newlogin.c update.c magic.c npc.c misc.c randeven.c data.c trade.c check.c
AOBJS = combat.o cexecuteA.o ioA.o admin.o makeworl.o  navyA.o \
newlogin.o update.o magicA.o npc.o miscA.o randeven.o dataA.o \
tradeA.o check.o $(GETOPT) spew.o

# GFILS are files needed to run a normal interactive game
GFILS = commands.c cexecute.c forms.c io.c main.c move.c navy.c \
magic.c misc.c reports.c data.c display.c extcmds.c trade.c check.c
GOBJS = commands.o cexecute.o forms.o io.o main.o move.o navy.o \
magic.o misc.o reports.o data.o display.o extcmds.o trade.o check.o $(GETOPT)

#txt[0-5] are input help files.  help[0-5] are output
HELP0=txt0
HELP1=txt1
HELP2=txt2
HELP3=txt3
HELP4=txt4
HELP5=txt5
HELPOUT=help

HEADERS=header.h data.h newlogin.h patchlevel.h
HELPFILES= $(HELP0) $(HELP1) $(HELP2) $(HELP3) $(HELP4) $(HELP5)
ALLFILS=$(HEADERS) $(AFILS) commands.c forms.c main.c move.c \
reports.c display.c extcmds.c $(HELPFILES) nations Makefile README \
run man.pag notes.v4 rules sort.c newhelp.c

all:	$(ADMIN) $(GAME) helpfile conqsort
	@echo YAY! make new_game to set up permissions, zero appropriate
	@echo initial files, move $(GAME) and $(ADMIN) to
	@echo $(EXEDIR), and set up the world.
	@echo If a game is in progress, make install will just move $(GAME)
	@echo and $(ADMIN) to $(EXEDIR).
	@echo

$(ADMIN):	$(AOBJS)
	@echo phew...
	@echo if the next command does not compile, you might also need -ltermcap
	@echo === compiling administrative functions
	$(CC) -p -o $(ADMIN) $(AOBJS) $(LIBRARIES)

$(GAME):	$(GOBJS)
	@echo phew...
	@echo if the next command does not compile, you might also need -ltermcap
	@echo === compiling user interface
	$(CC) -O -o $(GAME) $(GOBJS) $(LIBRARIES)

conqsort:
	$(CC) -O -oconqsort sort.c

clobber:
	$(RM) *.o core newhelp sed.1 sed.2 lint* \
		conquer.doc $(GAME) $(ADMIN) $(HELPOUT)[0-5] 2>/dev/null

clean:
	$(RM) *.o core conquer.doc sed.1 sed.2 newhelp $(HELPOUT)[0-5] 2>/dev/null

install:  all
	-$(LN) $(GAME) $(EXEDIR)/$(GAME) $(NULL)
	-$(LN) $(ADMIN) $(EXEDIR)/$(ADMIN) $(NULL)
	-$(LN) conqsort $(EXEDIR)/conqsort $(NULL)
	chmod 4755 $(EXEDIR)/$(GAME)
	chmod 4750 $(EXEDIR)/$(ADMIN)
	$(CP) conqsort nations rules $(HELPOUT)? $(DATA) $(NULL)

new_game:  all
	@echo Installing new game in $(EXEDIR)
	-mkdir $(EXEDIR) 2>/dev/null
	chmod 755 $(EXEDIR)
	-$(LN) $(GAME) $(EXEDIR)/$(GAME) $(NULL)
	-$(LN) $(ADMIN) $(EXEDIR)/$(ADMIN) $(NULL)
	chmod 4755 $(EXEDIR)/$(GAME)
	chmod 4750 $(EXEDIR)/$(ADMIN)
	chmod 0700 run
	@echo Copying data to library $(DATA)
	-mkdir $(DATA) 2>/dev/null
	chmod 751 $(DATA)
	chmod 0600 nations
	$(CP) nations rules $(HELPOUT)? $(DATA) $(NULL)
	@echo now making the world
	$(EXEDIR)/$(ADMIN) -d$(DATA) -m
	$(EXEDIR)/$(ADMIN) -d$(DATA) -a

helpfile:	$(HELPOUT)0
	@echo Helpfiles built

$(HELPOUT)0:	newhelp $(HELPFILES)
	@echo Building the help files
	newhelp
	cat $(HELP0) | sed -f sed.1 > $(HELPOUT)0
	cat $(HELP1) | sed -f sed.1 | sed -f sed.2 > $(HELPOUT)1
	cat $(HELP2) | sed -f sed.1 > $(HELPOUT)2
	cat $(HELP3) | sed -f sed.1 | sed -f sed.2 > $(HELPOUT)3
	cat $(HELP4) | sed -f sed.1 > $(HELPOUT)4
	cat $(HELP5) | sed -f sed.1 > $(HELPOUT)5
	chmod 0644 $(HELPOUT)[0-5]
	-$(RM) sed.1 sed.2

lint:
	lint -DDEFAULTDIR=\"$(DATA)\" -DEXEDIR=\"$(EXEDIR)\" -DCONQUER $(GFILS) $(LIBRARIES)> lintg
	lint -DDEFAULTDIR=\"$(DATA)\" -DEXEDIR=\"$(EXEDIR)\" -DADMIN $(AFILS) $(LIBRARIES) > linta

docs:	helpfile conquer.doc

conquer.doc:	$(HELPOUT)0 $(HELPOUT)1 $(HELPOUT)2 $(HELPOUT)3 $(HELPOUT)4 $(HELPOUT)5
	cat $(HELPOUT)?|sed -e "s/^DONE//g"|sed -e "s/^END//g" > conquer.doc

shar:
	echo " lines   words chars   FILENAME" > MANIFEST
	wc $(ALLFILS) >> MANIFEST
	$(HOME)/src/xshar/xshar -D -c -l64 -oshar -v $(ALLFILS)

combat.o:	data.h header.h combat.c
	$(CC) $(CFLAGS) -DADMIN -c combat.c
cexecute.o:	data.h header.h cexecute.c
	$(CC) $(CFLAGS) -DCONQUER -c cexecute.c
io.o:	data.h header.h  io.c
	$(CC) $(CFLAGS) -DCONQUER -c io.c
cexecuteA.o:	data.h header.h cexecute.c
	$(CC) $(CFLAGS) -DADMIN -c cexecute.c
	mv cexecute.o cexecuteA.o
ioA.o:	data.h header.h  io.c
	$(CC) $(CFLAGS) -DADMIN -c io.c
	mv io.o ioA.o
admin.o:	data.h header.h admin.c
	$(CC) $(CFLAGS) -DADMIN -c admin.c
makeworl.o:	data.h header.h makeworl.c
	$(CC) $(CFLAGS) -DADMIN -c makeworl.c
newlogin.o:	data.h header.h newlogin.h newlogin.c
	$(CC) $(CFLAGS) -DADMIN -c newlogin.c
update.o:	data.h header.h update.c
	$(CC) $(CFLAGS) -DADMIN -c update.c
magic.o:	data.h header.h magic.c
	$(CC) $(CFLAGS) -DCONQUER -c magic.c
magicA.o:	data.h header.h magic.c
	$(CC) $(CFLAGS) -DADMIN -c magic.c
	mv magic.o magicA.o
npc.o:	data.h header.h npc.c
	$(CC) $(CFLAGS) -DADMIN -c npc.c
misc.o:	data.h header.h misc.c
	$(CC) $(CFLAGS) -DCONQUER -c misc.c
miscA.o:	data.h header.h misc.c
	$(CC) $(CFLAGS) -DADMIN -c misc.c
	mv misc.o miscA.o
randeven.o:	data.h header.h randeven.c
	$(CC) $(CFLAGS) -DADMIN -c randeven.c
data.o:	data.h header.h data.c
	$(CC) $(CFLAGS) -DCONQUER -c data.c
dataA.o:	data.h header.h data.c
	$(CC) $(CFLAGS) -DADMIN -c data.c
	mv data.o dataA.o
display.o:	data.h header.h display.c
	$(CC) $(CFLAGS) -DCONQUER -c display.c
reports.o:	data.h header.h reports.c
	$(CC) $(CFLAGS) -DCONQUER -c reports.c
move.o:	data.h header.h move.c
	$(CC) $(CFLAGS) -DCONQUER -c move.c
main.o:	data.h header.h main.c
	$(CC) $(CFLAGS) -DCONQUER -c main.c
forms.o:	data.h header.h forms.c
	$(CC) $(CFLAGS) -DCONQUER -c forms.c
commands.o:	data.h header.h commands.c
	$(CC) $(CFLAGS) -DCONQUER -c commands.c
check.o:	data.h header.h check.c
	$(CC) $(CFLAGS) -c check.c
trade.o:	data.h header.h trade.c
	$(CC) $(CFLAGS) -DCONQUER -c trade.c
tradeA.o:	data.h header.h trade.c
	$(CC) $(CFLAGS) -DADMIN -c trade.c
	mv trade.o tradeA.o
navyA.o:	data.h header.h navy.c
	$(CC) $(CFLAGS) -DADMIN -c navy.c
	mv navy.o navyA.o
navy.o:	data.h header.h trade.c
	$(CC) $(CFLAGS) -DCONQUER -c navy.c
newhelp:	data.o header.h data.h newhelp.c
	@echo Compiling the help program
	$(CC) $(CFLAGS) newhelp.c data.o -o newhelp
