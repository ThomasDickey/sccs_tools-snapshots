# $Header: /users/source/archives/sccs_tools.vcs/src/sccs2rcs/src/RCS/Makefile,v 1.4 1989/03/29 13:26:02 dickey Exp $
# Make-file for sccs/RCS conversion utility
#
# $Log: Makefile,v $
# Revision 1.4  1989/03/29 13:26:02  dickey
# use (parallel) module CM_TOOLS for library-location
#
#	Revision 1.3  89/03/22  14:56:46  dickey
#	use 'rcsedit.o' module in common/lib
#	
#	Revision 1.2  89/03/22  10:14:12  dickey
#	use "ptypes.h" include-file
#	
#	Revision 1.1  89/03/22  08:07:48  dickey
#	Initial revision
#	
#
####### (Environment) ##########################################################
I	= ../../../../cm_tools/interface
L	= ../../../../cm_tools/lib
B	= ../bin
GET	= checkout
#
####### (Command-line Options) #################################################
INCLUDES= -I$I
CFLAGS  = $(DEFINES) $(INCLUDES) -O
DATE	= echo '****'`date` >> $@
LINTOPT	= $(DEFINES) $(INCLUDES) -ltd
#
####### (Standard Lists) #######################################################
ALL	= $B/sccs2rcs
CSRC	= sccs2rcs.c
SRCS	= $(CSRC)
MAIN_O	= sccs2rcs.o $L/lib.a
#
####### (Standard Productions) #################################################
all:		$(ALL)
#
clean:				; rm -f *.o *.out
clobber:	clean		; rm -f $(ALL)
destroy:			; rm -f *
sources:	$(SRCS)
#
####### (Details of Productions) ###############################################
$B/sccs2rcs:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
sccs2rcs.o:	$I/ptypes.h $I/rcsdefs.h
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) -s sccs/s.$@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
