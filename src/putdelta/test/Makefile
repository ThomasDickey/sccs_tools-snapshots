# $Id: Makefile,v 5.1 1992/10/19 10:52:27 dickey Exp $
# make-file for testing sccs delta+admin package

TOP	= ../../../..
include $(TOP)/cm_library/support/cm_library.mk

####### (Standard Lists) #######################################################
SCRIPTS	=\
	run_tests.sh

SOURCES	= $(SCRIPTS)

####### (Standard Productions) #################################################
all:	$(SOURCES) $(ALL)
clean:				; rm -f $(CLEAN)
clobber:	clean		; rm -f $(ALL)
destroy:			; $(DESTROY)
sources:	$(SOURCES)
run_tests:	$(SOURCES)	; $(RUN_TESTS)
lint.out:
lincnt.out:

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
