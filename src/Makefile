# $Header: /users/source/archives/sccs_tools.vcs/src/RCS/Makefile,v 1.1 1989/03/29 13:12:06 dickey Exp $
# Top-level makefile for SCCS_TOOLS src-directory
#
# $Log: Makefile,v $
# Revision 1.1  1989/03/29 13:12:06  dickey
# Initial revision
#

####### (Development) ##########################################################
GET	= checkout
MAKE	= make $(MFLAGS) -k$(MAKEFLAGS)	GET=$(GET)

####### (Standard Lists) #######################################################
FIRST	=\
	getdelta/Makefile\
	putdelta/Makefile\
	sccs2rcs/Makefile

####### (Standard Productions) #################################################
all\
clean\
clobber\
rdestroy\
sources\
lint.out\
lincnt.out\
run_tests\
install\
deinstall:	$(FIRST)
	cd getdelta;	$(MAKE) $@
	cd putdelta;	$(MAKE) $@
	cd sccs2rcs;	$(MAKE) $@

destroy: rdestroy	; rm -f *

####### (Details of Productions) ###############################################
getdelta/Makefile:	; cd baseline;	$(GET) Makefile
putdelta/Makefile:	; cd checkin;	$(GET) Makefile
sccs2rcs/Makefile:	; cd checkout;	$(GET) Makefile
