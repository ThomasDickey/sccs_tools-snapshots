# $Id: Makefile,v 6.0 1993/04/29 08:35:21 ste_cm Rel $

TOP	= ../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
B	= $(INSTALL_BIN)

IT	=\
	$B/getdelta\
	$B/putdelta\
	$B/sccsget\
	$B/sccsput\
	$B/sccs2rcs

####### (Standard Productions) #################################################
all:	$(SOURCES) $(ALL)	; @echo '** use "make install"'
clean:				; rm -f $(CLEAN)
clobber:	clean		; rm -f [a-z]*
destroy:			; $(DESTROY)
run_test:
sources:
install:	$(IT)
deinstall:			; rm -f $(IT)

####### (Details of Productions) ###############################################
$B/getdelta:	getdelta	; $(PUT)
$B/putdelta:	putdelta	; $(PUT)
$B/sccsget:	sccsget		; $(PUT)
$B/sccsput:	sccsput		; $(PUT)
$B/sccs2rcs:	sccs2rcs	; $(PUT)
