# $Id: Makefile,v 5.2 1992/10/19 11:48:46 dickey Exp $

TOP	= ../..
include $(TOP)/cm_library/support/cm_library.mk

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
run_tests:
sources:
install:	$(IT)
deinstall:			; rm -f $(IT)

####### (Details of Productions) ###############################################
$B/getdelta:	getdelta	; $(PUT)
$B/putdelta:	putdelta	; $(PUT)
$B/sccsget:	sccsget		; $(PUT)
$B/sccsput:	sccsput		; $(PUT)
$B/sccs2rcs:	sccs2rcs	; $(PUT)
