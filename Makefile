#	MODIFICATION OF THIS FILE COMMITS THE MODIFIER TO FOLLOW THE
#	THE LIMITED USE CONTRACT CONTAINED IN THE FILE "header.h"
MAKE	= $(BINROOT)/bin/make
CC	= $(BINROOT)/bin/cc
CURSES	= microcurses
CFLAGS	= -O
RM	= /bin/rm -f
LD	= mld
LDFLAGS	= $(LLDFLAGS)
EXECUTABLE = .
DATA = ./lib
LIB	= /lib
SHAREDLIB= $(LIB)/crt0s.o $(LIB)/shlib.ifile

OBJ=main.o makeworld.o io.o newlogin.o forms.o commands.o update.o move.o combat.o  execute.o magic.o npc.o reports.o
FILS=combat.c commands.c execute.c forms.c io.c main.c makeworld.c move.c newlogin.c update.c magic.c npc.c reports.c
HEADERS=header.h
SUPT1=.nations Makefile .help README run
SUPT2=.execute .messages .news
MISC= MISC/MAGIC MISC/creleader.c MISC/combat.c MISC/io.c MISC/ucommands.c MISC/rivers 

all: $(OBJ) 
	@echo phew... it is time to link, link your hands together and pray this works
	$(CC) $(LDFLAGS) -o $(EXECUTABLE)/conquest $(OBJ) -lcurses -ltermcap
	@echo YAY!  make install if this is your !first! time.  this will set up
	@echo permissions and zero appropriate initial files.  have fun

again: $(OBJ) 
	$(CC) $(LDFLAGS) -o $(EXECUTABLE)/conquest $(OBJ) -lcurses -ltermcap
	@echo ALL MADE
	cp .help $(DATA)/.help
	cp .nations $(DATA)/.nations
	chmod 0600 *
	chmod +x MISC $(EXECUTABLE) $(DATA)

$(OBJ): header.h 

lint:
	lint -u $(FILS) 

clobber:
	$(RM) shar1 shar2 shar3 *.o conquest .data core
	$(RM) $(DATA)/conq.* $(EXECUTABLE)/conquest 

clean:
	$(RM) shar1 shar2 shar3 *.o core

flop:
	$(RM) core
	find . -name '*[CrpsEech]' -print | cpio -ocBv > /dev/rfp021

install:
	if test -d !$(EXECUTABLE) ; \
	then mkdir $(EXECUTABLE) ; \
	fi 
	if test -d !$(DATA) ; \
	then mkdir $(DATA) ; \
	fi 
	cp .help $(DATA)/.help
	cp .nations $(DATA)/.nations
	cp run $(DATA)/run
	> $(DATA)/.execute
	> $(DATA)/.messages
	> $(DATA)/.news
	chmod 0600 *
	chmod +x MISC $(EXECUTABLE) $(DATA) $(DATA)/run
	chmod 0666 $(DATA)/.execute
	chmod 0644 $(DATA)/.help $(DATA)/.news $(DATA)/.messages

docs:
	sed -e "s/^END//g" .help | pr

shar:
	makeshar 1 combat.c commands.c forms.c io.c 
	mv shar.out shar1 
	makeshar 2  move.c makeworld.c main.c 
	mv shar.out shar2 
	makeshar 3 $(SUPT1) $(HEADERS) 
	mv shar.out shar3
	makeshar 4 update.c npc.c newlogin.c
	mv shar.out shar4
	makeshar 5 execute.c reports.c magic.c
	mv shar.out shar5

rmshar:
	$(RM) shar1
	$(RM) shar2
	$(RM) shar3
