#	conquer: Copyright (c) 1988 by Edward M Barlow
#	MODIFICATION OF THIS FILE COMITS THE MODIFIER TO FOLLOW THE
#	THE LIMITED USE CONTRACT CONTAINED IN THE FILE "header.h"
MAKE	= /bin/make
CC	= /bin/cc
RM	= /bin/rm -f
LD	= mld

#	This should be installed by whomever you want to own the game.
#	I recommend "games" or "root".

#	uncomment the next line if you dont have getopt in your library
#	(eg you are on a pc, or a non unix system).  getopt.c is a
#	public domain software, and has not been tested by the author
#	of conquer.
#GETOPT	= getopt.o	

#if the final link does not compile change to the line below
#LIBRARIES = -lcurses -ltermcap
LIBRARIES = -lcurses

#	DEFAULT is the directory where default nations & help files will be
#	stored.  It is also the default directory = where players will play
#	if they do not use the -d option.
DEFAULT = /c28/smile/game/default

#	The following CFLAGS should be set by a normal user
CFLAGS  = -DDEFAULTDIR=\"$(DEFAULT)\" -O
#	The following CFLAGS should be used if you wish to debug the game
#CFLAGS  = -DDEFAULTDIR=\"$(DEFAULT)\" -DDEBUG -g

#	this is the name of the user executable
#	the user executable contains commands for the games players
GAME = conquer
#	this is the name of the administrative executable
#	the administrative executable contains commands for the game super user
ADMIN = cadmin

#	This directory is where the executables will be stored
EXEDIR = /c28/smile/game/runv

#	GAME IDENTIFICATION
#	this is the game identifier.  See the DATA variable below
GAMEID = 1
#	This directory is where individual Conquer game data will be stored.
#	As multiple simultaneous games are supported, each game must have its
#	own directory.
DATA = $(EXEDIR)/lib$(GAMEID)

# AFILS are files needed for game updating...
AFILS = combat.c cexecute.c io.c admin.c makeworld.c  \
newlogin.c update.c magic.c npc.c misc.c randevent.c data.c trade.c
AOBJS = combat.o cexecuteA.o ioA.o admin.o makeworld.o  \
newlogin.o update.o magicA.o npc.o miscA.o randevent.o dataA.o \
tradeA.o $(GETOPT)

# GFILS are files needed to run a normal interactive game
GFILS = commands.c cexecute.c forms.c io.c main.c move.c \
magic.c misc.c reports.c data.c display.c extcmds.c trade.c
GOBJS = commands.o cexecute.o forms.o io.o main.o move.o \
magic.o misc.o reports.o data.o display.o extcmds.o trade.o $(GETOPT)

HEADERS=header.h data.h newlogin.h
HELPFILE=help.txt
SUPT1=nations Makefile $(HELPFILE) README run man.page 
SUPT2=execute messages news
HELPOUT=help
ALLFILS=$(SUPT1) $(HEADERS) $(AFILS) commands.c forms.c main.c move.c \
reports.c display.c extcmds.c

all:	$(ADMIN) $(GAME) helpfile
	@echo YAY! make new_game to set up permissions, zero appropriate
	@echo initial files, move $(GAME) and $(ADMIN) to 
	@echo $(EXEDIR), and set up the world.
	@echo If a game is in progress, make install will just move $(GAME) 
	@echo and $(ADMIN) to $(EXEDIR).
	@echo

$(ADMIN):	$(AOBJS)
	@echo phew...
	@echo if the next command does not you might also need -ltermcap
	@echo === compiling administrative functions
	$(CC) -O -o $(ADMIN) $(AOBJS) $(LIBRARIES)

$(GAME):	$(GOBJS)
	@echo phew...
	@echo if the next command does not compile you might also need -ltermcap
	@echo === compiling user interface
	$(CC) -O -o $(GAME) $(GOBJS) $(LIBRARIES)

clobber:
	$(RM) *.o core newhelp conquer.doc $(GAME) $(ADMIN) $(HELPOUT) 2>/dev/null

clean:
	$(RM) *.o core conquer.doc newhelp $(HELPOUT) 2>/dev/null

install:	$(ADMIN) $(GAME) helpfile
	cp $(GAME) $(ADMIN) $(EXEDIR)
	cp $(HELPOUT) $(DATA)
	cp $(HELPOUT) $(DEFAULT)
	chmod 4755 $(EXEDIR)/$(GAME)
	chmod 4750 $(EXEDIR)/$(ADMIN) 

new_game:	$(ADMIN) $(GAME) helpfile
	@echo Installing new game
	-mkdir $(EXEDIR) 2>/dev/null
	-mkdir $(DATA) 2>/dev/null
	-mkdir $(DEFAULT)  2>/dev/null
	chmod 755 $(EXEDIR)
	chmod 751 $(DATA) $(DEFAULT)
	cp $(GAME) $(ADMIN) $(EXEDIR)
	chmod 4755 $(EXEDIR)/$(GAME) $(EXEDIR)/$(ADMIN)
	chmod 0700 run
	chmod 0644 $(HELPOUT)
	chmod 0600 nations
	cp $(HELPOUT) nations $(DATA)
	cp $(HELPOUT) nations $(DEFAULT)
	@echo now making the world
	$(EXEDIR)/$(ADMIN) -d$(DATA) -m
	$(EXEDIR)/$(ADMIN) -d$(DATA) -a

helpfile:	$(HELPOUT)

$(HELPOUT):	newhelp.c data.o header.h data.h
	$(CC) $(CFLAGS) newhelp.c data.o -o newhelp
	@echo Building the help file
	newhelp
	cat $(HELPFILE) | sed -f helpscript > $(HELPOUT)
	$(RM) helpscript
	-mkdir $(DEFAULT)  2>/dev/null
	cp $(HELPOUT) $(DEFAULT)

lint:
	lint -DDEFAULTDIR=\"$(DEFAULT)\" -DCONQUER $(GFILS) > lintg
	lint -DDEFAULTDIR=\"$(DEFAULT)\" -DADMIN $(AFILS) > linta

docs:	$(HELPOUT) conquer.doc

conquer.doc:
	sed -e "s/^END//g" $(HELPOUT) > conquer.doc

cpio:
	-$(RM) core
	find . -name '*[CrpsEech]' -print | cpio -ocBv > cpiosv

shar:	
	echo " lines   words chars   FILENAME" > MANIFEST
	wc $(ALLFILS) >> MANIFEST
	createshar 1 combat.c commands.c forms.c
	mv shar.out shar1
	createshar 2  move.c makeworld.c main.c
	mv shar.out shar2
	createshar 3 $(SUPT1)
	mv shar.out shar3
	createshar 4 npc.c newlogin.c MANIFEST newlogin.h
	mv shar.out shar4
	createshar 5 reports.c misc.c randevent.c
	mv shar.out shar5
	createshar 6 magic.c update.c cexecute.c data.c MANIFEST
	mv shar.out shar6
	createshar 7 admin.c display.c getopt.c newhelp.c extcmds.c io.c data.h header.h 
	mv shar.out shar7

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
makeworld.o:	data.h header.h makeworld.c
	$(CC) $(CFLAGS) -DADMIN -c makeworld.c
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
randevent.o:	data.h header.h randevent.c
	$(CC) $(CFLAGS) -DADMIN -c randevent.c
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
trade.o:	data.h header.h trade.c
	$(CC) $(CFLAGS) -DCONQUER -c trade.c
tradeA.o:	data.h header.h trade.c
	$(CC) $(CFLAGS) -DADMIN -c trade.c
	mv trade.o tradeA.o
