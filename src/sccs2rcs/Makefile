# $Header: /users/source/archives/sccs_tools.vcs/src/sccs2rcs/RCS/Makefile,v 5.0 1989/07/10 08:40:03 ste_cm Rel $
# Top-level makefile for SCCS_TOOLS sccs/RCS conversion utility
#
# $Log: Makefile,v $
# Revision 5.0  1989/07/10 08:40:03  ste_cm
# BASELINE Mon Jul 20 12:41:28 1992 -- CM_TOOLS #11
#
#	Revision 4.0  89/07/10  08:40:03  ste_cm
#	BASELINE Tue Dec 17 11:56:35 1991
#	
#	Revision 3.0  89/07/10  08:40:03  ste_cm
#	BASELINE Tue Jun 18 08:04:39 1991 -- apollo sr10.3
#	
#	Revision 2.0  89/07/10  08:40:03  ste_cm
#	BASELINE Mon Jul 10 09:23:25 EDT 1989
#	
#	Revision 1.2  89/07/10  08:40:03  dickey
#	corrected 'destroy' rule
#	
#	Revision 1.1  89/03/29  13:22:10  dickey
#	Initial revision
#	
####### (Development) ##########################################################
B	= ../../bin
L	= ../../../cm_tools/lib
PUT	= copy -v -d ../$@
GET	= checkout
MAKE	= make $(MFLAGS) -k$(MAKEFLAGS)	GET=$(GET)

####### (Standard Lists) #######################################################
PROG	= sccs2rcs
ALL	=\
	$B/$(PROG)
FIRST	=\
	bin\
	src/Makefile\
	test/Makefile

####### (Standard Productions) #################################################
all:		$L/lib.a

all\
clean\
clobber\
run_tests\
sources\
lincnt.out\
lint.out:	$(FIRST)
	cd src;		$(MAKE) $@
	cd test;	$(MAKE) $@

rdestroy\
destroy:	$(FIRST) clobber
	cd src;		$(MAKE) destroy
	cd test;	$(MAKE) destroy
	rmdir bin
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done'

install:	all $(ALL)
deinstall:		; rm -f $(ALL)

####### (Details of Productions) ###############################################
bin:				; mkdir $@
src/Makefile:			; cd src;	$(GET) Makefile
test/Makefile:			; cd test;	$(GET) Makefile
$B/$(PROG):	bin/$(PROG)	; cd bin;	$(PUT)
