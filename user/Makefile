# $Id: Makefile,v 5.1 1992/10/19 10:58:15 dickey Exp $

TOP	= ../..
include $(TOP)/cm_library/support/cm_library.mk

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
run_tests:
sources:	$(SOURCES)
install:	$(IT)
deinstall:			; rm -f $(IT)

####### (Details of Productions) ###############################################
$(PREFIX)getdelta.$(SECTION):	getdelta.$(USE)	; $(PUT)
$(PREFIX)putdelta.$(SECTION):	putdelta.$(USE)	; $(PUT)
$(PREFIX)sccsget.$(SECTION):	sccsget.$(USE)	; $(PUT)
$(PREFIX)sccsput.$(SECTION):	sccsput.$(USE)	; $(PUT)

$(SOURCES):			; $(GET) $@
