#	MODIFICATION OF THIS FILE COMITS THE MODIFIER TO FOLLOW THE
#	THE LIMITED USE CONTRACT CONTAINED IN THE FILE "header.h"
MAKE	= /bin/make
CC	= /bin/cc
CFLAGS	= -O
#GETOPT	= getopt.o	# define this if you don't have it in your library

#if the final link does not compile change to the line below
#LIBRARIES = -lcurses -ltermcap
LIBRARIES = -lcurses

RM	= /bin/rm -f
LD	= mld
LDFLAGS	= $(LLDFLAGS)

#	This directory is where the executable should be stored
EXECUTABLE = /d7/c7913/smile/oldgame

#	This directory is where individual Conquer game data will be stored.
#	Multiple games are now supported.  Each should have its own directory.
DATA = /d7/c7913/smile/oldgame/game.1

OBJ=combat.o commands.o cexecute.o forms.o io.o main.o makeworld.o move.o newlogin.o update.o magic.o npc.o reports.o misc.o randevent.o 
FILS=combat.c commands.c cexecute.c forms.c io.c main.c makeworld.c move.c newlogin.c update.c magic.c npc.c reports.c misc.c randevent.c 
HEADERS=header.h data.h
SUPT1=nations Makefile help README run man.page
SUPT2=execute messages news

all: $(OBJ) 
	@echo phew... 
	@echo if the next command does not compile you might also need -ltermcap
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(EXECUTABLE)/conquer $(OBJ) $(LIBRARIES)
	@echo YAY!  make install if this is your !first! time.  This will set up
	@echo permissions, zero appropriate initial files, and set up the world.
	@echo Note that this version expects each game to have a separate data 
	@echo directory - please see documentation. Have Fun.

$(OBJ): data.h  

$(OBJ): header.h  

clobber:
	$(RM) shar1 shar2 shar3 *.o conquer .data core
	$(RM) $(DATA)/conq.* $(EXECUTABLE)/conquer 

clean:
	$(RM) shar1 shar2 shar3 *.o core

install:
	-if test -d $(EXECUTABLE) ; \
	then echo "EXECUTABLE DIRECTORY OK" ; \
	else mkdir $(EXECUTABLE) ; \
	fi 
	-if test -d $(DATA) ; \
	then echo "DATA DIRECTORY OK" ; \
	else mkdir $(DATA) ; \
	fi 
	cp help $(DATA)/help
	cp nations $(DATA)/nations
	cp run $(DATA)/run
	chmod 0777 $(EXECUTABLE) $(DATA) $(EXECUTABLE)/conquer
	conquer -d$(DATA) -m
	chmod 0700 $(DATA)/run
	chmod 0644 $(DATA)/help $(DATA)/news 

lint:
	lint -u $(FILS) 

docs:
	sed -e "s/^END//g" help | pr

cpio:
	$(RM) core
	find . -name '*[CrpsEech]' -print | cpio -ocBv > cpiosv

shar:	$(FILS)
	createshar 1 combat.c commands.c forms.c magic.c 
	mv shar.out shar1 
	createshar 2  move.c makeworld.c main.c
	mv shar.out shar2 
	createshar 3 $(SUPT1) header.h update.c
	mv shar.out shar3
	createshar 4 npc.c newlogin.c data.h
	mv shar.out shar4
	createshar 5 cexecute.c reports.c io.c misc.c randevent.c 
	mv shar.out shar5

rmshar:
	$(RM) shar1
	$(RM) shar2
	$(RM) shar3
	$(RM) shar4
	$(RM) shar5
