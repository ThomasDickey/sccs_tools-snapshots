# @(#)Makefile	1.1 88/07/29 09:51:53
# Make-file for SCCS/RCS checkin utilities
B	= ../../bin
I	= ../lib
GET	= getdelta
#
INCLUDES= -I$I
CFLAGS  = $(DEFINES) $(INCLUDES) -O
LIBS	=\
	../lib/lib.a
#
ALL	=\
	getdelta
#
all:	$(ALL)
#
clean:
	rm -f *.out *.o
clobber: clean
	rm -f $(ALL)
#
SRCS	=\
	getdelta.c
#
#
getdelta:	getdelta.o $(LIBS)
	$(CC) -o $@ getdelta.o $(LIBS)
#
getdelta.o:	$I/ptypes.h	getdelta.c
#
# Install this stuff in public bin:
install:\
	$B/getdelta\
	$B/sccsget
#
$B/getdelta:	getdelta	; rm -f $@; cp $? $@; chmod +x $@
$B/sccsget:	sccsget.sh	; rm -f $@; cp $? $@; chmod +x $@
#
# Development stuff:
lint.out:	$(SRCS)
	echo '****'`date` | tee -a $@
	lintit.sh $? | tee -a $@
lincnt.out:	$(SRCS)
	echo '**** '`date` | tee -a $@
	lincnt $(SRCS) | tee -a $@
