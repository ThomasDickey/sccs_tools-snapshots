# $Id: Makefile,v 6.1 1994/07/21 00:33:53 tom Exp $
# make-file for testing sccs delta+admin package

TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
SCRIPTS	=\
	run_test.sh

SOURCES	= $(SCRIPTS)

####### (Standard Productions) #################################################
all:	$(SOURCES) $(ALL)
clean:				; $(RM) $(CLEAN)
clobber:	clean		; -$(RM) $(ALL)
destroy:			; $(DESTROY)
sources:	$(SOURCES)
run_test:	$(SOURCES)	; $(RUN_TESTS)
lint.out:

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
