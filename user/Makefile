# $Id: Makefile,v 6.0 1993/04/29 08:35:20 ste_cm Rel $

TOP	= ../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Development) ##########################################################
USE	= man
SECTION	= 1
PREFIX	= $(INSTALL_MAN)/$(USE)$(SECTION)/

####### (Standard Lists) #######################################################
IT	=\
	$(PREFIX)getdelta.$(SECTION)\
	$(PREFIX)putdelta.$(SECTION)\
	$(PREFIX)sccsget.$(SECTION)\
	$(PREFIX)sccsput.$(SECTION)

SOURCES	=\
	getdelta.man\
	putdelta.man\
	sccsget.man\
	sccsput.man

####### (Standard Productions) #################################################
all:	$(SOURCES) $(ALL)	; @echo '** use "make install"'
clean:				; rm -f $(CLEAN)
clobber:	clean		; rm -f $(ALL)
destroy:			; $(DESTROY)
run_test:
sources:	$(SOURCES)
install:	$(IT)
deinstall:			; rm -f $(IT)

####### (Details of Productions) ###############################################
$(PREFIX)getdelta.$(SECTION):	getdelta.$(USE)	; $(PUT)
$(PREFIX)putdelta.$(SECTION):	putdelta.$(USE)	; $(PUT)
$(PREFIX)sccsget.$(SECTION):	sccsget.$(USE)	; $(PUT)
$(PREFIX)sccsput.$(SECTION):	sccsput.$(USE)	; $(PUT)

$(SOURCES):			; $(GET) $@
