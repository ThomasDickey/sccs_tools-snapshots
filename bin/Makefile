# $Id: Makefile,v 5.0 1991/10/24 08:00:05 ste_cm Rel $
#

####### (Development) ##########################################################
INSTALL_BIN=../../install_bin
COPY	= cp -p
B	= $(INSTALL_BIN)
PUT	= $(COPY) $? $@

####### (Standard Lists) #######################################################
ALL	=\
	$B/getdelta\
	$B/putdelta\
	$B/sccsget\
	$B/sccsput\
	$B/sccs2rcs

####### (Standard Productions) #################################################
all:		; @echo '** use "make install"'
clean:			; rm -f *.bak *.log *.out core
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
