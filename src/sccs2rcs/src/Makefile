# $Header: /users/source/archives/sccs_tools.vcs/src/sccs2rcs/src/RCS/Makefile,v 2.0 1989/07/10 08:41:13 ste_cm Rel $
# Make-file for sccs/RCS conversion utility
#
# $Log: Makefile,v $
# Revision 2.0  1989/07/10 08:41:13  ste_cm
# BASELINE Mon Jul 10 09:22:04 EDT 1989
#
#	Revision 1.5  89/07/10  08:41:13  dickey
#	corrected 'destroy', 'source' rules
#	
#	Revision 1.4  89/03/29  13:26:02  dickey
#	use (parallel) module CM_TOOLS for library-location
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
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done'
sources:	$(SRCS)
#
####### (Details of Productions) ###############################################
$B/sccs2rcs:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
sccs2rcs.o:	$I/ptypes.h $I/rcsdefs.h
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
