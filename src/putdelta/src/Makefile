# @(#)Makefile	1.1 88/07/29 09:17:51
# Make-file for SCCS check-in utilities
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
	putdelta
#
all:	$(ALL)
#
clean:
	rm -f *.out *.o
clobber: clean
	rm -f $(ALL)
destroy:
	rm -f *
#
SRCS	=\
	putdelta.c
#
putdelta:	putdelta.o $(LIBS)
	$(CC) -o $@ putdelta.o $(LIBS)
#
putdelta.o:	$I/ptypes.h	putdelta.c
#
# Install this stuff in public bin:
install:\
	$B/putdelta\
	$B/sccsput
#
$B/putdelta:	putdelta	; rm -f $@; cp $? $@; chmod +x $@
$B/sccsput:	sccsput.sh	; rm -f $@; cp $? $@; chmod +x $@
#
sccsput.sh\
$(SRCS):
	$(GET) -s $@
#
# Development stuff:
lint.out:	$(SRCS)
	echo '****'`date` | tee -a $@
	lintit.sh $? | tee -a $@
lincnt.out:	$(SRCS)
	echo '**** '`date` | tee -a $@
	lincnt $(SRCS) | tee -a $@
