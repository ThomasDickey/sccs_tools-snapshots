# $Header: /users/source/archives/sccs_tools.vcs/src/sccs2rcs/src/RCS/Makefile,v 3.0 1991/06/05 17:08:25 ste_cm Rel $
# Make-file for sccs/RCS conversion utility
#
# $Log: Makefile,v $
# Revision 3.0  1991/06/05 17:08:25  ste_cm
# BASELINE Tue Jun 18 08:04:39 1991 -- apollo sr10.3
#
#	Revision 2.1  91/06/05  17:08:25  dickey
#	mods for baseline
#	
#	Revision 1.5  89/07/10  08:41:25  dickey
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
CPP_OPTS= $(DEFINES) $(INCLUDES)
DATE	= echo '****'`date` >> $@
LINTOPT	= $(CPP_OPTS) -ltd
.c.o:	; $(CC) $(CFLAGS) $(CPP_OPTS) -c $<
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
run_tests:
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
