# $Header: /users/source/archives/sccs_tools.vcs/src/getdelta/src/RCS/Makefile,v 1.4 1988/08/17 09:29:10 dickey Exp $
# Make-file for SCCS checkout utilities
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
ALL	= getdelta
CSRC	= getdelta.c
SCRIPTS	= sccsget.sh
SRCS	= $(CSRC) $(SCRIPTS)
MAIN_O	= getdelta.o $(LIBS)
#
####### (Standard Productions) #################################################
all:		$(ALL)
#
clean:				; rm -f *.o *.out
clobber:	clean		; rm -f $(ALL)
destroy:			; rm -f *
sources:	$(SRCS)
#
install:	$B/getdelta\
		$B/sccsget
#
####### (Details of Productions) ###############################################
getdelta:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
#
getdelta.o:	$I/ptypes.h
#
$B/getdelta:	getdelta	; copy -d $@
$B/sccsget:	sccsget.sh	; copy $? $@
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) -s sccs/s.$@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
