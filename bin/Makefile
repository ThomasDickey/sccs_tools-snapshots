# $Header: /users/source/archives/sccs_tools.vcs/bin/RCS/Makefile,v 1.1 1989/03/29 15:01:30 dickey Exp $
#
# $Log: Makefile,v $
# Revision 1.1  1989/03/29 15:01:30  dickey
# Initial revision
#

####### (Development) ##########################################################
INSTALL_PATH=/ste_site/ste/bin
B	= $(INSTALL_PATH)
PUT	= copy -v -d $@

####### (Standard Lists) #######################################################
ALL	=\
	$B/getdelta\
	$B/putdelta\
	$B/sccsget\
	$B/sccsput\
	$B/sccs2rcs

####### (Standard Productions) #################################################
all:		; @echo '** use "make install"'
clean:
clobber:	clean	; rm -f [a-z]*
destroy:	; rm -f *
run_tests:
sources:
install:	$(ALL)
deinstall:	; rm -f $(ALL)

####### (Details of Productions) ###############################################
$B/getdelta:	getdelta	; $(PUT)
$B/putdelta:	putdelta	; $(PUT)
$B/sccsget:	sccsget		; $(PUT)
$B/sccsput:	sccsput		; $(PUT)
$B/sccs2rcs:	sccs2rcs	; $(PUT)
