# $Header: /users/source/archives/sccs_tools.vcs/src/putdelta/src/RCS/Makefile,v 1.4 1988/08/17 09:30:26 dickey Exp $
# Make-file for SCCS check-in utilities
#
####### (Environment) ##########################################################
B	= ../../bin
I	= ../lib
GET	= getdelta
#
####### (Command-line Options) #################################################
INCLUDES= -I$I
CFLAGS  = $(DEFINES) $(INCLUDES) -O
LIBS	= ../lib/lib.a
DATE	= echo '****'`date` >> $@
LINTOPT	= $(DEFINES) $(INCLUDES) -ltd
#
####### (Standard Lists) #######################################################
ALL	= putdelta
CSRC	= putdelta.c
SCRIPTS	= sccsput.sh
SRCS	= $(CSRC) $(SCRIPTS)
MAIN_O	= putdelta.o $(LIBS)
#
####### (Standard Productions) #################################################
all:		$(ALL)
#
clean:				; rm -f *.o *.out
clobber:	clean		; rm -f $(ALL)
destroy:			; rm -f *
sources:	$(SRCS)
#
install:	$B/putdelta\
		$B/sccsput
#
####### (Details of Productions) ###############################################
putdelta:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
#
putdelta.o:	$I/ptypes.h
#
$B/putdelta:	putdelta	; copy -d $@
$B/sccsput:	sccsput.sh	; copy $? $@
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) -s sccs/s.$@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
