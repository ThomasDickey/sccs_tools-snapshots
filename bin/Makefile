# $Header: /users/source/archives/sccs_tools.vcs/bin/RCS/Makefile,v 3.0 1989/07/10 08:24:49 ste_cm Rel $
#
# $Log: Makefile,v $
# Revision 3.0  1989/07/10 08:24:49  ste_cm
# BASELINE Tue Jun 18 08:04:39 1991 -- apollo sr10.3
#
#	Revision 2.0  89/07/10  08:24:49  ste_cm
#	BASELINE Mon Jul 10 09:15:53 EDT 1989
#	
#	Revision 1.2  89/07/10  08:24:49  dickey
#	corrected 'destroy' rule
#	
#	Revision 1.1  89/03/29  15:01:30  dickey
#	Initial revision
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
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done'
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
