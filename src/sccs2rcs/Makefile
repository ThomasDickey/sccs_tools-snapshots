# $Header: /users/source/archives/sccs_tools.vcs/src/sccs2rcs/RCS/Makefile,v 1.1 1989/03/29 13:22:10 dickey Exp $
# Top-level makefile for SCCS_TOOLS sccs/RCS conversion utility
#
# $Log: Makefile,v $
# Revision 1.1  1989/03/29 13:22:10  dickey
# Initial revision
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
	rm -f *

install:	all $(ALL)
deinstall:		; rm -f $(ALL)

####### (Details of Productions) ###############################################
bin:				; mkdir $@
src/Makefile:			; cd src;	$(GET) Makefile
test/Makefile:			; cd test;	$(GET) Makefile
$B/$(PROG):	bin/$(PROG)	; cd bin;	$(PUT)
