# $Header: /users/source/archives/sccs_tools.vcs/src/putdelta/src/RCS/Makefile,v 1.5 1989/03/29 13:31:34 dickey Exp $
# Make-file for SCCS check-in utilities
#
####### (Environment) ##########################################################
B	= ../bin
I	= ../../../../cm_tools/interface
L	= ../../../../cm_tools/lib
GET	= checkout
#
####### (Command-line Options) #################################################
INCLUDES= -I$I
CFLAGS  = $(DEFINES) $(INCLUDES) -O
LIBS	= $L/lib.a
DATE	= echo '****'`date` >> $@
LINTOPT	= $(DEFINES) $(INCLUDES) -ltd
#
####### (Standard Lists) #######################################################
ALL	= $B/putdelta $B/sccsput
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
$B/putdelta:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
putdelta.o:	$I/ptypes.h
#
$B/sccsput:	sccsput.sh	; copy $? $@
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
