# $Header: /users/source/archives/sccs_tools.vcs/src/getdelta/src/RCS/Makefile,v 1.5 1989/03/29 14:13:59 dickey Exp $
# Make-file for SCCS checkout utilities
#
####### (Environment) ##########################################################
B	= ../bin
I	= ../../../../cm_tools/interface
L	= ../../../../cm_tools/lib
GET	= getdelta
#
####### (Command-line Options) #################################################
INCLUDES= -I$I
CFLAGS  = $(DEFINES) $(INCLUDES) -O
LIBS	= $L/lib.a
DATE	= echo '****'`date` >> $@
LINTOPT	= $(DEFINES) $(INCLUDES) -ltd
#
####### (Standard Lists) #######################################################
ALL	= $B/getdelta $B/sccsget
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
$B/getdelta:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
getdelta.o:	$I/ptypes.h
#
$B/sccsget:	sccsget.sh	; copy $? $@
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) -s sccs/s.$@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
